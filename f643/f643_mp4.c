/*
 * file    : f643_mp4.c
 * project : f640
 *
 * Created on: Aug 25, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <string.h>


#include "f643.h"

//
struct f643 *f643_alloc(int width, int height, float fps) {
    struct f643 *f643 = calloc(1, sizeof(struct f643));
    f643->width  = width;
    f643->height = height;
    f643->fps    = fps;
    return f643;
}

void f643_free(struct f643 **f643) {
    if (*f643) free(*f643);
    *f643 = NULL;
}

// MP4
void f643_init_mp4(struct f643 *f643, uint32_t timescale) {
    f643->timescale = timescale;
}


// X264
void f643_init_x264(struct f643 *f643, int preset, int tune) {
    f643->preset = preset;
    f643->tune   = tune;
}

int f643_setup_x264_codec(struct f643 *f643) {
    int r;

    r = x264_param_default_preset(&f643->param, x264_preset_names[f643->preset], x264_tune_names[f643->tune]);
    //
    f643->param.i_width = f643->width;
    f643->param.i_height = f643->height;
    f643->param.vui.i_sar_width = 1;
    f643->param.vui.i_sar_height = 1;
    f643->param.i_timebase_num = 1;
    f643->param.i_timebase_den = f643->fps;
    f643->param.i_csp = X264_CSP_I420;
    f643->param.i_fps_num = f643->fps;
    f643->param.i_fps_den = 1;
    // From cmp
    f643->param.vui.b_fullrange = 0;
    f643->param.b_repeat_headers = 0;
    f643->param.b_annexb = 0;
    f643->param.b_vfr_input = 0;
    //
    f643->param.i_threads = 1;
    //
    r = x264_param_apply_profile(&f643->param, "high444");
    FOG("x264_param_apply_profile return %d", r);
    //
    //
    if (f643->pix_in == PIX_FMT_BGR24) {
            r = x264_picture_alloc(&f643->rgb, X264_CSP_BGR, f643->width, f643->height);
            f643->swsCtxt = sws_getCachedContext(NULL,
                    f643->width, f643->height, PIX_FMT_BGR24,
                    f643->width, f643->height, PIX_FMT_YUV420P,
                        SWS_BICUBIC, NULL, NULL, NULL
            );
    } else if (f643->pix_in == PIX_FMT_BGRA) {
            r = x264_picture_alloc(&f643->rgb, X264_CSP_BGRA, f643->width, f643->height);
            f643->swsCtxt = sws_getCachedContext(NULL,
                    f643->width, f643->height, PIX_FMT_BGRA,
                    f643->width, f643->height, PIX_FMT_YUV420P,
                        SWS_BICUBIC, NULL, NULL, NULL
            );
    }
//    free(f643->rgb.img.plane[0]);
//    f643->rgb.img.plane[0] = NULL;
    FOG("f-X264 stride in : %d", f643->rgb.img.i_stride[0]);
    FOG("f-X264 x264_picture_alloc return %d", r);
    r = x264_picture_alloc(&f643->yuv, X264_CSP_I420, f643->width, f643->height);
    FOG("f-X264 oStride = %d , %d, %d, %d", f643->yuv.img.i_stride[0], f643->yuv.img.i_stride[1], f643->yuv.img.i_stride[2], f643->yuv.img.i_stride[3]);
    FOG("f-X264 x264_picture_alloc return %d", r);
    //
    f643->x264 = NULL;
    f643->frame = 0;
    f643->pts = f643->pts0 = LONG_MIN;
    f643->dts = f643->dts0 = 0;
    return 0;
}

static long temp = 155;
int f643_add_x264_frame(struct f643 *f643) {
    x264_picture_t ipb;
    x264_nal_t *nal;
    int i_nal, i_coded_bytes;
    FOG("Starting");
    if (temp > 0) {
        //
        //f643->rgb.img.plane[0] = malloc(3 * f643->width * f643->height);
        memset(f643->rgb.img.plane[0], temp, 3 * f643->width * f643->height);
        int r = sws_scale(f643->swsCtxt, (const uint8_t **const)f643->rgb.img.plane, f643->rgb.img.i_stride, 0, f643->height, f643->yuv.img.plane, f643->yuv.img.i_stride);
        FOG("SwScale return %d", r);
        //free(f643->rgb.img.plane[0]);
        if (r < f643->height) return -3;
        f643->yuv.i_pts = f643->frame++;
        i_coded_bytes = x264_encoder_encode(f643->x264, &nal, &i_nal, &f643->yuv, &ipb );
        FOG("Encode return %d / %d for pts = %ld, dts = %ld / pts = %ld, dts = %ld", i_coded_bytes, i_nal, ipb.i_pts, ipb.i_dts, f643->yuv.i_pts, f643->yuv.i_dts);
    } else {
        //
        i_coded_bytes = x264_encoder_encode(f643->x264, &nal, &i_nal, NULL, &ipb);
        FOG("Encode return %d / %d for pts = %ld, dts = %ld", i_coded_bytes, i_nal, ipb.i_pts, ipb.i_dts);
    }
    temp--;
    if (temp < 0) return -2;
    if (i_coded_bytes < 0) {
        FOG("End encoding (return %d)", i_coded_bytes);
        return -1;
    }
    if (i_coded_bytes == 0) {
        FOG("Frame buffered");
        return 0;
    }
    if (ipb.i_pts > f643->pts) {
        if (f643->pts > f643->pts0) f643->pts0 = f643->pts;
        f643->pts  = ipb.i_pts;
    }
    if (ipb.i_pts > f643->pts0 && ipb.i_pts < f643->pts) {
        f643->pts0  = ipb.i_pts;
    }
    f643->dts = ipb.i_dts;

    if (f643->sei) {
        uint8_t *tmp = malloc(f643->lsei + i_coded_bytes);
        memcpy(tmp, f643->sei, f643->lsei);
        memcpy(tmp + f643->lsei, nal[0].p_payload, i_coded_bytes);
        bool b = MP4WriteSample(f643->handle, f643->x264_track, tmp, f643->lsei + i_coded_bytes, 4500, 4500 * (ipb.i_pts - ipb.i_dts), true);
        FOG("MP4WriteSample0 return %d", b);
        free(tmp);
        free(f643->sei);
        f643->sei  = NULL;
        f643->lsei = 0;
    } else {
        bool b = MP4WriteSample(f643->handle, f643->x264_track, nal[0].p_payload, i_coded_bytes, 4500, 4500 * (ipb.i_pts - ipb.i_dts), true);
        FOG("MP4WriteSample return %d", b);
    }

    return i_coded_bytes;
}

int f643_close_x264_codec(struct f643 *f643) {
    //
    int nr = 100;
    while(x264_encoder_delayed_frames(f643->x264) && nr) {
        if (f643_add_x264_frame(f643) < 0) break;
        nr--;
    }
    FOG("Got %d delayed frames", 100 - nr);
    // Picture
    f643->rgb.img.plane[0] = NULL;
    x264_picture_clean(&f643->rgb);
    x264_picture_clean(&f643->yuv);
    sws_freeContext(f643->swsCtxt);
    //
    x264_encoder_close(f643->x264);
    f643->x264 = NULL;
    //
    FOG("X264 closed");
    return 0;
}

//
int f643_open_file(const char *path, struct f643 *f643) {
    FOG("starting");
    //
    f643->handle = MP4CreateEx(path, 0, 1, 1, NULL, 0, NULL, 0);
    FOG("MP4CreateEx return %p", f643->handle);

    return 0;
}


int f643_open_x264_track(struct f643 *f643) {
    // x264
    f643->x264 = x264_encoder_open(&f643->param);
    FOG("X264 opened : %p", f643->x264);
    x264_encoder_parameters(f643->x264, &f643->param);
    // headers
    x264_nal_t *p_nal;
    int i_nal;
    x264_encoder_headers(f643->x264, &p_nal, &i_nal);
    // From matroska & flv
    uint8_t *sps = p_nal[0].p_payload + 4 ; int lsps = p_nal[0].i_payload - 4;
    uint8_t *pps = p_nal[1].p_payload + 4 ; int lpps = p_nal[1].i_payload - 4;
    uint8_t *sei = p_nal[2].p_payload ;     int lsei = p_nal[2].i_payload;
    f643->lsei   = lsei;
    f643->sei    = malloc(lsei);
    memcpy(f643->sei, sei, lsei);

    //
    FILE *filp = fopen("sps.dat", "wb");
    fwrite(p_nal[0].p_payload, 1, p_nal[0].i_payload, filp);
    fflush(filp); fclose(filp);
    //
    filp = fopen("pps.dat", "wb");
    fwrite(p_nal[1].p_payload, 1, p_nal[1].i_payload, filp);
    fflush(filp); fclose(filp);
    //
    filp = fopen("sei.dat", "wb");
    fwrite(p_nal[2].p_payload, 1, p_nal[2].i_payload, filp);
    fflush(filp); fclose(filp);

    int lpcodec = 5 + 1 + 2 + lsps + 1 + 2 + lpps;
    uint8_t *pcodec = (uint8_t*)malloc(lpcodec);

    pcodec[0] = 1;
    pcodec[1] = sps[1];
    pcodec[2] = sps[2];
    pcodec[3] = sps[3];
    pcodec[4] = 0xff; // nalu size length is four bytes
    pcodec[5] = 0xe1; // one sps
    pcodec[6] = lsps >> 8;
    pcodec[7] = lsps;
    memcpy(pcodec + 8, sps, lsps);

    pcodec[8 + lsps] = 1; // one pps
    pcodec[9 + lsps] = lpps >> 8;
    pcodec[10 + lsps] = lpps;
    memcpy(pcodec + 11 + lsps, pps, lpps);

    // mp4
    f643->x264_track = MP4AddH264VideoTrack(f643->handle
            , f643->timescale, MP4_INVALID_DURATION
            , f643->width, f643->height
            , sps[1]    // ?version = 1
            , sps[2]
            , sps[3]
            , 3         // ??
    );
//    f643->x264_track = MP4AddVideoTrack(f643->handle
//            , f643->timescale, MP4_INVALID_DURATION
//            , f643->width, f643->height, MP4_MPEG4_VIDEO_TYPE);
    FOG("MP4AddH264VideoTrack return %u", f643->x264_track);

    //
    bool b = MP4WriteSample(f643->handle, f643->x264_track, pcodec, lpcodec, 0, 0, true);
    FOG("MP4WriteSample return %d", b);
    //
//    bool b = MP4SetTrackBytesProperty(f643->handle, f643->x264_track, "options", pcodec, lpcodec);
//    FOG("MP4SetBytesProperty return %d", b);
    free(pcodec);


    return 0;
}


int f643_close_file(struct f643 *f643) {
    //
    bool b = MP4MakeIsmaCompliant(MP4GetFilename(f643->handle), true);
    FOG("MP4MakeIsmaCompliant %d", b);
    //
    MP4Close(f643->handle, 0);
    FOG("MP4Close");
    //
    return 0;
}

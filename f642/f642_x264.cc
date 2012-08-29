/*
 * file    : f642_x264.cc
 * project : f640
 *
 * Created on: Aug 22, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <stdlib.h>

#include "f642_x264.hh"

//using namespace t508::f642;

// Encoding
t508::f642::X264::X264(int width, int height, enum PixelFormat pix_in, float fps, int preset, int tune) {
    int r;
    this->width = width;
    this->height = height;
    this->fps = fps;
    this->logLevel = 0;
    // Parameters
    r = x264_param_default_preset(&param, x264_preset_names[preset], x264_tune_names[tune]);
    if (logLevel) fprintf(stderr, "f-X264 x264_param_default_preset return %d\n", r);
    //
    param.i_width = width;
    param.i_height = height;
    param.vui.i_sar_width = 1;
    param.vui.i_sar_height = 1;
    param.i_timebase_num = 1;
    param.i_timebase_den = fps;
    param.i_csp = X264_CSP_I420;
    param.i_fps_num = fps;
    param.i_fps_den = 1;
    // From cmp
    param.vui.b_fullrange = 0;
    param.b_repeat_headers = 0;
    param.b_annexb = 0;
    param.b_vfr_input = 0;
    //
    param.i_threads = 1;

    r = x264_param_apply_profile(&param, "high444");
    if (logLevel) fprintf(stderr, "f-X264 x264_param_apply_profile return %d\n", r);
    //
    if (pix_in == PIX_FMT_BGR24) {
            r = x264_picture_alloc(&rgb, X264_CSP_BGR, width, height);
            swsCtxt = sws_getCachedContext(NULL,
                        width, height, PIX_FMT_BGR24,
                        width, height, PIX_FMT_YUV420P,
                        SWS_BICUBIC, NULL, NULL, NULL
            );
    } else if (pix_in == PIX_FMT_BGRA) {
            r = x264_picture_alloc(&rgb, X264_CSP_BGRA, width, height);
            swsCtxt = sws_getCachedContext(NULL,
                        width, height, PIX_FMT_BGRA,
                        width, height, PIX_FMT_YUV420P,
                        SWS_BICUBIC, NULL, NULL, NULL
            );
    }
    free(rgb.img.plane[0]);
    rgb.img.plane[0] = NULL;
    if (logLevel) fprintf(stderr, "f-X264 stride in : %d\n", rgb.img.i_stride[0]);
    if (logLevel) fprintf(stderr, "f-X264 sws_getCachedContext return %p\n", swsCtxt);
    if (logLevel) fprintf(stderr, "f-X264 x264_picture_alloc return %d\n", r);
    r = x264_picture_alloc(&yuv, X264_CSP_I420, width, height);
    if (logLevel) fprintf(stderr, "f-X264 oStride = %d , %d, %d, %d\n", yuv.img.i_stride[0], yuv.img.i_stride[1], yuv.img.i_stride[2], yuv.img.i_stride[3]);
    if (logLevel) fprintf(stderr, "f-X264 x264_picture_alloc return %d\n", r);
    //
    x264 = NULL;
    frame = 0;
    pts = pts0 = LONG_MIN;
    dts = dts0 = 0;
    //
    if (logLevel) fprintf(stderr, "X264 constructed\n");
}

t508::f642::X264::~X264() {
}

//
int t508::f642::X264::open(const char *path, int muxer) {
    x264_nal_t *pp_nal;
    int pi_nal;

    if (x264) return -1;
    //
    x264 = x264_encoder_open(&param);
    x264_encoder_parameters(x264, &param);
    //
    cli_output_opt_t opt;
    opt.use_dts_compress = 0;
    switch(muxer) {
        case 0 :
            output = &raw_output;
            break;
        case 1 :
            output = &mkv_output;
            break;
        case 2 :
            output = &flv_output;
            break;
        default :
            output = &raw_output;
    }
    output->open_file((char*)path, &outh, &opt);
    output->set_param(outh, &param);
    //
    x264_encoder_headers(x264, &pp_nal, &pi_nal);
    output->write_headers(outh, pp_nal);
    //
    if (logLevel) fprintf(stderr, "X264 opened : %p\n", x264);
    return 0;
}

int t508::f642::X264::close() {
    //
    int nr = 60;
    while(x264_encoder_delayed_frames(x264) && nr) {
        if (addFrame(NULL) < 0) break;
        nr--;
    }
    // File
    output->close_file(outh, pts, pts0);
    if (logLevel) fprintf(stderr, "Close file with %ld - %ld\n", pts0, pts);
    outh = NULL;
    // Picture
    rgb.img.plane[0] = rgb.img.plane[3];
    rgb.img.plane[3] = NULL;
    x264_picture_clean(&rgb);
    x264_picture_clean(&yuv);
    sws_freeContext(swsCtxt);
    //
    x264_encoder_close(x264);
    x264 = NULL;
    //
    if (logLevel) fprintf(stderr, "X264 closed\n");
    return 0;
}

int t508::f642::X264::setQP(int qp) {
    if (!x264) {
        param.rc.i_qp_constant = qp;
        return 1;
    }
    // ?
    x264_encoder_parameters(x264, &param);
    param.rc.i_qp_constant = qp;
    if (x264_encoder_reconfig(x264, &param) < 0) return -1;
    return 1;
}

int t508::f642::X264::setQP(int qpmin, int qpmax, int qpstep) {
    if (!x264) {
        param.rc.i_qp_min  = qpmin;
        param.rc.i_qp_max  = qpmax;
        param.rc.i_qp_step = qpstep;
        return 1;
    }
    // ?
    x264_encoder_parameters(x264, &param);
    param.rc.i_qp_min  = qpmin;
    param.rc.i_qp_max  = qpmax;
    param.rc.i_qp_step = qpstep;
    if (x264_encoder_reconfig(x264, &param) < 0) return -1;
    return 1;
}

int t508::f642::X264::setParam(const char *name, const char *value) {
    if (!x264) return x264_param_parse(&param, name, value);
    // ?
    x264_encoder_parameters(x264, &param);
    if (x264_param_parse(&param, name, value) < 0) return -1;
    if (x264_encoder_reconfig(x264, &param) < 0) return -1;
    return 1;
}

int t508::f642::X264::setNbThreads(int nb) {
    if (!x264) {
        param.i_threads = nb;
        return 1;
    }
    // ?
    x264_encoder_parameters(x264, &param);
    param.i_threads = nb;
    if (x264_encoder_reconfig(x264, &param) < 0) return -1;
    return 1;
}

int t508::f642::X264::addFrame(uint8_t *img) {
    int r;
    if (!x264) return -1;
    //
    x264_picture_t ipb;
    x264_nal_t *nal;
    int i_nal;
    if (img) {
        //
        rgb.img.plane[0] = img;
        r = sws_scale(swsCtxt, (const uint8_t **const)rgb.img.plane, rgb.img.i_stride, 0, height, yuv.img.plane, yuv.img.i_stride);
        if (logLevel) fprintf(stderr, "SwScale return %d\n", r);
        if (r < height) return -3;
        yuv.i_pts = frame++;
        r = x264_encoder_encode(x264, &nal, &i_nal, &yuv, &ipb );
        if (logLevel) fprintf(stderr, "Encode return %d for pts = %ld, dts = %ld / pts = %ld, dts = %ld\n", r, ipb.i_pts, ipb.i_dts, yuv.i_pts, yuv.i_dts);
    } else {
        //
        r = x264_encoder_encode(x264, &nal, &i_nal, NULL, &ipb);
        if (logLevel) fprintf(stderr, "x264_encoder_encode return %d for pts = %ld, dts = %ld\n", r, ipb.i_pts, ipb.i_dts);
    }
    if (r < 0) {
        //x264_picture_clean(&ipb);
        if (logLevel) fprintf(stderr, "Pb encoding frame %d\n", r);
        return -1;
    } else if (r == 0) {
        //x264_picture_clean(&ipb);
        if (logLevel) fprintf(stderr, "Encoding frame return 0 : buffered\n");
        return 0;
    }
    //
    r = output->write_frame(outh, nal[0].p_payload, r, &ipb);
    //x264_picture_clean(&ipb);
    if (logLevel) fprintf(stderr, "write_frame return %d\n", r);
    if (r > 0) {
        if (ipb.i_pts > pts) {
            if (pts > pts0) pts0 = pts;
            pts  = ipb.i_pts;
        }
        if (ipb.i_pts > pts0 && ipb.i_pts < pts) {
            pts0  = ipb.i_pts;
        }
    }
    return r;
}

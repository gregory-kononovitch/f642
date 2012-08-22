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
t508::f642::X264::X264(int width, int height, float fps, int preset, int tune) {
    int r;
    this->width = width;
    this->height = height;
    this->fps = fps;
    // Parameters
    r = x264_param_default_preset(&param, x264_preset_names[preset], x264_tune_names[tune]);
    fprintf(stderr, "f-X264 x264_param_default_preset return %d\n", r);

    param.i_width = width;
    param.i_height = height;
    param.vui.i_sar_width = 1;
    param.vui.i_sar_height = 1;
    param.i_timebase_num = 1;
    param.i_timebase_den = fps;
    param.i_csp = X264_CSP_I420;
    param.i_fps_num = fps;
    param.i_fps_den = 1;
    r = x264_param_apply_profile(&param, "high444");
    fprintf(stderr, "f-X264 x264_param_apply_profile return %d\n", r);
    //
    r = x264_picture_alloc(&rgb, X264_CSP_BGR, width, height);
    fprintf(stderr, "f-X264 iStride = %d\n", rgb.img.i_stride[0]);
    free(rgb.img.plane[0]);
    rgb.img.plane[0] = NULL;
    fprintf(stderr, "f-X264 x264_picture_alloc return %d\n", r);
//    memset(iPlan, 0, sizeof(iPlan));
//    memset(iStride, 0, sizeof(iStride));
//    iStride[0] = 3 * width * height;
    r = x264_picture_alloc(&yuv, X264_CSP_I420, width, height);
    fprintf(stderr, "f-X264 oStride = %d , %d, %d, %d\n", yuv.img.i_stride[0], yuv.img.i_stride[1], yuv.img.i_stride[2], yuv.img.i_stride[3]);
    fprintf(stderr, "f-X264 x264_picture_alloc return %d\n", r);
//    memset(oPlan, 0, sizeof(oPlan));
//    oPlan[0] = yuv.img.plane[0];
//    oPlan[1] = yuv.img.plane[1];
//    oPlan[2] = yuv.img.plane[2];
//    memset(oStride, 0, sizeof(oStride));
//    oStride[0] = yuv.img.i_stride[0];
//    oStride[1] = yuv.img.i_stride[1];
//    oStride[2] = yuv.img.i_stride[2];
    //
    swsCtxt = sws_getCachedContext(NULL,
                width, height, PIX_FMT_BGR24,
                width, height, PIX_FMT_YUV420P,
                SWS_BICUBIC, NULL, NULL, NULL
    );
    fprintf(stderr, "f-X264 sws_getCachedContext return %p\n", swsCtxt);
    //
    x264 = NULL;
    frame = 0;
    pts = pts0 = 0;
    dts = dts0 = 0;
    //
    fprintf(stderr, "X264 constructed\n");
}

t508::f642::X264::~X264() {
}

//
int t508::f642::X264::open(char *path) {
    x264_nal_t *pp_nal;
    int pi_nal;

    if (x264) return -1;
    //
    x264 = x264_encoder_open(&param);
    x264_encoder_parameters(x264, &param);
    //
    cli_output_opt_t opt;
    opt.use_dts_compress = 0;
    output = &flv_output;
    output->open_file(path, &outh, &opt);
    output->set_param(outh, &param);
    //
    x264_encoder_headers(x264, &pp_nal, &pi_nal);
    output->write_headers(outh, pp_nal);
    //
    fprintf(stderr, "X264 opened : %p\n", x264);
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
    fprintf(stderr, "X264 closed\n");
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

int t508::f642::X264::addFrame(uint8_t *img) {
    int r;
    if (!x264) return -1;
    //
    fprintf(stderr, "Add frame starts with %p / %p\n", img, &rgb.img);
    x264_picture_t ibp;
    x264_nal_t *nal;
    int i_nal;
    if (img) {
        //
        rgb.img.plane[0] = img;
        r = sws_scale(swsCtxt, (const uint8_t **const)rgb.img.plane, rgb.img.i_stride, 0, height, yuv.img.plane, yuv.img.i_stride);
        //r = sws_scale(swsCtxt, (const uint8_t **const)iPlan, iStride, 0, height, oPlan, oStride);
        fprintf(stderr, "SwScale return %d\n", r);
        if (r < height) return -3;
        yuv.i_pts = frame++;
        r = x264_encoder_encode(x264, &nal, &i_nal, &yuv, &ibp );
        fprintf(stderr, "x264_encoder_encode return %d for pts = %ld, dts = %ld / pts = %ld, dts = %ld\n", r, ibp.i_pts, ibp.i_dts, yuv.i_pts, yuv.i_dts);
    } else {
        //
        r = x264_encoder_encode(x264, &nal, &i_nal, NULL, &ibp);
        fprintf(stderr, "x264_encoder_encode return %d for pts = %ld, dts = %ld\n", r, ibp.i_pts, ibp.i_dts);
    }
    if (r < 0) {
        fprintf(stderr, "Pb encoding frame\n");
        return -1;
    } else if (r == 0) {
        fprintf(stderr, "Encoding frame return 0...\n");
        return 0;
    }
    //
    r = output->write_frame(outh, &nal->p_payload[0], i_nal, &ibp);
    fprintf(stderr, "write_frame return %d\n", r);
    if (r > 0) {
        pts0 = pts;
        pts  = ibp.i_pts;
        dts0 = dts;
        dts  = ibp.i_dts;
    }
    return r;
}

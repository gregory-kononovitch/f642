/*
 * file    : f642_x264.c
 * project : f640
 *
 * Created on: Sep 25, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#include <libavutil/pixdesc.h>
#include <libswscale/swscale.h>
#include <x264.h>
#include "muxers/output.h"

// Muxers
#define RAW_MUXER = 0
#define MKV_MUXER = 1
#define FLV_MUXER = 2

#include "f642_x264.h"

// Encoding
f642x264 *init642_x264(int width, int height, float fps, int preset, int tune) {
    int r;
    f642x264 *x264 = calloc(1, sizeof(f642x264));

    x264->width = width;
    x264->height = height;
    x264->fps = fps;
    x264->loglevel = 1;
    // Parameters
    r = x264_param_default_preset(&x264->param, x264_preset_names[preset], x264_tune_names[tune]);
    if (x264->loglevel) fprintf(stderr, "f-X264 x264_x264->param_default_preset return %d\n", r);
    //
    x264->param.i_width = width;
    x264->param.i_height = height;
    x264->param.vui.i_sar_width = 1;
    x264->param.vui.i_sar_height = 1;
    x264->param.i_timebase_num = 1;
    x264->param.i_timebase_den = fps;
    x264->param.i_csp = X264_CSP_I422;
    x264->param.i_fps_num = fps;
    x264->param.i_fps_den = 1;
    // From cmp
    x264->param.vui.b_fullrange = 0;
    x264->param.b_repeat_headers = 0;
    x264->param.b_annexb = 0;
    x264->param.b_vfr_input = 0;
    //
    x264->param.i_threads = 1;

    r = x264_param_apply_profile(&x264->param, "high444");
    if (x264->loglevel) fprintf(stderr, "f-X264 x264_x264->param_apply_profile return %d\n", r);
    //
    r = x264_picture_alloc(&x264->yuv, X264_CSP_I422, width, height);
    if (x264->loglevel) fprintf(stderr, "f-X264 oStride = %d , %d, %d, %d\n", x264->yuv.img.i_stride[0], x264->yuv.img.i_stride[1], x264->yuv.img.i_stride[2], x264->yuv.img.i_stride[3]);
    if (x264->loglevel) fprintf(stderr, "f-X264 x264_picture_alloc return %d\n", r);
    //
    x264->frame = 0;
    x264->pts = x264->pts0 = LONG_MIN;
    x264->dts = x264->dts0 = 0;
    //
    x264->x264 = NULL;
    if (x264->loglevel) fprintf(stderr, "X264 constructed\n");
    return x264;
}


int f642_open(f642x264 *x264, const char *path, int muxer) {
    x264_nal_t *pp_nal;
    int pi_nal;

    if (x264->x264) return -1;
    //
    x264->x264 = x264_encoder_open(&x264->param);
    x264_encoder_parameters(x264->x264, &x264->param);
    //
    cli_output_opt_t opt;
    opt.use_dts_compress = 0;
    switch(muxer) {
        case 0 :
            x264->output = &raw_output;
            break;
        case 1 :
            x264->output = &mkv_output;
            break;
        case 2 :
            x264->output = &flv_output;
            break;
        default :
            x264->output = &raw_output;
    }
    x264->output->open_file((char*)path, &x264->outh, &opt);
    x264->output->set_param(x264->outh, &x264->param);
    //
    x264_encoder_headers(x264->x264, &pp_nal, &pi_nal);
    x264->output->write_headers(x264->outh, pp_nal);
    //
    if (x264->loglevel) fprintf(stderr, "X264 opened : %p\n", x264);
    fprintf(stderr, "X264 opened : %p\n", x264);
    return 0;
}

int f642_close(f642x264 *x264) {
    //
    int nr = 100;
    struct timeval tv;
    while(x264_encoder_delayed_frames(x264->x264) && nr) {
        if (f642_addFrame(x264, NULL, tv) < 0) break;
        nr--;
    }
    // File
    x264->output->close_file(x264->outh, x264->pts, x264->pts0);
    if (x264->loglevel) fprintf(stderr, "Close file with %ld - %ld\n", x264->pts0, x264->pts);
    x264->outh = NULL;
    x264_picture_clean(&x264->yuv);

    //
    x264_encoder_close(x264->x264);
    x264->x264 = NULL;
    //
    if (x264->loglevel) fprintf(stderr, "X264 closed\n");

    return 0;
}

int f642_setQP(f642x264 *x264, int qp) {
    if (!x264->x264) {
        x264->param.rc.i_qp_constant = qp;
        return 1;
    }
    // ?
    x264_encoder_parameters(x264->x264, &x264->param);
    x264->param.rc.i_qp_constant = qp;
    if (x264_encoder_reconfig(x264->x264, &x264->param) < 0) return -1;
    return 1;
}

int f642_setQPb(f642x264 *x264, int qpmin, int qpmax, int qpstep) {
    if (!x264->x264) {
        x264->param.rc.i_qp_min  = qpmin;
        x264->param.rc.i_qp_max  = qpmax;
        x264->param.rc.i_qp_step = qpstep;
        return 1;
    }
    // ?
    x264_encoder_parameters(x264->x264, &x264->param);
    x264->param.rc.i_qp_min  = qpmin;
    x264->param.rc.i_qp_max  = qpmax;
    x264->param.rc.i_qp_step = qpstep;
    if (x264_encoder_reconfig(x264->x264, &x264->param) < 0) return -1;
    return 1;
}

int f642_setParam(f642x264 *x264, const char *name, const char *value) {
    if (!x264->x264) return x264_param_parse(&x264->param, name, value);
    // ?
    x264_encoder_parameters(x264->x264, &x264->param);
    if (x264_param_parse(&x264->param, name, value) < 0) return -1;
    if (x264_encoder_reconfig(x264->x264, &x264->param) < 0) return -1;
    return 1;
}

int f642_setNbThreads(f642x264 *x264, int nb) {
    if (!x264->x264) {
        x264->param.i_threads = nb;
        return 1;
    }
    // ?
    x264_encoder_parameters(x264->x264, &x264->param);
    x264->param.i_threads = nb;
    if (x264_encoder_reconfig(x264->x264, &x264->param) < 0) return -1;
    return 1;
}

int f642_addFrame(f642x264 *x264, x264_picture_t *yuv, struct timeval tv) {
    int r;
    long tmsp = 90000 * tv.tv_sec + 90000 * tv.tv_usec / 1000000;
    if (!x264->x264) return -1;

    //
    x264_picture_t ipb;
    x264_nal_t *nal;
    int i_nal;
    if (yuv) {
        yuv->i_pts = x264->frame++;
        r = x264_encoder_encode(x264->x264, &nal, &i_nal, yuv, &ipb );
        if (x264->loglevel > 2) fprintf(stderr, "Encode return %d for pts = %ld, dts = %ld / pts = %ld, dts = %ld\n", r, ipb.i_pts, ipb.i_dts, yuv->i_pts, yuv->i_dts);
    } else {
        //
        r = x264_encoder_encode(x264->x264, &nal, &i_nal, NULL, &ipb);
        if (x264->loglevel > 2) fprintf(stderr, "x264_encoder_encode return %d for pts = %ld, dts = %ld\n", r, ipb.i_pts, ipb.i_dts);
    }
    if (r < 0) {
        //x264_picture_clean(&ipb);
        if (x264->loglevel > 2) fprintf(stderr, "Pb encoding frame %d\n", r);
        return -1;
    } else if (r == 0) {
        //x264_picture_clean(&ipb);
        if (x264->loglevel > 2) fprintf(stderr, "Encoding frame return 0 : buffered\n");
        return 0;
    }
    //
    r = x264->output->write_frame(x264->outh, nal[0].p_payload, r, &ipb);
    //x264_picture_clean(&ipb);
    if (x264->loglevel > 2) fprintf(stderr, "write_frame return %d\n", r);
    if (r > 0) {
        if (ipb.i_pts > x264->pts) {
            if (x264->pts > x264->pts0) x264->pts0 = x264->pts;
            x264->pts  = ipb.i_pts;
        }
        if (ipb.i_pts > x264->pts0 && ipb.i_pts < x264->pts) {
            x264->pts0  = ipb.i_pts;
        }
    }
    return r;
}

queue642 *queue_open642(f642x264 *x264, int nb) {
    int i, r;
    nb = 3;
    queue642 *q = (queue642*)calloc(1, sizeof(queue642));
    //q->yuv = (x264_picture_t**)calloc(nb, sizeof(x264_picture_t*));
    q->index1 = (int*)calloc(nb, sizeof(int));
    q->index2 = (int*)calloc(nb, sizeof(int));
    q->tvs = (struct timeval*)calloc(nb, sizeof(struct timeval));

    q->nb_yuv = nb;
    //
    for(i = 0 ; i < nb ; i++) {
        r = x264_picture_alloc(&q->yuv[i], X264_CSP_I422, x264->width, x264->height);
        q->yuv[i].opaque = x264;
    }
    //
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
    //
    q->next1 = q->last1 = 0, q->full1 = 1;
    q->next2 = q->last2 = 0, q->full2 = 0;
    q->run = 1;
    return q;
}

void queue_close642(queue642 **queue) {
    int i;
    pthread_mutex_lock((*queue)->mutex);
    (*queue)->run = 0;
    while((*queue)->full2 || (*queue)->last2 != (*queue)->next2) {
        pthread_cond_wait(&(*queue)->attr, &(*queue)->mutex);
    }
    pthread_mutex_unlock((*queue)->mutex);
    void *ret;
    pthread_join((*queue)->thread, &ret);

    pthread_mutex_destroy(&(*queue)->mutex);
    pthread_cond_destroy(&(*queue)->cond);
    for(i = 0 ; i < (*queue)->nb_yuv ; i++) {
        x264_picture_clean(&(*queue)->yuv[i]);
    }
    free((*queue)->tvs);
    free((*queue)->index1);
    free((*queue)->index2);
//    free((*queue)->yuv);
    free(*queue);
    queue = NULL;
}

int dequ1ue642(queue642 *queue) {
    int i;
    pthread_mutex_lock(&queue->mutex);
    while(!queue->full1 && queue->next1 == queue->last1) {
        pthread_cond_wait(&queue->cond, &queue->mutex);
        if (!queue->run) return -1;
    }
    i = queue->index1[queue->next1];
    queue->next1 = (queue->next1 + 1) % queue->nb_yuv;
    queue->full1 = 0;
    pthread_cond_broadcast(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);

    return i;
}

int dequ2ue642(queue642 *queue, struct timeval *tv) {
    int i;
    pthread_mutex_lock(&queue->mutex);
    while(!queue->full2 && queue->next2 == queue->last2) {
        pthread_cond_wait(&queue->cond, &queue->mutex);
//        if (!queue->run) return -1;
    }
    i = queue->index1[queue->next2];
    memcpy(tv, &queue->tvs[queue->next2], sizeof(struct timeval));
    queue->next2 = (queue->next2 + 1) % queue->nb_yuv;
    queue->full1 = 0;
    pthread_cond_broadcast(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);

    return i;
}

void enqu1ue642(queue642 *queue, int i) {
    pthread_mutex_lock(&queue->mutex);
    queue->index1[queue->last1] = i;
    queue->last1 = (queue->last1 + 1) % queue->nb_yuv;
    if (queue->next1 == queue->last1) {
        queue->full1 = 1;
    }
    pthread_cond_broadcast(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);
}

void enqu2ue642(queue642 *queue, int i, struct timeval tv) {
    pthread_mutex_lock(&queue->mutex);
    queue->index2[queue->last2] = i;
    memcpy(&queue->tvs[queue->next2], &tv, sizeof(struct timeval));
    queue->last2 = (queue->last2 + 1) % queue->nb_yuv;
    if (queue->next2 == queue->last2) {
        queue->full2 = 1;
    }
    pthread_cond_broadcast(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);
}

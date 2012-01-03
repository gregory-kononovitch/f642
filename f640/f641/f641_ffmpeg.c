/*
 * file    : f641_ffmpeg.c
 * project : f640
 *
 * Created on: Dec 20, 2011 (from ../f641_processing)
 * Author and copyright (C) 2011 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "f641.h"


/*************************************
 *      MJPEG Decoding Thread
 *************************************/
static int DEBUG = 0;
struct f641_decode_mjpeg_ressources {
    AVCodec             *codec;
    AVCodecContext      *decoderCtxt;
    AVFrame             *picture;
    AVPacket            pkt;

    int                 get_buffer_index;
    struct f640_queue   *get_buffer_queue;
    struct f640_line    **line0;
};

static int f641_get_mjpeg_buffer(struct AVCodecContext *c, AVFrame *picture, struct f640_line *line) {
    int r;

    picture->opaque = line;

    picture->data[0] = line->yuvp->data[0];
    picture->data[1] = line->yuvp->data[1];
    picture->data[2] = line->yuvp->data[2];
    picture->data[3] = line->yuvp->data[3];

    picture->linesize[0] = line->grid->width + 32;
    picture->linesize[1] = picture->linesize[0] / 2;
    picture->linesize[2] = picture->linesize[0] / 2;
    picture->linesize[3] = 0;

//    picture->linesize[0] = line->yuvp->linesize[0];
//    picture->linesize[1] = line->yuvp->linesize[1];
//    picture->linesize[2] = line->yuvp->linesize[2];
//    picture->linesize[3] = line->yuvp->linesize[3];

    return 0;
}

#define F641_MJPEG_MAX_GET_BUFFER   3
static int f641_func[F641_MJPEG_MAX_GET_BUFFER] = {0,};
static struct f640_queue *f641_get_buffer_queue0 = NULL;
static struct f640_queue *f641_get_buffer_queue1 = NULL;
static struct f640_queue *f641_get_buffer_queue2 = NULL;
static struct f640_line *line0 = NULL;
static struct f640_line *line1 = NULL;
static struct f640_line *line2 = NULL;
static int f641_get_mjpeg_buffer0(struct AVCodecContext *c, AVFrame *picture) {
//    // Dequeue
//    int s = f640_dequeue(f641_get_buffer_queue0, 1, 0);
//    struct f640_stone *st = &f641_get_buffer_queue0->stones[s];
//    struct f640_line *line = (struct f640_line*) st->private;
//
//    gettimeofday(&line->tvd0, NULL);

    return f641_get_mjpeg_buffer(c, picture, line0);
}
static int f641_get_mjpeg_buffer1(struct AVCodecContext *c, AVFrame *picture) {
//    // Dequeue
//    int s = f640_dequeue(f641_get_buffer_queue1, 1, 0);
//    struct f640_stone *st = &f641_get_buffer_queue1->stones[s];
//    struct f640_line *line = (struct f640_line*) st->private;
//
//    gettimeofday(&line->tvd0, NULL);

    return f641_get_mjpeg_buffer(c, picture, line1);
}
static int f641_get_mjpeg_buffer2(struct AVCodecContext *c, AVFrame *picture) {
//    // Dequeue
//    int s = f640_dequeue(f641_get_buffer_queue2, 1, 0);
//    struct f640_stone *st = &f641_get_buffer_queue2->stones[s];
//    struct f640_line *line = (struct f640_line*) st->private;
//
//    gettimeofday(&line->tvd0, NULL);

    return f641_get_mjpeg_buffer(c, picture, line2);
}

static void f641_release_mjpeg_buffer(struct AVCodecContext *c, AVFrame *picture) {
    AVPicture *pict = (AVPicture*)picture->opaque;
}

int f641_init_ffmpeg(void* appli) {
    struct f641_appli *app = appli;
    avcodec_register_all();
    av_register_all();
//    f641_get_buffer_queue0 = f640_make_queue(app->process->stones, app->process->proc_len, NULL, 0, 0, 0);
//    f641_get_buffer_queue1 = f640_make_queue(app->process->stones, app->process->proc_len, NULL, 0, 0, 0);
//    f641_get_buffer_queue2 = f640_make_queue(app->process->stones, app->process->proc_len, NULL, 0, 0, 0);
}

/*
 * thread unsafe cf f641_func
 */
static void* f641_init_decoding_mjpeg(void* appli) {
    int r;
    struct f641_appli *app = appli;
    struct f641_decode_mjpeg_ressources *res;

    for(r = 0 ; r < F641_MJPEG_MAX_GET_BUFFER ; r++)
        if (!f641_func[r]) break;
    if (r == F641_MJPEG_MAX_GET_BUFFER) return NULL;

    res = calloc(1, sizeof(struct f641_decode_mjpeg_ressources));
    if (!res) {
        printf("ENOMEM allocation struct f641_decode_ressources, returning\n");
        return NULL;
    }
    res->get_buffer_index = r;

    res->codec = avcodec_find_decoder(CODEC_ID_MJPEG);
    res->decoderCtxt = avcodec_alloc_context3(res->codec);    // PIX_FMT_YUVJ422P
    if (!res->decoderCtxt) {
        printf("PB  allocating context in struct f641_decode_ressources, returning\n");
        free(res);
        return NULL;
    }

    r = avcodec_open2(res->decoderCtxt, res->codec, NULL);
    if (r < 0) {
        printf("PB opening context in struct f641_decode_ressources, returning\n");
        av_free(res->decoderCtxt);
        free(res);
        return NULL;
    }

    switch(res->get_buffer_index) {
        case 0 :
            res->get_buffer_queue = f641_get_buffer_queue0;
            res->decoderCtxt->get_buffer = f641_get_mjpeg_buffer0;
            res->decoderCtxt->release_buffer = f641_release_mjpeg_buffer;
            res->line0 = &line0;
            break;
        case 1 :
            res->get_buffer_queue = f641_get_buffer_queue1;
            res->decoderCtxt->get_buffer = f641_get_mjpeg_buffer1;
            res->decoderCtxt->release_buffer = f641_release_mjpeg_buffer;
            res->line0 = &line1;
            break;
        case 2 :
            res->get_buffer_queue = f641_get_buffer_queue2;
            res->decoderCtxt->get_buffer = f641_get_mjpeg_buffer2;
            res->decoderCtxt->release_buffer = f641_release_mjpeg_buffer;
            res->line0 = &line2;
            break;
    }
    f641_func[res->get_buffer_index] = 1;

    // Data
    res->picture = avcodec_alloc_frame();
    if (!res->picture) {
        printf("ENOMEM allocation picture in struct f641_decode_ressources, returning\n");
        f641_func[res->get_buffer_index] = 0;
        av_free(res->decoderCtxt);
        free(res);
        return NULL;
    }
    memset(&res->pkt, 0, sizeof(AVPacket));


    return res;
}


static int f641_exec_decoding_mjpeg(void *appli, void *ressources, struct f640_stone *stone) {
    int i, j, k, m;

    int len, got_pict;
    struct f641_appli *app = (struct f641_appli*)appli;
    struct f641_decode_mjpeg_ressources *res = (struct f641_decode_mjpeg_ressources*)ressources;
    struct f640_line *line = (struct f640_line*) stone->private;

    struct timeval tv;
    gettimeofday(&line->tvd0, NULL);

    if (DEBUG) printf("\t\tDECODE a : dequeue %d, frame %lu (size = %ld)\n", stone->key, line->frame, line->buf.bytesused);

    // Processing
    res->pkt.data = line->buffers[line->buf.index].start;
    res->pkt.size = line->buf.bytesused;

//    f640_enqueue(res->get_buffer_queue, 1, stone->key, 0);
    *res->line0 = line;

    if (DEBUG) printf("\t\tDECODE b : dequeue %d, frame %lu, data %p, pkt %p\n", stone->key, line->frame, line->yuvp ? line->yuvp->data[0] : 0, &res->pkt);

    res->picture->opaque = line->yuvp;
    len = avcodec_decode_video2(res->decoderCtxt, res->picture, &got_pict, &res->pkt);
    if (got_pict) {
        if (DEBUG) printf("\t\tDECODE e : dequeue %d, frame %lu, data %p\n", stone->key, line->frame, line->yuvp ? line->yuvp->data[0] : 0);
        if (line->frame == 1) { // @@@
            for(i = 0 ; i < app->process->proc_len ; i++) {
                struct f640_line *lin = &line->lineup[i];
                memcpy(lin->yuvp->linesize, res->picture->linesize, 4 * sizeof(int));
            }
        }
    } else {
        if (DEBUG) printf("\t\tDECODE e : failed, dequeue %d, frame %lu, data %p\n", stone->key, line->frame, line->yuvp ? line->yuvp->data[0] : 0);
    }

    gettimeofday(&line->tvd1, NULL);

//    timersub(&line->tvd1, &line->tvd0, &tv);
//    printf("Decode MJPEG [%u (%u)] = %d (%d) : %.1fms\n", line->buf.index, line->buf.bytesused, len, got_pict, 0.001 * tv.tv_usec);

    return !got_pict;
}

/*
 * thread unsafe cf f641_func
 */
static void f641_free_decoding_mjpeg(void *appli, void* ressources) {
    int r;
    struct f641_decode_mjpeg_ressources *res = (struct f641_decode_mjpeg_ressources*)ressources;
    if (!res) return;

    f641_func[res->get_buffer_index] = 0;
    av_free(res->picture);      // @@@ ?
    av_free(res->decoderCtxt);
    free(res);
}

//
void f641_attrib_decoding_mjpeg(struct f641_thread_operations *ops) {
        ops->init = f641_init_decoding_mjpeg;
        ops->updt = NULL;
        ops->exec = f641_exec_decoding_mjpeg;
        ops->free = f641_free_decoding_mjpeg;
}


/*************************************
 *      Converse to RGB Thread
 *************************************/
struct f641_convert_torgb_ressources {
    struct SwsContext   *swsCtxt;
    AVFrame             *origin;
    AVPicture           scaled;
};

//
static void* f641_init_converting_torgb(void *appli) {
    int r;
    struct f641_appli *app = (struct f641_appli*)appli;
    struct f641_convert_torgb_ressources *res = NULL;

    // Calloc
    res = calloc(1, sizeof(struct f641_convert_torgb_ressources));
    if (!res) {
        printf("ENOMEM allocating struct f641_convert_torgb, returning\n");
        return NULL;
    }

    // SwScaleCtxt
    res->swsCtxt = sws_getCachedContext(res->swsCtxt,
            app->width, app->height, app->process->decoded_format,
            app->process->broadcast_width, app->process->broadcast_height, app->process->broadcast_format,
            SWS_BICUBIC, NULL, NULL, NULL
    );
    if (!res->swsCtxt) {
        printf("PB allocating scale context f641_convert_torgb, returning\n");
        free(res);
        return NULL;
    }

    // Origin Picture
    res->origin = avcodec_alloc_frame();
    if (!res->origin) {
        printf("ENOMEM allocating struct f641_convert_torgb, returning\n");
        av_free(res->swsCtxt);
        free(res);
        return NULL;
    }
    avcodec_get_frame_defaults(res->origin);
    r = avpicture_alloc((AVPicture*)res->origin, app->process->decoded_format, app->width, app->height);
    if (r < 0) {
        printf("PB allocating origin picture in converting, returning\n");
        av_free(res->origin);
        av_free(res->swsCtxt);
        free(res);
        return NULL;
    }
    res->origin->width = app->width;
    res->origin->height = app->height;
    res->origin->format = app->process->decoded_format;

    // Scaled Picture
    r = avpicture_alloc(&res->scaled, app->process->broadcast_format, app->width, app->height);
    if (r < 0) {
        printf("PB allocating origin picture in converting, returning\n");
        avpicture_free((AVPicture*)res->origin);
        av_free(res->origin);
        av_free(res->swsCtxt);
        free(res);
        return NULL;
    }
    avpicture_free(&res->scaled);
    printf("Converse : scaled.linesize = (%d , %d , %d , %d)\n", res->scaled.linesize[0], res->scaled.linesize[1], res->scaled.linesize[2], res->scaled.linesize[3]);

    return res;
}

//
static int f641_exec_converting_torgb(void *appli, void* ressources, struct f640_stone *stone) {
    struct f641_appli *app = (struct f641_appli*)appli;
    struct f641_convert_torgb_ressources *res = (struct f641_convert_torgb_ressources*)ressources;
    struct f640_line *line = (struct f640_line*) stone->private;


    gettimeofday(&line->tv20, NULL);

    if (DEBUG) printf("\t\t\t\tCONVERT : dequeue %d, frame %lu\n", line->index, line->frame);

    res->scaled.data[0] = line->rgb->data;
    //res->origin->data[0] = line->yuv->data;
    //sws_scale(line->swsCtxt, (const uint8_t**)picture->data, picture->linesize, 0, line->grid->height, scaled->data, scaled->linesize);
    sws_scale(res->swsCtxt, (const uint8_t**)line->yuvp->data, line->yuvp->linesize, 0, app->height, res->scaled.data, res->scaled.linesize);


    if (DEBUG) printf("\t\t\t\tCONVERT : enqueue %d, frame %lu\n", line->index, line->frame);
    gettimeofday(&line->tv21, NULL);

    return 0;
}

//
static void f641_free_converting_torgb(void *appli, void* ressources) {
    struct f641_convert_torgb_ressources *res = (struct f641_convert_torgb_ressources*)ressources;
    if (res) {
        avpicture_free((AVPicture*)res->origin);
        av_free(res->origin);
        av_free(res->swsCtxt);
        free(res);
    }
}

//
void f641_attrib_converting_torgb(struct f641_thread_operations *ops) {
        ops->init = f641_init_converting_torgb;
        ops->updt = NULL;
        ops->exec = f641_exec_converting_torgb;
        ops->free = f641_free_converting_torgb;
}


/******************************
 *      GRAY THREAD
 ******************************/
static int f641_exec_graying(void *appli, void *ressources, struct f640_stone *stone) {
    struct f641_appli *app = (struct f641_appli*)appli;
    struct f640_line *line = (struct f640_line*) stone->private;
    int i;
    uint8_t *im, *pix;

    gettimeofday(&line->tve0, NULL);

    im  = line->gry->data;
    pix = line->yuvp->data[0];
    for(i = line->grid->height ; i > 0 ; i--) {
        memcpy(im, pix, line->grid->width);
        im  += line->grid->width;
        pix += line->yuvp->linesize[0];
    }

    gettimeofday(&line->tve1, NULL);
    return 0;
}

/******************************
 *      SKY THREAD
 ******************************/
// @@ deprecated
static int f641_exec_skying_422p(void *appli, void *ressources, struct f640_stone *stone) {
    struct f641_appli *app = (struct f641_appli*)appli;
    struct f640_line *line = (struct f640_line*) stone->private;
    int i, j;
    uint8_t *im, *pix0, *pix1, *pix2;
    long *acc, min = 0xFFFFFFFFFFFFFFL, max = 0, l, lmin = 0xFFFFFFFFFFFFFFL, lmax = 0;
    uint16_t *sky;

    static int rep[256] = {0,};
    static int del = 0, mins, maxs;


    gettimeofday(&line->tve0, NULL);

    if (line->frame > 30) {
        pix0 = line->yuvp->data[0];
        pix1 = line->yuvp->data[0] + line->yuvp->linesize[0];
        pix2 = line->yuvp->data[0] + 2 * line->yuvp->linesize[0];
        acc  = app->process->grid2->acc + app->width + 1;

        for(i = app->height ; i > 2 ; i--) {
            for(j = app->width ; j > 2 ; j--) {
                l  = pix0[0] + pix0[1] + pix0[2];
                l += pix1[0] + pix1[1] + pix1[2];
                l += pix2[0] + pix2[1] + pix2[2];

//                if (l < 255) rep[l]++;
//                else rep[255]++;
                if (l < lmin) lmin = l;
                if (l > lmax) lmax = l;

                *acc += l;
                if (*acc < min) min = *acc;
                if (*acc > max) max = *acc;

                acc++;
                pix0++;
                pix1++;
                pix2++;
            }
            acc += 2;
            pix0 += line->yuvp->linesize[0] - app->width + 2;
            pix1 += line->yuvp->linesize[0] - app->width + 2;
            pix2 += line->yuvp->linesize[0] - app->width + 2;
        }

        if (line->frame % (app->recording_perst * app->frames_pers) == 0) {
            mins = 0xFFFFFFF;
            maxs = 0;
            sky  = app->process->grid2->sky + 4 * (app->width + 1);
            acc  = app->process->grid2->acc + app->width + 1;
//            for(i = app->height ; i > 2 ; i--) {
//                for(j = app->width ; j > 2 ; j--) {
//
//                    sky[0] = *acc;
//                    if (*acc < sky[1]) sky[1] = *acc;
//                    if (*acc > sky[2]) sky[2] = *acc;
//
//                    if (*acc < mins) mins = *acc;
//                    if (*acc > maxs) maxs = *acc;
//                    if (sky[2] - sky[1] > del) del = sky[2] - sky[1];
//
//                    if (sky[1] < 2551) rep[sky[1] / 10]++;
//                    else rep[255]++;
//
//
//                    sky += 4;
//                    acc++;
//                }
//                acc  += 2;
//                sky  += 4 * 2;
//            }

            pix0 = app->process->grid2->skyDif; //line->gry->data + app->width + 1;
            acc  = app->process->grid2->acc + app->width + 1;
            for(i = app->height ; i > 2 ; i--) {
                for(j = app->width ; j > 2 ; j--) {
                    *pix0 = 256 * (*acc - min) / (max - min + 1);
                    if (app->level) {
                        if (*pix0 > app->level) *pix0 = 0xFF;
                        else *pix0 = 0;
                    }
                    acc++;
                    pix0++;
                }
                acc  += 2;
                pix0 += 2;
            }

//            printf("\nMin = %ld , Max = %ld, lmin = %ld , lmax = %ld , mins = %d , maxs = %d , del = %d", min, max, lmin, lmax, mins, maxs, del);
//            for(i = 0 ; i < 32 ; i++) {
//                printf("\n%d :", 8 * i);
//                for(j = 0 ; j < 8 ; j++) {
//                    printf(" %d", rep[8*i + j]);
//                }
//            }

            memset(app->process->grid2->acc, 0, app->size * sizeof(long));
            memset(rep, 0, sizeof(rep));
        }
    }

    gettimeofday(&line->tve1, NULL);
    return 0;
}


static int f641_exec_skying_422(void *appli, void *ressources, struct f640_stone *stone) {
    struct f641_appli *app = (struct f641_appli*)appli;
    struct f640_line *line = (struct f640_line*) stone->private;
    int i;
    uint8_t  *pix;
    uint16_t *acc;
    uint16_t *sky;


    gettimeofday(&line->tve0, NULL);

    if (line->frame > 30) {
        pix = line->buffers[line->buf.index].start;

        acc  = app->process->grid2->acc;
        for(i = app->size ; i > 0 ; i--) {
            *(acc++) += *(pix++);
            *(acc++) += *(pix++);
        }

        if (line->frame % (app->recording_perst * app->frames_pers) == 0) {
            memcpy(app->process->grid2->sky, app->process->grid2->acc, 2 * app->size * sizeof(uint16_t));
            memset(app->process->grid2->acc, 0, 2 * app->size * sizeof(uint16_t));
        }
    }

    gettimeofday(&line->tve1, NULL);
    return 0;
}

//
void f641_attrib_skying_422(struct f641_thread_operations *ops) {
        ops->init = NULL;
        ops->updt = NULL;
        ops->exec = f641_exec_skying_422;
        ops->free = NULL;
}


/******************************
 *      EDGE THREAD
 ******************************/
static int f641_exec_edging(void *appli, void *ressources, struct f640_stone *stone) {
    struct f641_appli *app = (struct f641_appli*)appli;
    struct f640_line *line = (struct f640_line*) stone->private;
    int i, j;
    uint8_t *im, *pix0, *pix1, *pix2, *edge;
    long *acc, min = 0xFFFFFFFFFFFFFFL, max = 0, l, lmin = 0xFFFFFFFFFFFFFFL, lmax = 0;

    gettimeofday(&line->tve0, NULL);

    if (line->frame > 30) {
        pix0 = line->yuvp->data[0];
        pix1 = line->yuvp->data[0] + line->yuvp->linesize[0];
        pix2 = line->yuvp->data[0] + 2 * line->yuvp->linesize[0];
        edge = line->gry->data + 2 * app->width + 2;

        for(i = app->height ; i > 3 ; i--) {
            for(j = app->width ; j > 3 ; j--) {
                if (pix0[0] > pix2[2]) *edge = pix0[0] - pix2[2];
                else *edge = pix2[2] - pix0[0];
                if (pix0[1] > pix2[2]) l = pix0[1] - pix2[2];
                else l = pix2[2] - pix0[1];
                if (*edge < l) *edge = l;
                if (pix0[2] > pix2[2]) l = pix0[2] - pix2[2];
                else l = pix2[2] - pix0[2];
                if (*edge < l) *edge = l;

                if (pix1[0] > pix2[2]) l = pix1[0] - pix2[2];
                else l = pix2[2] - pix1[0];
                if (*edge < l) *edge = l;
                if (pix1[1] > pix2[2]) l = pix1[1] - pix2[2];
                else l = pix2[2] - pix1[1];
                if (*edge < l) *edge = l;
                if (pix1[2] > pix2[2]) l = pix1[2] - pix2[2];
                else l = pix2[2] - pix1[2];
                if (*edge < l) *edge = l;

                if (pix2[0] > pix2[2]) l = pix2[0] - pix2[2];
                else l = pix2[2] - pix2[0];
                if (*edge < l) *edge = l;
                if (pix2[1] > pix2[2]) l = pix2[1] - pix2[2];
                else l = pix2[2] - pix2[1];
                if (*edge < l) *edge = l;

                edge++;
                pix0++;
                pix1++;
                pix2++;
            }
            edge += 3;
            pix0 += line->yuvp->linesize[0] - app->width + 3;
            pix1 += line->yuvp->linesize[0] - app->width + 3;
            pix2 += line->yuvp->linesize[0] - app->width + 3;
        }
    }

    gettimeofday(&line->tve1, NULL);
    return 0;
}

//
void f641_attrib_edging(struct f641_thread_operations *ops) {
        ops->init = NULL;
        ops->updt = NULL;
        ops->exec = f641_exec_skying_422;
        ops->free = NULL;
}

/*************************************
 *      Recording Thread
 *************************************/
struct f641_recording_ressources {
    AVFormatContext *outputFile;
    AVStream *video_stream;
    AVCodec *encoder;
    AVPacket pkt;
};

//
static void* f641_init_recording(void *appli) {
    int r;
    struct f641_appli *app = (struct f641_appli*)appli;
    struct f641_recording_ressources *res = NULL;

    // Calloc
    res = calloc(1, sizeof(struct f641_recording_ressources));
    if (!res) {
        printf("ENOMEM allocating struct f641_record_ressources, returning\n");
        return NULL;
    }

    // AVFormatCtxt
    r = avformat_alloc_output_context2(&res->outputFile, NULL, NULL, app->record_path);
    if (r < 0) {
        printf("PB allocating output ctxt in recording, returning\n");
        free(res);
        return NULL;
    }
    res->outputFile->start_time_realtime = 1500000000;

    // AVIoCtxt
    r = avio_open(&res->outputFile->pb, app->record_path, AVIO_FLAG_WRITE);
    if (r < 0) {
        printf("PB opening output ctxt in recording, returning\n");
        avformat_free_context(res->outputFile);
        free(res);
        return NULL;
    }

    // Video stream
    res->video_stream = av_new_stream(res->outputFile, 0);
    if (!res->video_stream) {
        printf("PB getting new stream in recording, returning\n");
        avio_close(res->outputFile->pb);
        avformat_free_context(res->outputFile);
        free(res);
        return NULL;
    }
    res->video_stream->codec->pix_fmt       = app->process->record_format;
    res->video_stream->codec->coded_width   = app->width;
    res->video_stream->codec->coded_height  = app->height;
    res->video_stream->codec->time_base.num = 1;
    res->video_stream->codec->time_base.den = app->recording_rate;
    res->video_stream->codec->sample_fmt    = SAMPLE_FMT_S16;
    res->video_stream->codec->sample_rate   = 44100;

    // Codec / CodecCtxt
    res->encoder = avcodec_find_encoder(app->process->recording_codec);
    if (!res->encoder) {
        printf("PB finding codec in recording, returning\n");
        avio_close(res->outputFile->pb);
        avformat_free_context(res->outputFile);
        free(res);
        return NULL;
    }
    r = avcodec_open2(res->video_stream->codec, res->encoder, NULL);
    if (r < 0) {
        printf("PB opening codecCtxt in recording, returning\n");
        avio_close(res->outputFile->pb);
        avformat_free_context(res->outputFile);
        free(res);
        return NULL;
    }
    res->video_stream->time_base.num = 1;
    res->video_stream->time_base.den = app->recording_rate;
    res->video_stream->r_frame_rate.num = app->recording_rate;
    res->video_stream->r_frame_rate.den = 1;
    res->video_stream->avg_frame_rate.num = app->recording_rate;
    res->video_stream->avg_frame_rate.den = 1;
    res->video_stream->pts.num = 1;
    res->video_stream->pts.den = 1;
    res->video_stream->pts.val = 1;

    //
    av_init_packet(&res->pkt);
    r = av_new_packet(&res->pkt, 2 * app->width * app->height);
    if (r < 0) {
        printf("PB getting new packet in recording, returning\n");
        avcodec_close(res->video_stream->codec);    // @@@ ?
        avio_close(res->outputFile->pb);
        avformat_free_context(res->outputFile);
        free(res);
        return NULL;
    }
    av_free(res->pkt.data);

    // Header
    r = avformat_write_header(res->outputFile, NULL);
    if (r < 0) {
        printf("PB getting writing header in recording, returning\n");
//        av_free_packet(&res->pkt);
        avcodec_close(res->video_stream->codec);    // @@@ ?
        avio_close(res->outputFile->pb);
        avformat_free_context(res->outputFile);
        free(res);
        return NULL;
    }

    return res;
}

//
static int f641_exec_recording(void *appli, void* ressources, struct f640_stone *stone) {
    struct f641_appli *app = (struct f641_appli*)appli;
    struct f641_recording_ressources *res = (struct f641_recording_ressources*)ressources;
    struct f640_line *line = (struct f640_line*) stone->private;
    int r;

    gettimeofday(&line->tv30, NULL);

    if ( app->recording && line->flaged ) {
        if (app->functions == 2) {
            res->pkt.data = app->process->grid2->skyDif;    //line->gry->data; //line->buffers[line->actual].start;
            res->pkt.size = app->size; //line->buf.bytesused;
        } else {
            res->pkt.data = line->buffers[line->actual].start;
            res->pkt.size = line->buf.bytesused;
        }
        res->pkt.pts  = app->process->recorded_frames;
        res->pkt.dts  = res->pkt.pts;
        res->pkt.duration = 1;
        res->pkt.pos  = -1;
        res->pkt.stream_index = 0;
        r = av_write_frame(res->outputFile, &res->pkt);
        avio_flush(res->outputFile->pb);

        gettimeofday(&app->process->tvr, NULL);
        app->process->recorded_frames++;
    }

    //
    if (DEBUG) printf("\t\t\t\t\t\tRECORD  : enqueue %d, frame %lu\n", line->index, line->frame);
    gettimeofday(&line->tv31, NULL);


    return 0;
}

//
static void f641_free_recording(void *appli, void* ressources) {
    struct f641_recording_ressources *res = (struct f641_recording_ressources*)ressources;
    if (res) {
        //av_free_packet(&res->pkt);
        //av_free(res->pkt.data);
        avcodec_close(res->video_stream->codec);    // @@@ ?
        avio_close(res->outputFile->pb);
        avformat_free_context(res->outputFile);
        free(res);
    }
}

//
void f641_attrib_recording(struct f641_thread_operations *ops) {
        ops->init = f641_init_recording;
        ops->updt = NULL;
        ops->exec = f641_exec_recording;
        ops->free = f641_free_recording;
}


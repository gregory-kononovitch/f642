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


/*
 *      MJPEG Decoding Thread
 */
static int DEBUG = 0;
struct f641_decode_mjpeg_ressources {
    AVCodec             *codec;
    AVCodecContext      *decoderCtxt;
    AVFrame             *picture;
    AVPacket            pkt;

    int                 get_buffer_index;
    struct f640_queue   *get_buffer_queue;
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
static int f641_get_mjpeg_buffer0(struct AVCodecContext *c, AVFrame *picture) {
    // Dequeue
    int s = f640_dequeue(f641_get_buffer_queue0, 1, 0);
    struct f640_stone *st = &f641_get_buffer_queue0->stones[s];
    struct f640_line *line = (struct f640_line*) st->private;

    gettimeofday(&line->tvd0, NULL);

    return f641_get_mjpeg_buffer(c, picture, line);
}
static int f641_get_mjpeg_buffer1(struct AVCodecContext *c, AVFrame *picture) {
    // Dequeue
    int s = f640_dequeue(f641_get_buffer_queue1, 1, 0);
    struct f640_stone *st = &f641_get_buffer_queue1->stones[s];
    struct f640_line *line = (struct f640_line*) st->private;

    gettimeofday(&line->tvd0, NULL);

    return f641_get_mjpeg_buffer(c, picture, line);
}
static int f641_get_mjpeg_buffer2(struct AVCodecContext *c, AVFrame *picture) {
    // Dequeue
    int s = f640_dequeue(f641_get_buffer_queue2, 1, 0);
    struct f640_stone *st = &f641_get_buffer_queue2->stones[s];
    struct f640_line *line = (struct f640_line*) st->private;

    gettimeofday(&line->tvd0, NULL);

    return f641_get_mjpeg_buffer(c, picture, line);
}

static void f641_release_mjpeg_buffer(struct AVCodecContext *c, AVFrame *picture) {
    AVPicture *pict = (AVPicture*)picture->opaque;
}

/*
 * thread unsafe cf f641_func
 */
void* f641_init_decode_mjpeg(void* appli) {
    int r;
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
            break;
        case 1 :
            res->get_buffer_queue = f641_get_buffer_queue1;
            res->decoderCtxt->get_buffer = f641_get_mjpeg_buffer1;
            res->decoderCtxt->release_buffer = f641_release_mjpeg_buffer;
            break;
        case 2 :
            res->get_buffer_queue = f641_get_buffer_queue2;
            res->decoderCtxt->get_buffer = f641_get_mjpeg_buffer2;
            res->decoderCtxt->release_buffer = f641_release_mjpeg_buffer;
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


int f641_exec_decode_mjpeg(void *ressources, struct f640_stone *stone) {
    int i, j, k, m;

    int len, got_pict;

    struct f641_decode_mjpeg_ressources *res = (struct f641_decode_mjpeg_ressources*)ressources;
    struct f640_line *line = (struct f640_line*) stone->private;
    gettimeofday(&line->tvd0, NULL);

    if (DEBUG) printf("\t\tDECODE   : dequeue %d, frame %lu (size = %ld)\n", stone->key, line->frame, line->buffers[line->actual].length);

    // Processing
    res->pkt.data = line->buffers[line->actual].start;
    res->pkt.size = line->buffers[line->actual].length;

    f640_enqueue(res->get_buffer_queue, 1, stone->key, 0);

    if (DEBUG) printf("\t\tDECODE   : dequeue %d, frame %lu, data %p, pkt %p\n", stone->key, line->frame, line->yuvp ? line->yuvp->data[0] : 0, &res->pkt);

    res->picture->opaque = line->yuvp;
    len = avcodec_decode_video2(res->decoderCtxt, res->picture, &got_pict, &res->pkt);
    if (got_pict) {
        if (DEBUG) printf("\t\tDECODE   : dequeue %d, frame %lu, data %p\n", stone->key, line->frame, line->yuvp ? line->yuvp->data[0] : 0);
    } else {
        if (DEBUG) printf("\t\tDECODE   : failed, dequeue %d, frame %lu, data %p\n", stone->key, line->frame, line->yuvp ? line->yuvp->data[0] : 0);
    }

    gettimeofday(&line->tvd1, NULL);

    return !got_pict;
}


/*
 * thread unsafe cf f641_func
 */
void f641_free_decode_mjpeg(void *appli, void* ressources) {
    int r;
    struct f641_decode_mjpeg_ressources *res = (struct f641_decode_mjpeg_ressources*)ressources;
    if (!res) return;

    f641_func[res->get_buffer_index] = 0;
    av_free(res->picture);      // @@@ ?
    av_free(res->decoderCtxt);
    free(res);
}

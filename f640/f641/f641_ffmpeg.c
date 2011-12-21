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
struct f641_decode_mjpeg_ressources {
    AVCodec             *codec;
    AVCodecContext      *decoderCtxt;
    AVFrame             *picture;
    AVPacket            pkt;
};

static int f641_get_mjpeg_buffer(struct AVCodecContext *c, AVFrame *picture) {
    int r;

    // Dequeue
    int l = f640_dequeue_line(f640_snaped);
    struct f640_line *line = &f640_snaped->lineup[l];
    gettimeofday(&line->tvd0, NULL);


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

static int f641_get_mjpeg_buffer1(struct AVCodecContext *c, AVFrame *picture) {
    int r;

    // Dequeue
    int l = f640_dequeue_line(f640_snaped);
    struct f640_line *line = &f640_snaped->lineup[l];
    gettimeofday(&line->tvd0, NULL);

    return f641_get_mjpeg_buffer(c, picture);
}

static void f641_release_mjpeg_buffer(struct AVCodecContext *c, AVFrame *picture) {
    AVPicture *pict = (AVPicture*)picture->opaque;
}


void* f641_init_decode_mjpeg(void* appli) {
    static int func = 0;
    int r;
    struct f641_decode_mjpeg_ressources *res;

    res = calloc(1, sizeof(struct f641_decode_ressources));
    if (!res) {
        printf("ENOMEM allocation struct f641_decode_ressources, returning\n");
        return NULL;
    }

    res->codec = avcodec_find_decoder(CODEC_ID_MJPEG);
    f611_dump_codec(res->codec);

    res->decoderCtxt = avcodec_alloc_context3(res->codec);    // PIX_FMT_YUVJ422P
    avcodec_open2(res->decoderCtxt, res->codec, NULL);
    switch(func) {
        case 0 :
            res->decoderCtxt->get_buffer = f641_get_mjpeg_buffer1;
            res->decoderCtxt->release_buffer = f641_release_mjpeg_buffer;
            break;
        case 1 :
            res->decoderCtxt->get_buffer = f641_get_mjpeg_buffer;
            res->decoderCtxt->release_buffer = f641_release_mjpeg_buffer;
            break;
        default :
    }

    // Data
    res->picture = avcodec_alloc_frame();
    memset(&res->pkt, 0, sizeof(AVPacket));


    func++;
    return 0;
}


int f641_exec_decode_mjpeg(void *ressources, struct f640_stone *stone) {
    int i, j, k, m;

    int                 len, got_pict;


    while(1) {
        // Dequeue
        int l = f640_dequeue_sync(&lines->snaped);
        struct f640_line *line = &lines->snaped.lineup[l];
        gettimeofday(&line->tvd0, NULL);

        if (DEBUG) printf("\t\tDECODE   : dequeue %d, frame %lu (size = %ld)\n", l, line->frame, line->buffers[line->actual].length);

        // Processing
        pkt.data = line->buffers[line->actual].start;
        pkt.size = line->buffers[line->actual].length;
        if ( line->frame == 0 ) {
            printf("SAVE %p, %lu\n", line->buffers[line->actual].start, line->buffers[line->actual].length);
            FILE *fj = fopen("tst.mjpeg", "wb");
            fwrite(line->buffers[line->actual].start, 1, line->buffers[line->actual].length, fj);
            fclose(fj);
        }

        f640_enqueue_line(&lines->buffered, l);

        if (DEBUG) printf("\t\tDECODE   : dequeue %d, frame %lu, data %p, pkt %p\n", l, line->frame, line->yuvp ? line->yuvp->data[0] : 0, &pkt);

        picture->opaque = line->yuvp;
        len = avcodec_decode_video2(decoderCtxt, picture, &got_pict, &pkt);
        if (got_pict) {
            if (DEBUG) printf("\t\tDECODE   : dequeue %d, frame %lu, data %p\n", l, line->frame, line->yuvp ? line->yuvp->data[0] : 0);

            if (DEBUG) printf("pict : linesize = %d,%d,%d,%d ; error %lu,%lu,%lu,%lu ; size = (%dx%d) ; format = %d (%u-%u-%u)\n"
                    , picture->linesize[0], picture->linesize[1], picture->linesize[2], picture->linesize[3]
                    , picture->error[0], picture->error[1], picture->error[2], picture->error[3]
                    , picture->width, picture->height
                    , picture->format
                    , picture->data[0][90], picture->data[0][91], picture->data[0][92]
            );

            if (line->frame == 1) {
                for(i = 0 ; i < lines->snaped.size ; i++) {
                    struct f640_line *lin = &lines->snaped.lineup[i];
                    memcpy(lin->yuvp->linesize, picture->linesize, 4 * sizeof(int));
                }
            }

//            av_picture_copy(line->yuvp, (AVPicture*)picture, PIX_FMT_YUVJ422P, line->grid->width, line->grid->height);
        }

        if (DEBUG) printf("\t\tDECODE   : image copied\n");


        // Enqueue
        if (DEBUG) printf("\t\tDECODE   : enqueue %d, frame %lu\n", l, line->frame);
        gettimeofday(&line->tvd1, NULL);
        f640_sync(&lines->snaped, line->frame);
        f640_enqueue_line(&lines->decoded, l);
    }
    pthread_exit(NULL);
}

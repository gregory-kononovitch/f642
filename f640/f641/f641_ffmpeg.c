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



/*************************************
 *      Converse to RGB Thread
 *************************************/
struct f641_convert_torgb_ressources {
    struct SwsContext   *swsCtxt;
    AVFrame             *origin;
    AVFrame             *scaled;
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
            app->process->grid->width, app->process->grid->height, app->process->decoded_format,
            app->process->grid->width, app->process->grid->height, app->process->broadcast_format,
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
    r = avpicture_alloc((AVPicture*)res->origin, app->process->decoded_format, app->process->grid->width, app->process->grid->height);
    if (r < 0) {
        printf("PB allocating origin picture in converting, returning\n");
        av_free(res->origin);
        av_free(res->swsCtxt);
        free(res);
        return NULL;
    }
    res->origin->width = app->process->grid->width;
    res->origin->height = app->process->grid->height;
    res->origin->format = app->process->decoded_format;

    // Scaled Picture
    res->scaled = avcodec_alloc_frame();
    if (!res->scaled) {
        printf("ENOMEM allocating struct f641_convert_torgb, returning\n");
        avpicture_free((AVPicture*)res->origin);
        av_free(res->origin);
        av_free(res->swsCtxt);
        free(res);
        return NULL;
    }
    avcodec_get_frame_defaults(res->scaled);
    r = avpicture_alloc((AVPicture*)res->scaled, app->process->broadcast_format, app->process->grid->width, app->process->grid->height);
    if (r < 0) {
        printf("PB allocating origin picture in converting, returning\n");
        av_free(res->scaled);
        avpicture_free((AVPicture*)res->origin);
        av_free(res->origin);
        av_free(res->swsCtxt);
        free(res);
        return NULL;
    }
    res->scaled->width = app->process->grid->width;
    res->scaled->height = app->process->grid->height;
    res->scaled->format = app->process->broadcast_format;

    return res;
}

//
static int f641_exec_converting_torgb(void *appli, void* ressources, struct f640_stone *stone) {
    struct f641_appli *app = (struct f641_appli*)appli;
    struct f641_convert_torgb_ressources *res = (struct f641_convert_torgb_ressources*)ressources;
    struct f640_line *line = (struct f640_line*) stone->private;


    gettimeofday(&line->tv20, NULL);

    if (DEBUG) printf("\t\t\t\tCONVERT : dequeue %d, frame %lu\n", line->index, line->frame);

    res->scaled->data[0] = line->rgb->data;
    //res->origin->data[0] = line->yuv->data;
    //sws_scale(line->swsCtxt, (const uint8_t**)picture->data, picture->linesize, 0, line->grid->height, scaled->data, scaled->linesize);
    sws_scale(res->swsCtxt, (const uint8_t**)line->yuvp->data, line->yuvp->linesize, 0, line->grid->height, res->scaled->data, res->scaled->linesize);


    if (DEBUG) printf("\t\t\t\tCONVERT : enqueue %d, frame %lu\n", line->index, line->frame);
    gettimeofday(&line->tv21, NULL);

    return 0;
}

//
static void f641_free_converting_torgb(void *appli, void* ressources) {
    struct f641_convert_torgb_ressources *res = (struct f641_convert_torgb_ressources*)ressources;
    if (res) {
        avpicture_free((AVPicture*)res->scaled);
        av_free(res->scaled);
        avpicture_free((AVPicture*)res->origin);
        av_free(res->origin);
        av_free(res->swsCtxt);
        free(res);
    }
}


/******************************
 *      GRAY THREAD
 ******************************/
static int f641_exec_edge(void *appli, void *ressources, struct f640_stone *stone) {
    struct f641_appli *app = (struct f641_appli*)appli;
    struct f640_line *line = (struct f640_line*) stone->private;
    int i;
    uint8_t *im, *pix;

    im  = line->gry->data;
    pix = line->yuvp->data[0];
    for(i = line->grid->height ; i > 0 ; i--) {
        memcpy(im, pix, line->grid->width);
        im  += line->grid->width;
        pix += line->yuvp->linesize[0];
    }

    return 0;
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
    res->video_stream->codec->time_base.den = app->frames_pers;
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
    res->video_stream->time_base.den = app->frames_pers;
    res->video_stream->r_frame_rate.num = app->frames_pers;
    res->video_stream->r_frame_rate.den = 1;
    res->video_stream->avg_frame_rate.num = app->frames_pers;
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

    // Header
    r = avformat_write_header(res->outputFile, NULL);
    if (r < 0) {
        printf("PB getting writing header in recording, returning\n");
        av_free_packet(&res->pkt);
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
        res->pkt.data = line->buffers[line->actual].start;
        res->pkt.size = line->buf.bytesused;
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
        av_free_packet(&res->pkt);
        avcodec_close(res->video_stream->codec);    // @@@ ?
        avio_close(res->outputFile->pb);
        avformat_free_context(res->outputFile);
        free(res);
    }
}

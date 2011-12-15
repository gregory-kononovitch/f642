/*
 * file    : f641_processing.c
 * project : f640
 *
 * Created on: Dec 4, 2011
 * Author and copyright (C) 2011 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */



#include "f640.h"

extern int DEBUG;

/*
 *
 */
double inline f640_duration(struct timeval tv2, struct timeval tv1) {
    return (tv2.tv_sec + tv2.tv_usec / 1000000.0) - (tv1.tv_sec + tv1.tv_usec / 1000000.0);
}

double inline  f640_frequency(struct timeval tv1, struct timeval tv0) {
    double d = f640_duration(tv1, tv0);
    if (d != 0) return 1 / d;
    return -1;
}


/*
 *
 */
struct f640_grid *f640_make_grid(int width, int height, int factor) {
    int i;
    struct f640_grid *grid = calloc(1, sizeof(struct f640_grid));
    if (!grid) {
        printf("ENOMEM in allocating grid, returning\n");
        return NULL;
    }
    //
    grid->width     = width;
    grid->height    = height;
    grid->size      = width * height;
    //
    for(grid->wlen = grid->width  / factor ; grid->wlen > 8 ; grid->wlen--) if (grid->width  % grid->wlen == 0) break;
    for(grid->hlen = grid->height / factor ; grid->hlen > 8 ; grid->hlen--) if (grid->height % grid->hlen == 0) break;
    grid->gsize    = grid->wlen * grid->hlen;
    //
    grid->cols = grid->width  / grid->wlen + ((grid->width  % grid->wlen) ? 1 : 0);
    grid->rows = grid->height / grid->hlen + ((grid->height % grid->hlen) ? 1 : 0);
    grid->num  = grid->rows * grid->cols;
    //
    grid->index_grid = calloc(grid->size, sizeof(uint16_t));
    if (!grid->index_grid) {
        printf("ENOMEM in allocating index grid, returning\n");
        free(grid);
        return NULL;
    }
    for(i = 0 ; i < grid->size ; i++) grid->index_grid[i] = ( i % grid->width ) / grid->wlen + ( ( i / grid->width ) / grid->hlen ) * grid->cols; // @@@ buggy
    //
    grid->grid_index = calloc(grid->num, sizeof(uint32_t));
    if (!grid->grid_index) {
        printf("ENOMEM in allocating grid index, returning\n");
        free(grid->index_grid);
        free(grid);
        return NULL;
    }
    for(i = 0 ; i < grid->num ; i++) grid->grid_index[i] = (i % grid->cols) * grid->wlen + (i / grid->cols) * grid->hlen * grid->width;
    //
    grid->grid_coefs = calloc(grid->num, sizeof(int64_t));
    if (!grid->grid_coefs) {
        printf("ENOMEM in allocating grid coefs, returning\n");
        free(grid->grid_index);
        free(grid->index_grid);
        free(grid);
        return NULL;
    }
    grid->grid_ratio = (int32_t*) grid->grid_coefs;
    for(i = 0 ; i < 2 * grid->num ; i++) grid->grid_ratio[i] = 1;
    pthread_mutex_init(&grid->mutex_coefs, NULL);

    return grid;
}


/*
 *
 */
struct f640_line* f640_make_lineup(v4l2_buffer_t *buffers, int nbuffers, struct f640_grid *grid, enum PixelFormat dstFormat, struct output_stream *stream, struct f051_log_env *log_env, long threshold) {
    int i;
    struct f640_line *f640_lineup = calloc(nbuffers, sizeof(struct f640_line));
    if (!f640_lineup) {
        printf("ENOMEM allocating lineup, returning\n");
        return NULL;
    }

    for(i = 0 ; i < nbuffers ; i++) {
        f640_lineup[i].index        = i;
        f640_lineup[i].buffers      = buffers;
        f640_lineup[i].grid         = grid;

        // Data Ptrs
        f640_lineup[i].width  = calloc(6 + grid->rows * grid->cols, sizeof(long));
        if (!f640_lineup[i].width) {
            printf("ENOMEM allocating grid_values, returning\n");
            for(--i ; i > -1 ; i--) {
                free(f640_lineup[i].rgb->data);
                free(f640_lineup[i].grid_values);
            }
            free(f640_lineup);
            f640_lineup = NULL;
            return NULL;
        }
        f640_lineup[i].height      = f640_lineup[i].width + 1;
        f640_lineup[i].cell_width  = f640_lineup[i].width + 2;
        f640_lineup[i].cell_height = f640_lineup[i].width + 3;
        f640_lineup[i].rows        = f640_lineup[i].width + 4;
        f640_lineup[i].cols        = f640_lineup[i].width + 5;
        f640_lineup[i].rms         = (long*) (f640_lineup[i].width + 6);
        f640_lineup[i].grid_min    = (long*) (f640_lineup[i].width + 8);       // out
        f640_lineup[i].grid_max    = (long*) (f640_lineup[i].width + 10);       // out
        f640_lineup[i].grid_values = (long*) (f640_lineup[i].width + 12);
        // Data
        *(f640_lineup[i].width       ) = grid->width;
        *(f640_lineup[i].height      ) = grid->height;
        *(f640_lineup[i].cell_width  ) = grid->wlen;
        *(f640_lineup[i].cell_height ) = grid->hlen;
        *(f640_lineup[i].rows        ) = grid->rows;
        *(f640_lineup[i].cols        ) = grid->cols;



        f640_lineup[i].grid_th = threshold;

        //
        f640_lineup[i].srcFormat    = PIX_FMT_YUYV422;
        f640_lineup[i].dstFormat    = dstFormat;
        f640_lineup[i].yuv          = f640_create_yuv_image(grid->width, grid->height);
        f640_lineup[i].gry          = f640_create_gry_image(grid->width, grid->height);

        switch(dstFormat) {
            case PIX_FMT_GRAY8 :
                f640_lineup[i].rgb  = f640_create_gry_image(grid->width, grid->height);
                break;
            case PIX_FMT_YUYV422 :
                f640_lineup[i].rgb  = f640_create_yuv_image(grid->width, grid->height);
                break;
            case PIX_FMT_BGR24 :
            case PIX_FMT_RGB24 :
                f640_lineup[i].rgb  = f640_create_rgb_image(grid->width, grid->height);
                break;
            case PIX_FMT_ABGR :
            case PIX_FMT_ARGB :
            case PIX_FMT_BGRA :
            case PIX_FMT_RGBA :
                f640_lineup[i].rgb  = f640_create_rgba_image(grid->width, grid->height);
                break;
        }

        // YUVJP
        f640_lineup[i].yuvp = calloc(1, sizeof(AVPicture));
        avpicture_alloc(f640_lineup[i].yuvp, PIX_FMT_YUVJ422P, 12 * grid->width / 10, 12 * grid->height / 10);
//        f640_lineup[i].yuvp->data[0] = malloc(grid->width + 32);
//        f640_lineup[i].yuvp->data[1] = malloc((grid->width + 32)/2);
//        f640_lineup[i].yuvp->data[2] = malloc((grid->width + 32)/2);
//        f640_lineup[i].yuvp->data[3] = NULL;
//
//        f640_lineup[i].yuvp->linesize[0] = grid->width + 32;
//        f640_lineup[i].yuvp->linesize[1] = (grid->width + 32)/2;
//        f640_lineup[i].yuvp->linesize[2] = (grid->width + 32)/2;
//        f640_lineup[i].yuvp->linesize[3] = 0;



        if (!f640_lineup[i].yuv || !f640_lineup[i].rgb) {
            printf("ENOMEM allocating grid_values, returning\n");
            if (f640_lineup[i].rgb->data) free(f640_lineup[i].rgb->data);
            if (f640_lineup[i].yuv->data) free(f640_lineup[i].yuv->data);
            free(f640_lineup[i].grid_values);
            for(--i ; i > -1 ; i--) {
                free(f640_lineup[i].rgb->data);
                free(f640_lineup[i].yuv->data);
                free(f640_lineup[i].width);
            }
            free(f640_lineup);
            f640_lineup = NULL;
            return NULL;
        }

        f640_lineup[i].log_env        = log_env;
        f640_lineup[i].stream         = stream;

        //
        f640_lineup[i].swsCtxt = sws_getCachedContext(f640_lineup[i].swsCtxt,
                grid->width, grid->height, PIX_FMT_YUVJ422P, //f640_lineup[i].srcFormat,
                grid->width, grid->height, f640_lineup[i].dstFormat,
                SWS_BICUBIC, NULL, NULL, NULL
        );

    }
    return f640_lineup;
}


/*
 *
 */
int f640_init_queue(struct f640_video_queue *queue, struct f640_line *lineup, int count) {
    memset(queue, 0, sizeof(struct f640_video_queue));
    queue->lineup   = lineup;
    queue->size     = count;
    queue->lines     = calloc(queue->size, sizeof(int));
    queue->last     = 0;
    queue->next     = 0;
    queue->real     = 0;
    queue->nb_in      = 0;
    queue->oldest   = 0;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init (&queue->cond , NULL);
    return 0;
}

int f640_enqueue_buffer(struct f640_video_queue *queue, struct v4l2_buffer *v4l2_buffer) {
    pthread_mutex_lock(&queue->mutex);
    queue->nb_in++;
    struct f640_line *line = &queue->lineup[queue->lines[queue->last]];
    gettimeofday(&queue->lineup[queue->lines[queue->last]].tv00, NULL);
    memcpy(&line->buf, v4l2_buffer, sizeof(struct v4l2_buffer));
    line->previous_line = queue->lines[queue->last ? queue->last - 1 : queue->size - 1];
    line->actual = line->buf.index;
    line->last   = queue->lineup[line->previous_line].actual;
    //line->last   = queue->lineup[queue->lines[(queue->last-1) % queue->size]].actual;
    line->frame  = queue->nb_in;
    //line->yuv->data = line->buffers[line->index].start;

    queue->last = (++queue->last) % queue->size;
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);


    return 0;
}

int f640_sync(struct f640_video_queue *queue, uint64_t f) {
    int i;
    pthread_mutex_lock(&queue->mutex);
    if (f == queue->oldest + 1) {
        queue->oldest++;
        while( f ) {
            f = 0;
            for(i = 0 ; i < F640_MULTI_MAX ; i++) {
                if (queue->done[i] == queue->oldest + 1) {
                    queue->done[i] = 0;
                    queue->oldest++;
                    f = 1;
                    break;
                }
            }
        }
    } else {
        for(i = 0 ; i < F640_MULTI_MAX ; i++) {
            if (queue->done[i] == 0) {
                queue->done[i] = f;
                break;
            }
        }
    }
    //printf("SYNC (3): oldest  %lu ; frame %lu\n", queue->oldest, f);
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);
    return 0;
}

int f640_enqueue_line(struct f640_video_queue *queue, int line) {
    pthread_mutex_lock(&queue->mutex);
    queue->lines[queue->last] = line;
    queue->last = (++queue->last) % queue->size;
    queue->nb_in++;
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);
    return 0;
}

int f640_dequeue_sync(struct f640_video_queue *queue) {
    int i = 0;
    pthread_mutex_lock(&queue->mutex);
    loop:
    i = queue->lines[queue->next];
    //printf("SYNC (1): old = %lu ; frame %lu\n", queue->oldest, queue->lineup[i].frame);
    if (queue->next == queue->last || queue->lineup[i].frame - queue->oldest > F640_MULTI_MAX) {
        pthread_cond_wait(&queue->cond, &queue->mutex);
        goto loop;
    }
    //printf("SYNC (2): old = %lu ; frame %lu\n", queue->oldest, queue->lineup[i].frame);
    queue->next = (++queue->next) % queue->size;
    pthread_mutex_unlock(&queue->mutex);
    return i;
}

int f640_dequeue_line(struct f640_video_queue *queue) {
    pthread_mutex_lock(&queue->mutex);
    while (queue->next == queue->last) {
        pthread_cond_wait(&queue->cond, &queue->mutex);
    }
    int i = queue->lines[queue->next];
    queue->next = (++queue->next) % queue->size;
    pthread_mutex_unlock(&queue->mutex);
    return i;
}

int f640_dequeue_line_number(struct f640_video_queue *queue, uint64_t frame) {
    int i;
    pthread_mutex_lock(&queue->mutex);
    loop:
    for(i = queue->next ; i != queue->last ; (++i) % queue->size)
        if (queue->lineup[queue->lines[i]].frame == frame) break;
    if ( i != queue->last ) {
        int j = queue->lines[i];
        for( ; i != queue->next ; (--i) % queue->size) {
          queue->lines[i] = queue->lines[(i-1) % queue->size];
        }
        queue->next = (++queue->next) % queue->size;
        //
        pthread_mutex_unlock(&queue->mutex);
        return j;
    }
    pthread_cond_wait(&queue->cond, &queue->mutex);
    goto loop;
}

int f640_free_line(struct f640_video_queue *queue, int line) {
    pthread_mutex_lock(&queue->mutex);
    while (queue->real == queue->next) {
        pthread_cond_wait(&queue->cond, &queue->mutex);
    }
    queue->lines[queue->real] = line;
    queue->real = (++queue->real) % queue->size;
    pthread_mutex_unlock(&queue->mutex);
    return 0;
}

int f640_dequeue_unblock(struct f640_video_queue *queue) {
    pthread_mutex_lock(&queue->mutex);
    if (DEBUG) printf("DEQ UNBL: next = %d ; last = %d\n", queue->next, queue->last);
    if (queue->next == queue->last) {
        pthread_mutex_unlock(&queue->mutex);
        return -1;
    }
    int i = queue->lines[queue->next];
    if (DEBUG) printf("DEQ UNBL: return = %d\n", i);
    queue->next = (++queue->next) % queue->size;
    pthread_mutex_unlock(&queue->mutex);
    return i;
}

int f640_uns_queue_size(struct f640_video_queue *queue) {
    if (queue->last < queue->next) return queue->size - (queue->next - queue->last);
    return queue->last - queue->next;
}

static struct f640_video_queue *f640_snaped;
static int f641_get_buffer(struct AVCodecContext *c, AVFrame *picture) {
    int r;
    AVPicture *pict = (AVPicture*)picture->opaque;
    if (DEBUG) printf("f641_get_buffer %p (%p) + %p (%p) ; decoder : %p\n", picture, picture->data[0], pict, picture->data[3]);
    if (DEBUG) printf("pict : linesize = %d,%d,%d,%d ; error %lu,%lu,%lu,%lu ; size = (%dx%d) ; format = %d\n" // (%u-%u-%u)\n"
            , picture->linesize[0], picture->linesize[1], picture->linesize[2], picture->linesize[3]
            , picture->error[0], picture->error[1], picture->error[2], picture->error[3]
            , picture->width, picture->height
            , picture->format
//            , picture->data[0][90], picture->data[0][91], picture->data[0][92]
    );

//    r = avcodec_default_get_buffer(c, picture);
//
//    if (DEBUG) printf("f641_get_default_buffer = %d : %p (%p) + %p (%p) ; decoder : %p\n", r, picture, picture->data[0], pict, picture->data[3], c->priv_data);
//    if (DEBUG) printf("pict : linesize = %d,%d,%d,%d ; error %lu,%lu,%lu,%lu ; size = (%dx%d) ; format = %d (%u-%u-%u)\n"
//            , picture->linesize[0], picture->linesize[1], picture->linesize[2], picture->linesize[3]
//            , picture->error[0], picture->error[1], picture->error[2], picture->error[3]
//            , picture->width, picture->height
//            , picture->format
//            , picture->data[0][90], picture->data[0][91], picture->data[0][92]
//    );

    // Dequeue
    int l = f640_dequeue_line(f640_snaped);
    struct f640_line *line = &f640_snaped->lineup[l];
    gettimeofday(&line->tvd0, NULL);

    if (DEBUG) printf("\t\tDECODE   : dequeue %d, frame %lu\n", l, line->frame);


    if (DEBUG) printf("\t\tDECODE   : dequeue %d, frame %lu, data %p\n", l, line->frame, line->yuvp ? line->yuvp->data[0] : 0);

    picture->opaque = line;
//    picture->pts = line->frame;
//    picture->pkt_pos = line->frame;


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

static void f641_release_buffer(struct AVCodecContext *c, AVFrame *picture) {
    AVPicture *pict = (AVPicture*)picture->opaque;
    if (DEBUG) printf("f641_get_buffer %p (%p) + %p (%p) ; decoder : %p\n", picture, picture->data[0], pict, picture->data[3], c->priv_data);
    if (DEBUG) printf("pict : linesize = %d,%d,%d,%d ; error %lu,%lu,%lu,%lu ; size = (%dx%d) ; format = %d\n" // (%u-%u-%u)\n"
            , picture->linesize[0], picture->linesize[1], picture->linesize[2], picture->linesize[3]
            , picture->error[0], picture->error[1], picture->error[2], picture->error[3]
            , picture->width, picture->height
            , picture->format
//            , picture->data[0][90], picture->data[0][91], picture->data[0][92]
    );
}

//
void *f640_decode(void *video_lines) {
    int i, j, k, m;
    struct f640_video_lines *lines = video_lines;

    AVCodec             *codec;
    AVCodecContext      *decoderCtxt;
    AVFrame             *picture;
    int                 len, got_pict;
    AVPacket            pkt;
    memset(&pkt, 0, sizeof(AVPacket));

    f640_snaped = &lines->buffered;

    codec = avcodec_find_decoder(CODEC_ID_MJPEG);
    f611_dump_codec(codec);

    decoderCtxt = avcodec_alloc_context3(codec);    // PIX_FMT_YUVJ422P
    avcodec_open2(decoderCtxt, codec, NULL);
    decoderCtxt->get_buffer = f641_get_buffer;
    decoderCtxt->release_buffer = f641_release_buffer;
    if (DEBUG) printf("DECODE: Codec OK\n");

    // Data
    picture = avcodec_alloc_frame();

    if (DEBUG) printf("DECODE: FFMpeg data inited\n");

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

//
void *f640_watch_mj(void *video_lines) {
    int i, j, k, ix;
    struct f640_video_lines *lines = video_lines;

    uint8_t *im, *pix;
    long rms;
    int fact_shift = 1;
    int fact = 1 << fact_shift;

    while(1) {
        // Dequeue
        int l = f640_dequeue_line(&lines->decoded);
        struct f640_line *line = &lines->decoded.lineup[l];
        struct f640_line *pline = &lines->decoded.lineup[line->previous_line];
        gettimeofday(&line->tv10, NULL);

        if (DEBUG) printf("\t\tWATCH_MJ   : dequeue %d, frame %lu\n", l, line->frame);

        // Processing
        if (line->frame > 1) {
            ix = 0;
            im  = lines->decoded.lineup[line->previous_line].yuvp->data[0];
            pix = line->yuvp->data[0];

            *(line->rms) = 0;
            memset(line->grid_values, 0, line->grid->num * sizeof(long));
            if (DEBUG) printf("\t\tWATCH_MJ   : image copied\n");

            k = (line->yuvp->linesize[0] << fact_shift) - line->grid->width;
            for(i = line->grid->height >> fact_shift ; i > 0 ; i--) {
                for(j = line->grid->width >> fact_shift ; j > 0 ; j--) {
                    rms = (*pix - *im) * (*pix - *im);
                    *(line->rms) += rms;
                    line->grid_values[line->grid->index_grid[ix]] += rms;
                    ix  += fact;
                    pix += fact;
                    im  += fact;
                }
                ix  += (fact - 1) * line->grid->width;
                pix += k;
                im  += k;
            }
            if (DEBUG) printf("\t\tWATCH_MJ   : rms = %ld\n", *(line->rms) / line->grid->size);

            // Totals
            *(line->rms) /= line->grid->size / (fact * fact);
            *(line->grid_min) = 0xFFFFFFFFFFL;
            *(line->grid_max) = 0;
            for(k = 0 ; k < line->grid->num ; k++) {
                line->grid_values[k] /= line->grid->gsize / (fact * fact);
//                line->grid->grid_coefs[k] += line->grid_values[k];
                if (line->grid_values[k] < *(line->grid_min)) *(line->grid_min) = line->grid_values[k];
                if (line->grid_values[k] > *(line->grid_max)) *(line->grid_max) = line->grid_values[k];
            }
            if (DEBUG) printf("\t\tWATCH_MJ (%d - %ld / %d - %ld)  : rms = %ld, min = %ld, max = %ld\n", l, line->frame, line->previous_line, pline->frame, *(line->rms), *(line->grid_min), *(line->grid_max));
        }

        // Enqueue
        if (DEBUG) printf("\t\tWATCH_MJ   : enqueue %d, frame %lu\n", l, line->frame);
        gettimeofday(&line->tv11, NULL);
        f640_enqueue_line(&lines->watched, l);
    }
    pthread_exit(NULL);
}

//
void *f640_gray_mj(void *video_lines) {
    int i, j, k, ix;
    struct f640_video_lines *lines = video_lines;

    uint8_t *im, *pix;
    long rms;
    int fact_shift = 1;
    int fact = 1 << fact_shift;

    while(1) {
        // Dequeue
        int l = f640_dequeue_line(&lines->watched);
        struct f640_line *line = &lines->watched.lineup[l];
        gettimeofday(&line->tve0, NULL);

        if (DEBUG) printf("\t\tEDGE_MJ   : dequeue %d, frame %lu\n", l, line->frame);

        // Processing
        if (line->frame > 1) {

            if (1) {
                im  = line->gry->data;
                pix = line->yuvp->data[0];
                for(i = line->grid->height ; i > 0 ; i--) {
                    memcpy(im, pix, line->grid->width);
                    im  += line->grid->width;
                    pix += line->yuvp->linesize[0];
                }
            } else {
                im  = line->gry->data + line->grid->width + 1;
                uint8_t *pixc = line->yuvp->data[0];
                uint8_t *pixa = line->yuvp->data[0] + 1;
                uint8_t *pixb = line->yuvp->data[0] + line->yuvp->linesize[0];
                pix = line->yuvp->data[0] + line->yuvp->linesize[0] + 1;

                k = line->yuvp->linesize[0] - line->grid->width + 2;
                for(i = line->grid->height - 1; i > 1 ; i--) {
                    for(j = line->grid->width - 1; j > 1 ; j--) {
                        if (*pixa > *pix) *im = *pixa - *pix;
                        else *im = *pix - *pixa;
                        if (*pixb > *pix) {
                            if (*pixb - *pix > *im) *im = *pixb - *pix;
                        } else {
                            if (*pix - *pixb > *im) *im = *pix - *pixb;
                        }
                        if (*pixc > *pix) {
                            if (*pixc - *pix > *im) *im = *pixc - *pix;
                        } else {
                            if (*pix - *pixc > *im) *im = *pix - *pixc;
                        }
                        im++;
                        pix++;
                        pixa++;
                        pixb++;
                        pixc++;
                    }
                    im   += 2;
                    pix  += k;
                    pixa += k;
                    pixb += k;
                    pixc += k;
                }
            }
            if (DEBUG) printf("\t\tEDGE_MJ   : rms = %ld\n", *(line->rms) / line->grid->size);
        }

        // Enqueue
        if (DEBUG) printf("\t\tEDGE_MJ   : enqueue %d, frame %lu\n", l, line->frame);
        gettimeofday(&line->tve1, NULL);
        f640_enqueue_line(&lines->edged, l);
    }
    pthread_exit(NULL);
}

//
void *f640_watch(void *video_lines) {
    int i, j, k;
    struct f640_video_lines *lines = video_lines;

    uint8_t *im, *pix;
    long rms;

    while(1) {
        // Dequeue
        int l = f640_dequeue_sync(&lines->snaped);
        struct f640_line *line = &lines->snaped.lineup[l];
        gettimeofday(&line->tv10, NULL);

        if (DEBUG) printf("\t\tWATCH   : dequeue %d, frame %lu\n", l, line->frame);

        // Processing
        i = 0;
        im  = line->buffers[line->last].start;
        pix = line->buffers[line->actual].start;
        memcpy(line->yuv->data, pix, line->buffers[line->actual].length);
        *(line->rms) = 0;
        memset(line->grid_values, 0, line->grid->num * sizeof(long));
        if (DEBUG) printf("\t\tWATCH   : image copied\n");

        while( i < line->grid->size ) {
            rms = (*pix - *im) * (*pix - *im);
            *(line->rms) += rms;
            line->grid_values[line->grid->index_grid[i]] += rms;
            i++;
            im  += 2;
            pix += 2;
        }
        if (DEBUG) printf("\t\tWATCH   : rms = %ld\n", *(line->rms) / line->grid->size);

        // Totals
        *(line->rms) /= line->grid->size;
        *(line->grid_min) = 0xFFFFFFFFFFL;
        *(line->grid_max) = 0;
        for(k = 0 ; k < line->grid->num ; k++) {
            line->grid_values[k] /= line->grid->gsize;
            line->grid->grid_coefs[k] += line->grid_values[k];
            if (line->grid_values[k] < *(line->grid_min)) *(line->grid_min) = line->grid_values[k];
            if (line->grid_values[k] > *(line->grid_max)) *(line->grid_max) = line->grid_values[k];
        }

        // Enqueue
        if (DEBUG) printf("\t\tWATCH   : enqueue %d, frame %lu\n", l, line->frame);
        gettimeofday(&line->tv11, NULL);
        f640_sync(&lines->snaped, line->frame);
        f640_enqueue_line(&lines->watched, l);
    }
    pthread_exit(NULL);
}

//
void *f640_convert(void *video_lines) {
    int ix, k;
    struct f640_video_lines *lines = video_lines;
    struct timeval tv;

    gettimeofday(&tv, NULL);

    AVFrame *picture = avcodec_alloc_frame();
    avcodec_get_frame_defaults(picture);
    avpicture_alloc((AVPicture *)picture, PIX_FMT_YUYV422, lines->grid->width, lines->grid->height);
    picture->width = lines->grid->width;
    picture->height = lines->grid->height;
    picture->format = PIX_FMT_YUYV422;

    AVFrame *scaled = avcodec_alloc_frame();
    avcodec_get_frame_defaults(scaled);
    avpicture_alloc((AVPicture *)scaled, PIX_FMT_BGR24, lines->grid->width, lines->grid->height);
    scaled->width = lines->grid->width;
    scaled->height = lines->grid->height;
    scaled->format = PIX_FMT_BGR24;


    while(1) {
        int l = f640_dequeue_line(&lines->edged);
        struct f640_line *line = &lines->edged.lineup[l];
        gettimeofday(&line->tv20, NULL);

        if (DEBUG) printf("\t\t\t\tCONVERT : dequeue %d, frame %lu\n", l, line->frame);

        // Alert
        line->flaged = 0;
//        if (line->frame > 35 && line->grid_max > 0) {//line->grid_th) {
        if (1) {
//            f640_full_yuv_to_rgb(line->yuv, line->rgb);
            picture->data[0] = line->yuv->data;
            scaled->data[0] = line->rgb->data;
            //sws_scale(line->swsCtxt, (const uint8_t**)picture->data, picture->linesize, 0, line->grid->height, scaled->data, scaled->linesize);
            sws_scale(line->swsCtxt, (const uint8_t**)line->yuvp->data, line->yuvp->linesize, 0, line->grid->height, scaled->data, scaled->linesize);

            if (line->dstFormat == PIX_FMT_ABGR) {
                for(k = 0 ; k < line->grid->size ; k++) {
                    //line->rgb->data[k << 2] = line->yuv->data[k << 1];
                    line->rgb->data[k << 2] = 0x7F;
                }
            }

            pthread_mutex_lock(&lines->grid->mutex_coefs);
            for(k = 0 ; k < line->grid->num ; k++) {
                if ( line->grid->grid_ratio[(k << 1) + 1] * line->grid_values[k] > line->grid->grid_ratio[k << 1] * line->grid_th) {
                    int i, j;
                    ix = line->grid->grid_index[k];
                    if (line->dstFormat != PIX_FMT_ABGR) {
                        //f640_draw_rect(line->yuv, ix, line->grid->wlen, line->grid->hlen);
                        f640_draw_rect(line->rgb, ix, line->grid->wlen, line->grid->hlen);
                        line->flaged = 1;
                    } else {
                        for(j = 0 ; j < line->grid->hlen ; j++) {
                            for(i = 0 ; i < line->grid->wlen ; i++) {
                                line->rgb->data[(ix++) << 2] = 0xFF;
                            }
                            ix += line->grid->width - line->grid->wlen;
                        }
                    }
                }
            }
            pthread_mutex_unlock(&lines->grid->mutex_coefs);

            ix = f640_draw_number(line->rgb, line->grid->width - 5, line->grid->height - 5, line->tv00.tv_sec - tv.tv_sec);
            ix = f640_draw_number(line->rgb, ix - 20, line->grid->height - 5, line->frame);
            ix = f640_draw_number(line->rgb, ix - 20, line->grid->height - 5, *(line->grid_max));
        }

        if (DEBUG) printf("\t\t\t\tCONVERT : enqueue %d, frame %lu\n", l, line->frame);
        gettimeofday(&line->tv21, NULL);
        f640_enqueue_line(&lines->converted, l);
    }
    pthread_exit(NULL);
}

//
void *f640_record(void *video_lines) {
    int frame = 1, ix;
    struct f640_video_lines *lines = video_lines;
    struct timeval tv;
    struct tm tm1;

    gettimeofday(&tv, NULL);
    while(1) {
        int l = f640_dequeue_line_number(&lines->converted, frame++);
        struct f640_line *line = &lines->converted.lineup[l];
        gettimeofday(&line->tv30, NULL);

        if (DEBUG) printf("\t\t\t\t\t\tRECORD  : dequeue %d, frame %lu\n", l, line->frame);

        //f051_send_data(line->log_env, line->rgb->data, line->rgb->data_size);
        write(lines->fd_grid, line->width, 8 * (6 + *(line->rows) * *(line->cols)));
        write(lines->fd_stream, line->rgb->data, line->rgb->data_size);

        if (*(line->grid_max) > line->grid_th && line->frame > 35) {
            //ix = f640_draw_number(line->yuv, line->grid->width - 5, line->grid->height - 5, line->tv00.tv_sec - tv.tv_sec);
            localtime_r(&line->tv00.tv_sec, &tm1);
            ix = f640_draw_number(line->yuv, line->grid->width - 5,          line->grid->height - 5, tm1.tm_sec);
            ix = f640_draw_number(line->yuv, ix - line->yuv->glyphs.width/2, line->grid->height - 5, tm1.tm_min);
            ix = f640_draw_number(line->yuv, ix - line->yuv->glyphs.width/2, line->grid->height - 5, tm1.tm_hour);

            ix = f640_draw_number(line->yuv, ix - 20              , line->grid->height - 5, line->frame);
            ix = f640_draw_number(line->yuv, ix - 20              , line->grid->height - 5, *(line->grid_max));

            ix = f640_draw_number(line->yuv, line->grid->width - 5,          line->grid->height - 8 - line->yuv->glyphs.height, tm1.tm_mday);
            ix = f640_draw_number(line->yuv, ix - line->yuv->glyphs.width/2, line->grid->height - 8 - line->yuv->glyphs.height, tm1.tm_mon+1);


            //f611_add_frame(line->stream, line->yuv->data);    // YUV422 -> MPEG

//            pkt.data = line->buffers[line->actual].start;
//            pkt.size = line->buffers[line->actual].length;
//            f611_add_mjpeg_frame(line->stream, line->buffers[line->actual].start);

            lines->recorded_frames++;
        }


        //
        if (DEBUG) printf("\t\t\t\t\t\tRECORD  : enqueue %d, frame %lu\n", l, line->frame);
        gettimeofday(&line->tv31, NULL);
        f640_enqueue_line(&lines->recorded, l);
    }
    pthread_exit(NULL);
}

//
void *f640_record_mj(void *video_lines) {
    int frame = 1, ix, r;
    struct f640_video_lines *lines = video_lines;
    struct timeval tv;
    struct tm tm1;

    AVFormatContext *outputFile;


    avformat_alloc_output_context2(&outputFile, NULL, NULL, "/work/test/loulou.avi");
    outputFile->start_time_realtime = 1500000000;
    r = avio_open(&outputFile->pb, "/work/test/loulou.avi", AVIO_FLAG_WRITE);
    AVStream *st = av_new_stream(outputFile, 0);
    AVCodec *encoder = avcodec_find_encoder(CODEC_ID_MJPEG);
    st->codec->pix_fmt       = PIX_FMT_YUVJ422P;
    st->codec->coded_width   = lines->grid->width;
    st->codec->coded_height  = lines->grid->height;
    st->codec->time_base.num = 1;
    st->codec->time_base.den = 30;
    st->codec->sample_fmt = SAMPLE_FMT_S16;
    st->codec->sample_rate = 44100;
    r = avcodec_open2(st->codec, encoder, NULL);
    st->time_base.num = 1;
    st->time_base.den = 30;
    st->r_frame_rate.num = 30;
    st->r_frame_rate.den = 1;
    st->avg_frame_rate.num = 30;
    st->avg_frame_rate.den = 1;
    st->pts.num = 1;
    st->pts.den = 1;
    st->pts.val = 1;

    AVPacket pkt;
    av_init_packet(&pkt);
    av_new_packet(&pkt, 2 * lines->grid->width * lines->grid->height);
    r = avformat_write_header(outputFile, NULL);



    gettimeofday(&tv, NULL);
    while(1) {
        int l = f640_dequeue_line_number(&lines->converted, frame++);
        struct f640_line *line = &lines->converted.lineup[l];
        gettimeofday(&line->tv30, NULL);

        if (DEBUG) printf("\t\t\t\t\t\tRECORD  : dequeue %d, frame %lu\n", l, line->frame);

        write(lines->fd_grid, line->width, sizeof(long) * (6 + *(line->rows) * *(line->cols)));
        if (lines->fd_edge > 0) write(lines->fd_edge, line->gry->data, line->gry->data_size);
        write(lines->fd_stream, line->rgb->data, line->rgb->data_size);


        //if (*(line->grid_max) > line->grid_th && line->frame > 35 ) {
        if ( line->flaged ) {
            pkt.data = line->buffers[line->actual].start;
            pkt.size = line->buffers[line->actual].length;
            pkt.pts  = lines->recorded_frames;
            pkt.dts  = pkt.pts;
            pkt.duration = 1;
            pkt.pos  = -1;
            pkt.stream_index = 0;
            r = av_write_frame(outputFile, &pkt);
            //printf("Write frame return %d\n", r);
            avio_flush(outputFile->pb);

            lines->recorded_frames++;
        }

        //
        if (DEBUG) printf("\t\t\t\t\t\tRECORD  : enqueue %d, frame %lu\n", l, line->frame);
        gettimeofday(&line->tv31, NULL);
        f640_enqueue_line(&lines->recorded, l);
    }
    pthread_exit(NULL);
}


//
void *f640_release(void *video_lines) {
    struct f640_video_lines *lines = video_lines;
    double t = 0, t0 = 0, td = 0, t1 = 0, t2 = 0, te = 0, t3 = 0, t4 = 0;
    struct timeval tv1, tv2, tv3;
    struct timespec ts1;
    struct tm tm1;
    time_t time70;
    clock_t times1, times2;
    unsigned long jiffs, ms;
    int show_freq = lines->fps;
    long recorded_frames = 0;

    gettimeofday(&tv1, NULL);
    while(1) {
        int l = f640_dequeue_line(&lines->recorded);
        struct f640_line *line = &lines->recorded.lineup[l];
        gettimeofday(&line->tv40, NULL);

        // Agreg
        lines->rms += *(line->rms);
        if (lines->grid_min_value > *(line->grid_min)) lines->grid_min_value = *(line->grid_min);
        if (lines->grid_max_value < *(line->grid_max)) lines->grid_max_value = *(line->grid_max);

        t  += f640_duration (line->tv40, line->tv10);
        td += f640_duration (line->tvd1, line->tvd0);   // decode
        t1 += f640_duration (line->tv11, line->tv10);   // watch
        te += f640_duration (line->tve1, line->tve0);   // edge
        t2 += f640_duration (line->tv21, line->tv20);   // convert
        t3 += f640_duration (line->tv31, line->tv30);   // record
        t4 += f640_duration (line->tvd1, line->tvd0)
           +  f640_duration (line->tv11, line->tv10)
           +  f640_duration (line->tve1, line->tve0)
           +  f640_duration (line->tv21, line->tv20)
           +  f640_duration (line->tv31, line->tv30);

        if (DEBUG) printf("\t\t\t\t\t\t\t\tRELEASE : dequeue %d, frame %lu\n", l, line->frame);
        if (line->frame % show_freq == 0) {
            gettimeofday(&tv2, NULL);
            localtime_r(&tv2.tv_sec, &tm1);
            times2 = clock();// / CLOCKS_PER_SECOND ~ 1 000 000;
            timersub(&tv2, &tv1, &tv3);
//            time(&time70);
//            jiffs = clock_t_to_jiffies(times);
//            ms = jiffies_to_msecs(jiffs);

            printf("%02d/%02d %02d:%02d:%02d |%3.1fHz %3lu%||"
                    , tm1.tm_mon+1, tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec
                    , 1. * (show_freq * 1000000) / (1000000 * tv3.tv_sec + tv3.tv_usec)
                    , 100 * (times2 - times1) / (1000000 * tv3.tv_sec + tv3.tv_usec)
            );

            //if ( lines->grid_max_value > line->grid_th || (lines->recorded_frames - recorded_frames) ) {
            if ( lines->recorded_frames - recorded_frames ) {
                printf("%2d|¤de %2.0f% |%2d|¤wa %2.0f %2ld - %2ld - " F640_RESET F640_BOLD F640_FG_RED "%5ld" F640_RESET
                        " |%2d|¤ed %2.0f% |%2d|¤cv %2.0f% |%2d|¤rc %2.0f " F640_RESET F640_BOLD F640_FG_RED "+%2ld" F640_RESET
                        " |%2d|%2d|¤%lds - %ldMo"
                        , f640_uns_queue_size(&lines->snaped),    100*td/t4
                        , f640_uns_queue_size(&lines->decoded),   100*t1/t4, lines->grid_min_value, lines->rms / show_freq, lines->grid_max_value
                        , f640_uns_queue_size(&lines->watched),   100*te/t4
                        , f640_uns_queue_size(&lines->edged),     100*t2/t4
                        , f640_uns_queue_size(&lines->converted), 100*t3/t4, lines->recorded_frames - recorded_frames
                        , f640_uns_queue_size(&lines->recorded)
                        , f640_uns_queue_size(&lines->released)
                        , lines->recorded_frames / lines->fps
                        , lines->recorded_frames * 2 * lines->grid->size / (1024 * 1024)
                );
            } else {
                printf("%2d|¤de %2.0f% |%2d|¤wa %2.0f %2ld - %2ld - %5ld |%2d|¤ed %2.0f% |%2d|¤cv %2.0f% |%2d|¤rc %2.0f +%2ld |%2d|%2d|¤%lds - %ldMo"
                        , f640_uns_queue_size(&lines->snaped),    100*td/t4
                        , f640_uns_queue_size(&lines->decoded) ,  100*t1/t4, lines->grid_min_value, lines->rms / show_freq, lines->grid_max_value
                        , f640_uns_queue_size(&lines->watched),   100*te/t4
                        , f640_uns_queue_size(&lines->edged),     100*t2/t4
                        , f640_uns_queue_size(&lines->converted), 100*t3/t4, lines->recorded_frames - recorded_frames
                        , f640_uns_queue_size(&lines->recorded)
                        , f640_uns_queue_size(&lines->released)
                        , lines->recorded_frames / lines->fps
                        //, lines->recorded_frames * lines->grid->size / (1536 * 1024)
                        , lines->recorded_frames * 2 * lines->grid->size / (1024 * 1024)
                );
            }
            printf("\n");
            t  = 0;
            td = 0;
            t1 = 0;
            te = 0;
            t2 = 0;
            t3 = 0;
            t4 = 0;
            lines->rms = 0;
            lines->grid_min_value = 0xFFFFFFFFFFL;
            lines->grid_max_value = 0;
            recorded_frames = lines->recorded_frames;
            tv1.tv_sec = tv2.tv_sec; tv1.tv_usec = tv2.tv_usec;
            times1 = times2;
        }

        gettimeofday(&line->tv41, NULL);

        if (DEBUG) printf("\t\t\t\t\t\t\t\tFrame %lu : - %5.0f - %5.0f - %5.0f = %4.3fms (%5.0f)\n", line->frame
                , f640_frequency(line->tv11, line->tv10)
                , f640_frequency(line->tv21, line->tv20)
                , f640_frequency(line->tv31, line->tv30)
                , f640_duration (line->tv41, line->tv00)
                , f640_frequency(line->tv41, line->tv00)
        );
//        printf("Frame " F640_BOLD "%4d" F640_RESET
//            " | " F640_BOLD "RMS" F640_RESET " = " F640_FG_RED "%5.1f" F640_RESET
//            " | ABS = %5.2f | min %4.1f | MAX %5.0f | N %5.0f |\n"// %u %u %u %u %u %u %u %u %u %u %u\n"
//            , frame, rms, 1.*moy/size, 0, 0, 0
//            //, y[6], y[7], y[8], y[9], y[10], y[11], y[12], y[13], y[14], y[15], y[16]
//        );
//        printf(" %3.0fms | " F640_FG_RED "%4.1fHz" F640_RESET " | seq %4u | %3u | %2u | %2u | %6u | %6u\n"
//                , frame, 1000 * d1, 1/d1
//                , buf.sequence, buf.timecode.minutes, buf.timecode.seconds, buf.timecode.frames, buf.bytesused, buf.length);

        f640_enqueue_line(&lines->released, l);

        // EnQueue
//        //pthread_mutex_lock(&lines->ioc);
//        if(ioctl(lines->fd, VIDIOC_QBUF, &line->buf) == -1) {
//            printf("!!!!!!!!!!!! VIDIOC_QBUF: %s !!!!!!!!!!!!!!!!!!\n", strerror(errno));
//        }
//        //pthread_mutex_unlock(&lines->ioc);
    }
    pthread_exit(NULL);
}

/*
 *
 */
void* f640_read(void *video_lines) {
    int r;
    struct f640_video_lines *lines = video_lines;
    void *grid = calloc(1, sizeof(long) * (6 + lines->grid->cols * lines->grid->rows));

    while(1) {
        r = read(lines->fd_get, grid, sizeof(long) * (6 + lines->grid->cols * lines->grid->rows) );
        if (r == sizeof(long) * (6 + lines->grid->cols * lines->grid->rows)) {
            printf("READ  %dbytes : %dx%d\n", r, *((int*)(grid+16)), *((int*)(grid+20)));
            pthread_mutex_lock(&lines->grid->mutex_coefs);
            memcpy(lines->grid->grid_coefs, grid + 6 * sizeof(long), r - 6 * sizeof(long));
            pthread_mutex_unlock(&lines->grid->mutex_coefs);
        } else if (r > 0) {
          printf("READ: wrong size read (%d), discarding\n", r);
        } else if (r) {
            printf("READ: Interrupt %d\n", r);
            usleep(1000*1000);  // bug
        }
    }

    pthread_exit(NULL);
}

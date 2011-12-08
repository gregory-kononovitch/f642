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

static int DEBUG = 0;

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
                grid->width, grid->height, f640_lineup[i].srcFormat,
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
    queue->nb_in++;
    struct f640_line *line = &queue->lineup[queue->lines[queue->last]];
    gettimeofday(&queue->lineup[queue->lines[queue->last]].tv00, NULL);
    memcpy(&line->buf, v4l2_buffer, sizeof(struct v4l2_buffer));
    line->actual = line->buf.index;
    line->last   = queue->lineup[queue->lines[(queue->last-1) % queue->size]].actual;
    line->frame  = queue->nb_in;
    //line->yuv->data = line->buffers[line->index].start;

    pthread_mutex_lock(&queue->mutex);
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
        int l = f640_dequeue_line(&lines->watched);
        struct f640_line *line = &lines->watched.lineup[l];
        gettimeofday(&line->tv20, NULL);

        if (DEBUG) printf("\t\t\t\tCONVERT : dequeue %d, frame %lu\n", l, line->frame);

        // Alert
//        if (line->frame > 35 && line->grid_max > 0) {//line->grid_th) {
        if (1) {
//            f640_full_yuv_to_rgb(line->yuv, line->rgb);
            picture->data[0] = line->yuv->data;
            scaled->data[0] = line->rgb->data;
            sws_scale(line->swsCtxt, (const uint8_t**)picture->data, picture->linesize, 0, line->grid->height, scaled->data, scaled->linesize);

            if (line->dstFormat == PIX_FMT_ABGR) {
                for(k = 0 ; k < line->grid->size ; k++) {
                    //line->rgb->data[k << 2] = line->yuv->data[k << 1];
                    line->rgb->data[k << 2] = 0x7F;
                }
            }

            for(k = 0 ; k < line->grid->num ; k++) {
                if (line->grid_values[k] > line->grid_th) {
                    int i, j;
                    ix = line->grid->grid_index[k];
                    if (line->dstFormat != PIX_FMT_ABGR) {
                        //f640_draw_rect(line->yuv, ix, line->grid->wlen, line->grid->hlen);
                        f640_draw_rect(line->rgb, ix, line->grid->wlen, line->grid->hlen);
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
    int fd1 = open("/dev/t030/t030-1", O_WRONLY);
    int fd2 = open("/dev/t030/t030-2", O_WRONLY);

    gettimeofday(&tv, NULL);
    while(1) {
        int l = f640_dequeue_line_number(&lines->converted, frame++);
        struct f640_line *line = &lines->converted.lineup[l];
        gettimeofday(&line->tv30, NULL);

        if (DEBUG) printf("\t\t\t\t\t\tRECORD  : dequeue %d, frame %lu\n", l, line->frame);

        //f051_send_data(line->log_env, line->rgb->data, line->rgb->data_size);
        write(fd2, line->width, 6 + *(line->rows) * *(line->cols));
        write(fd1, line->rgb->data, line->rgb->data_size);

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


            f611_add_frame(line->stream, line->yuv->data);
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
    double t = 0, t0 = 0, t1 = 0, t2 = 0, t3 = 0, t4 = 0;
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
        t1 += f640_duration (line->tv11, line->tv10);
        t2 += f640_duration (line->tv21, line->tv20);
        t3 += f640_duration (line->tv31, line->tv30);
        t4 += f640_duration (line->tv11, line->tv10) + f640_duration (line->tv21, line->tv20) + f640_duration (line->tv31, line->tv30);

        if (DEBUG) printf("\t\t\t\t\t\t\t\tRELEASE : dequeue %d, frame %lu\n", l, line->frame);
        if (line->frame % show_freq == 0) {
            gettimeofday(&tv2, NULL);
            localtime_r(&tv2.tv_sec, &tm1);
            times2 = clock();// / CLOCKS_PER_SECOND ~ 1 000 000;
            timersub(&tv2, &tv1, &tv3);
//            time(&time70);
//            jiffs = clock_t_to_jiffies(times);
//            ms = jiffies_to_msecs(jiffs);

            printf("RELEASE: %02d/%02d %02d:%02d:%02d | %3.1fHz - %3lu% || "
                    , tm1.tm_mon+1, tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec
                    , 1. * (show_freq * 1000000) / (1000000 * tv3.tv_sec + tv3.tv_usec)
                    , 100 * (times2 - times1) / (1000000 * tv3.tv_sec + tv3.tv_usec)
            );

            if ( lines->grid_max_value > line->grid_th || (lines->recorded_frames - recorded_frames) ) {
                printf("%2d |¤ wa %2.0f%, %2ld - %2ld - " F640_RESET F640_BOLD F640_FG_RED "%5ld" F640_RESET
                        " ¤| %2d |¤ cv %2.0f% ¤| %2d |¤ rc %2.0f%, " F640_RESET F640_BOLD F640_FG_RED "+%2ld" F640_RESET
                        " ¤| %2d | %2d |¤ %lds - %ldMo"
                        , f640_uns_queue_size(&lines->snaped) ,   100*t1/t4, lines->grid_min_value, lines->rms / show_freq, lines->grid_max_value
                        , f640_uns_queue_size(&lines->watched),   100*t2/t4
                        , f640_uns_queue_size(&lines->converted), 100*t3/t4, lines->recorded_frames - recorded_frames
                        , f640_uns_queue_size(&lines->recorded)
                        , f640_uns_queue_size(&lines->released)
                        , lines->recorded_frames / lines->fps, lines->recorded_frames * lines->grid->size / (1536 * 1024)
                );
            } else {
                printf("%2d |¤ wa %2.0f%, %2ld - %2ld - %5ld ¤| %2d |¤ cv %2.0f% ¤| %2d |¤ rc %2.0f%, +%2ld ¤| %2d | %2d |¤ %lds - %ldMo"
                        , f640_uns_queue_size(&lines->snaped) ,   100*t1/t4, lines->grid_min_value, lines->rms / show_freq, lines->grid_max_value
                        , f640_uns_queue_size(&lines->watched),   100*t2/t4
                        , f640_uns_queue_size(&lines->converted), 100*t3/t4, lines->recorded_frames - recorded_frames
                        , f640_uns_queue_size(&lines->recorded)
                        , f640_uns_queue_size(&lines->released)
                        , lines->recorded_frames / lines->fps, lines->recorded_frames * lines->grid->size / (1536 * 1024)
                );
            }
            printf("\n");
            t  = 0;
            t1 = 0;
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

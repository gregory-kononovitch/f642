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

static int DEBUG = 1;

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
    //
    grid->cols = grid->width  / grid->wlen + ((grid->width  % grid->wlen) ? 1 : 0);
    grid->rows = grid->height / grid->hlen + ((grid->height % grid->hlen) ? 1 : 0);
    grid->num  = grid->rows * grid->cols;
    //
    grid->index = calloc(grid->width * grid->height, sizeof(uint16_t));
    if (!grid->index) {
        printf("ENOMEM in allocating grid index, returning\n");
        free(grid);
        return NULL;
    }
    for(i = 0 ; i < grid->size ; i++) grid->index[i] = ( i % grid->width ) / grid->wlen + ( ( i / grid->width ) / grid->hlen ) * grid->cols; // @@@ buggy

    return grid;
}


/*
 *
 */
struct f640_line* f640_make_lineup(v4l2_buffer_t *buffers, int nbuffers, struct f640_grid *grid, struct output_stream *stream, struct f051_log_env *log_env, double threshold) {
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
        f640_lineup[i].grid_values  = calloc(grid->rows * grid->cols, sizeof(double));
        if (!f640_lineup[i].grid_values) {
            printf("ENOMEM allocating grid_values, returning\n");
            for(--i ; i > -1 ; i--) {
                free(f640_lineup[i].rgb->data);
                free(f640_lineup[i].grid_values);
            }
            free(f640_lineup);
            f640_lineup = NULL;
            return NULL;
        }
        f640_lineup[i].grid_th = threshold;

        f640_lineup[i].yuv          = f640_create_yuv_image(grid->width, grid->height);
        free(f640_lineup[i].yuv->data);
        f640_lineup[i].rgb          = f640_create_rgb_image(grid->width, grid->height);
        if (!f640_lineup[i].rgb) {
            printf("ENOMEM allocating grid_values, returning\n");
            free(f640_lineup[i].grid_values);
            for(--i ; i > -1 ; i--) {
                free(f640_lineup[i].rgb->data);
                free(f640_lineup[i].grid_values);
            }
            free(f640_lineup);
            f640_lineup = NULL;
            return NULL;
        }

        f640_lineup[i].log_env        = log_env;
        f640_lineup[i].stream         = stream;

        //
        f640_lineup[i].swsCtxt = sws_getCachedContext(f640_lineup[i].swsCtxt,
                grid->width, grid->height, PIX_FMT_YUYV422,
                grid->width, grid->height, PIX_FMT_RGB24,
                SWS_BICUBIC, NULL, NULL, NULL
        );

    }
    return f640_lineup;
}


/*
 *
 */
int f640_init_queue(struct f640_video_queue *queue, struct f640_line *lineup, int count) {
    queue->lineup   = lineup;
    queue->size     = count;
    queue->lines     = calloc(queue->size, sizeof(int));
    queue->last     = 0;
    queue->next     = 0;
    queue->real     = 0;
    queue->cpt      = 0;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init (&queue->cond , NULL);
    return 0;
}

int f640_enqueue_buffer(struct f640_video_queue *queue, struct v4l2_buffer *v4l2_buffer) {
    queue->cpt++;
    struct f640_line *line = &queue->lineup[queue->lines[queue->last]];

    memcpy(&line->buf, v4l2_buffer, sizeof(struct v4l2_buffer));
    line->actual = line->buf.index;
    line->last   = queue->lineup[queue->lines[(queue->last-1) % queue->size]].actual;
    line->frame  = queue->cpt;
    line->yuv->data = line->buffers[line->index].start;
    gettimeofday(&queue->lineup[queue->lines[queue->last]].tv00, NULL);

    pthread_mutex_lock(&queue->mutex);
    queue->last = (++queue->last) % queue->size;
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);


    return 0;
}

int f640_enqueue_line(struct f640_video_queue *queue, int line) {
    pthread_mutex_lock(&queue->mutex);
    queue->lines[queue->last] = line;
    queue->last = (++queue->last) % queue->size;
    queue->cpt++;
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);
    return 0;
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


//
void *f640_watch(void *video_lines) {
    int i, j, k;
    struct f640_video_lines *lines = video_lines;

    uint8_t *im, *pix;
    double r, rms;

    while(1) {
        int l = f640_dequeue_line(&lines->snaped);
        struct f640_line *line = &lines->snaped.lineup[l];
        gettimeofday(&line->tv10, NULL);

        if (DEBUG) printf("WATCH : dequeue %d, frame %lu\n", l, line->frame);

        // Processing
        i = 0;
        im  = line->buffers[line->last].start;
        pix = line->buffers[line->actual].start;
        rms = 0;
        //line->yuv->data = line->buffers[line->actual].start;
        memset(line->grid_values, 0, line->grid->num * sizeof(double));
        while( i < line->grid->size ) {
            r = (*pix - *im) * (*pix - *im);
            rms += r;
            line->grid_values[line->grid->index[i]] += r;
            i++;
            im  += 2;
            pix += 2;
        }

        // Totals
        rms /= line->grid->size;
        line->grid_min = 0xFFFFFFFFFFL;
        line->grid_max = 0;
        for(k = 0 ; k < line->grid->num ; k++) {
            line->grid_values[k] /= line->grid->wlen * line->grid->hlen;
            if (line->grid_values[k] < line->grid_min) line->grid_min = line->grid_values[k];
            if (line->grid_values[k] > line->grid_max) line->grid_max = line->grid_values[k];

            if (line->grid_values[k] > line->grid_th) {
                //f640_draw_rect(yuv, (k/nbc) * cary * cwidth + (k % nbc) * carx, carx, cary);
//                    f640_draw_rect(rgb, (k/nbc) * cary * cwidth + (k % nbc) * carx, carx, cary);
            }
        }

        if (DEBUG) printf("WATCH : enqueue %d, frame %lu\n", l, line->frame);
        gettimeofday(&line->tv11, NULL);
        f640_enqueue_line(&lines->watched, l);
    }
    pthread_exit(NULL);
}

//
void *f640_convert(void *video_lines) {
    int ix;
    struct f640_video_lines *lines = video_lines;

    AVFrame *picture = avcodec_alloc_frame();
    avcodec_get_frame_defaults(picture);
    avpicture_alloc((AVPicture *)picture, PIX_FMT_YUYV422, 800, 600);
    picture->width = 800;
    picture->height = 600;
    picture->format = PIX_FMT_YUYV422;

    AVFrame *scaled = avcodec_alloc_frame();
    avcodec_get_frame_defaults(scaled);
    avpicture_alloc((AVPicture *)scaled, PIX_FMT_RGB24, 800, 600);
    scaled->width = 800;
    scaled->height = 600;
    scaled->format = PIX_FMT_RGB24;


    while(1) {
        int l = f640_dequeue_line(&lines->watched);
        struct f640_line *line = &lines->watched.lineup[l];
        gettimeofday(&line->tv20, NULL);

        if (DEBUG) printf("CONVERT : dequeue %d, frame %lu\n", l, line->frame);

        // Alert
//        if (line->frame > 35 && line->grid_max > 0) {//line->grid_th) {
        if (1) {
            //f640_full_yuv_to_rgb(line->yuv, line->rgb);
            picture->data[0] = line->yuv->data;
            scaled->data[0] = line->rgb->data;
            sws_scale(line->swsCtxt, (const uint8_t**)picture->data, picture->linesize, 0, line->grid->height, scaled->data, scaled->linesize);

            ix = f640_draw_number(line->rgb, line->grid->width - 5, line->grid->height - 5, line->frame);
            f640_draw_number(line->rgb, ix - 20, line->grid->height - 5, line->grid_max);

        }

        if (DEBUG) printf("CONVERT : enqueue %d, frame %lu\n", l, line->frame);
        gettimeofday(&line->tv21, NULL);
        f640_enqueue_line(&lines->converted, l);
    }
    pthread_exit(NULL);
}

//
void *f640_record(void *video_lines) {
    int frame = 1;
    struct f640_video_lines *lines = video_lines;

    while(1) {
        int l = f640_dequeue_line_number(&lines->converted, frame++);
        struct f640_line *line = &lines->converted.lineup[l];
        gettimeofday(&line->tv30, NULL);

        if (DEBUG) printf("RECORD : dequeue %d, frame %lu  | %p ; %p ; %d | %p ; %p |\n", l, line->frame
                , line->log_env, line->rgb->data, line->rgb->data_size
                , line->stream, line->yuv->data
        );

        f051_send_data(line->log_env, line->rgb->data, line->rgb->data_size);
        f611_add_frame(line->stream, line->yuv->data);

        if (DEBUG) printf("RECORD : enqueue %d, frame %lu\n", l, line->frame);
        gettimeofday(&line->tv31, NULL);
        f640_enqueue_line(&lines->recorded, l);
    }
    pthread_exit(NULL);
}

//
#include <sys/ioctl.h>
#include <linux/videodev2.h>
void *f640_release(void *video_lines) {
    struct f640_video_lines *lines = video_lines;

    while(1) {
        int l = f640_dequeue_line(&lines->recorded);
        struct f640_line *line = &lines->recorded.lineup[l];
        gettimeofday(&line->tv40, NULL);

        if (DEBUG) printf("RELEASE : dequeue %d, frame %lu\n", l, line->frame);

        gettimeofday(&line->tv41, NULL);

        if (DEBUG) printf("Frame %lu : - %5.0f - %5.0f - %5.0f = %4.3fms (%5.0f)\n", line->frame
                , f640_frequency(line->tv11, line->tv10)
                , f640_frequency(line->tv21, line->tv20)
                , f640_frequency(line->tv31, line->tv30)
                , f640_duration (line->tv41, line->tv00)
                , f640_frequency(line->tv41, line->tv00)
        );
        f640_free_line(&lines->snaped, l);

        // EnQueue
        //pthread_mutex_lock(&lines->ioc);
        if(ioctl(lines->fd, VIDIOC_QBUF, &line->buf) == -1) {
            printf("!!!!!!!!!!!! VIDIOC_QBUF: %s !!!!!!!!!!!!!!!!!!\n", strerror(errno));
        }
        //pthread_mutex_unlock(&lines->ioc);
    }
    pthread_exit(NULL);
}

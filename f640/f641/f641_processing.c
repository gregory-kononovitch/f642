/*
 * file    : f641_processing.c
 * project : f640
 *
 * Created on: Dec 20, 2011
 * Author and copyright (C) 2011 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */



#include "f641.h"

static int DEBUG = 0;


/***************************************
 *      WATCHING 422P THREAD
 ***************************************/

//
static int f641_exec_watching_422p(void *appli, void *ressources, struct f640_stone *stone) {
    struct f641_appli *app = (struct f641_appli*)appli;
    struct f640_line *line = (struct f640_line*) stone->private;

    int i, j, k, ix;

    uint8_t *im, *pix;
    int32_t *grid2;
    long rms;
    int fact_shift = 1;
    int fact = 1 << fact_shift;

    gettimeofday(&line->tv10, NULL);

    if (DEBUG) printf("\t\tWATCH_MJ   : dequeue %d, frame %lu\n", line->index, line->frame);

    // Processing
    if (line->frame > 1) {
        ix = 0;
        im  = line->lineup[line->previous_line].yuvp->data[0];
        pix = line->yuvp->data[0];
        grid2 = app->process->grid2->grid_ratio_0 + (line->frame & app->process->grid2->mask0) * app->process->grid->num;

        *(line->rms) = 0;
        memset(line->grid_values, 0, line->grid->num * sizeof(long));
        memset(grid2, 0, line->grid->num * sizeof(int32_t));
        if (DEBUG) printf("\t\tWATCH_MJ   : image copied\n");

        k = (line->yuvp->linesize[0] << fact_shift) - line->grid->width;
        for(i = line->grid->height >> fact_shift ; i > 0 ; i--) {
            for(j = line->grid->width >> fact_shift ; j > 0 ; j--) {
                rms = (*pix - *im) * (*pix - *im);
                *(line->rms) += rms;
                line->grid_values[line->grid->index_grid[ix]] += rms;
                grid2[line->grid->index_grid[ix]] += rms;
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
            grid2[k] /= line->grid->gsize / (fact * fact);

            if (line->grid_values[k] < *(line->grid_min)) *(line->grid_min) = line->grid_values[k];
            if (line->grid_values[k] > *(line->grid_max)) *(line->grid_max) = line->grid_values[k];
        }
        if (DEBUG) printf("\t\tWATCH_MJ (%d - %ld / %d - ?)  : rms = %ld, min = %ld, max = %ld\n", line->index, line->frame, line->previous_line, *(line->rms), *(line->grid_min), *(line->grid_max));
    }

    // Enqueue
    if (DEBUG) printf("\t\tWATCH_MJ   : enqueue %d, frame %lu\n", line->index, line->frame);
    gettimeofday(&line->tv11, NULL);

//    struct timeval tv;
//    timersub(&line->tv11, &line->tv10, &tv);
//    printf("Watching : %.1fms\n", 0.001 * tv.tv_usec);

    return 0;
}

//
void f641_attrib_watching_422p(struct f641_thread_operations *ops) {
        ops->init = NULL;
        ops->updt = NULL;
        ops->exec = f641_exec_watching_422p;
        ops->free = NULL;
}


/******************************
 *      TAGGING THREAD
 ******************************/
static int f641_exec_tagging(void *appli, void *ressources, struct f640_stone *stone) {
    struct f641_appli *app = (struct f641_appli*)appli;
    struct f640_line *line = (struct f640_line*) stone->private;
    int k, ix;


    if (app->functions == 0) {
        line->flaged = 0;
        if (line->frame % app->recording_perst == 0) line->flaged = 1;
    } else if (app->functions == 1) {
        //    pthread_mutex_lock(&app->process->grid->mutex_coefs);
        line->flaged = 0;
        for(k = 0 ; k < app->process->grid->num ; k++) {
            if ( app->process->grid->grid_ratio[(k << 1) + 1] * line->grid_values[k] > app->process->grid->grid_ratio[k << 1] * app->threshold) {
                int i, j;
                ix = app->process->grid->grid_index[k];
                //f640_draw_rect(line->yuv, ix, line->grid->wlen, line->grid->hlen);
                f640_draw_rect(line->rgb, ix, line->grid->wlen, line->grid->hlen);
                line->flaged = 1;
            }
        }
        //    pthread_mutex_unlock(&app->process->grid->mutex_coefs);
    } else if (app->functions == 2) {
        line->flaged = 0;
        if (line->frame % (app->recording_perst * app->frames_pers) == 0) line->flaged = 1;
    }

    if (app->functions == 0) {
        ix = f640_draw_number(line->rgb, app->process->broadcast_width - 5, app->process->broadcast_height - 5, line->tv00.tv_sec - app->process->tv0.tv_sec);
        ix = f640_draw_number(line->rgb, ix - 20, app->process->broadcast_height - 5, line->frame);
        ix = f640_draw_number(line->rgb, ix - 20, app->process->broadcast_height - 5, *(line->grid_max));
    } else {
        ix = f640_draw_number(line->rgb, line->grid->width - 5, line->grid->height - 5, line->tv00.tv_sec - app->process->tv0.tv_sec);
        ix = f640_draw_number(line->rgb, ix - 20, line->grid->height - 5, line->frame);
        ix = f640_draw_number(line->rgb, ix - 20, line->grid->height - 5, *(line->grid_max));
    }


    return 0;
}


//
void f641_attrib_tagging(struct f641_thread_operations *ops) {
        ops->init = NULL;
        ops->updt = NULL;
        ops->exec = f641_exec_tagging;
        ops->free = NULL;
}

/******************************
 *      TAGGING THREAD
 ******************************/
static int f641_exec_saving(void *appli, void *ressources, struct f640_stone *stone) {
    struct f641_appli *app = (struct f641_appli*)appli;
    struct f640_line *line = (struct f640_line*) stone->private;
    int r;
    static long nb = 1;
    static long size = 0;
    struct timeval tv;

    gettimeofday(&tv, NULL);

    if ( (app->functions == 2) && ( (line->frame % (app->recording_perst * app->frames_pers)) == 0) ) {
        char fname[128];
        FILE *filp;
        if ( (size >> 10) < 0) {
            sprintf(fname, "/work/test/snap/sky7-%ld.raw", nb++);
        } else if ( (size >> 10) < 15000000L) {
            sprintf(fname, "/media/test/snap/sky7-%ld.raw", nb++);
        } else {
            sprintf(fname, "/mnt/test/snap/sky7-%ld.raw", nb++);
        }
        filp = fopen(fname, "wb");
        if (!filp) {
            printf("\nPB to open %s, returning", fname);
            return 0;
        }
        r = fwrite(app->process->grid2->sky, 1, 2 * app->size * sizeof(uint16_t), filp);
        fwrite(&tv.tv_sec, 1, sizeof(long), filp);
        fflush(filp);
        fclose(filp);
        size += 2 * app->size * sizeof(uint16_t) + 8;
        printf("\nSaved %d bytes (%.1f Mo)", r, 1. * size / (1024*1024));
    }

    return 0;
}

//
void f641_attrib_saving(struct f641_thread_operations *ops) {
        ops->init = NULL;
        ops->updt = NULL;
        ops->exec = f641_exec_saving;
        ops->free = NULL;
}

/******************************
 *      BROADCAST THREAD
 ******************************/
struct f641_broadcast_ressources {
    int fd_fb;
    int fd_sky;
};

static void* f641_init_broadcasting(void *appli) {
    int r;
    struct f641_appli *app = (struct f641_appli*)appli;
    struct f641_broadcast_ressources *res = calloc(1, sizeof(struct f641_broadcast_ressources));

    res->fd_fb = open("/dev/fb0", O_WRONLY);
    res->fd_sky = open("/dev/t030/t030-15", O_WRONLY);

    return res;
}

static int f641_exec_broadcasting(void *appli, void *ressources, struct f640_stone *stone) {
    struct f641_appli *app = (struct f641_appli*)appli;
    struct f641_broadcast_ressources *res = ressources;
    struct f640_line *line = (struct f640_line*) stone->private;

    gettimeofday(&line->tvb0, NULL);

    // FB
    if (res && res->fd_fb > 0 && app->functions == 0) {
        lseek(res->fd_fb, 0, SEEK_SET);
//        write(res->fd_fb, line->rgb->data, line->rgb->data_size);
        write(res->fd_fb, line->rgb->data, 4 * app->width * app->process->broadcast_height);
    } else if (app->functions == 1) {
        // Grid
        if (app->fd_grid > 0) {
            write(app->fd_grid, line->width, sizeof(long) * (6 + line->grid->num));
        }
        // Edge
        if (app->fd_edge > 0) {
            //write(app->fd_edge, line->gry->data, line->gry->data_size);
            write(app->fd_edge, app->process->grid2->skyDif, app->size);
        }
        // RGB
        write(app->fd_stream, line->rgb->data, line->rgb->data_size);

        if (res->fd_sky > 0) {
            write(res->fd_sky, app->process->grid2->sky, app->size * sizeof(long));
        }

        // GRID 10
        if (app->fd_grid10 > 0) {
            write(app->fd_grid10, app->process->grid2->grid_ratio_10, 4 * sizeof(int16_t) * line->grid->num);
        }

        // GRID 20
        if (app->fd_grid20 > 0) {
            write(app->fd_grid20, app->process->grid2->grid_ratio_20, 4 * sizeof(int16_t) * line->grid->num);
        }

        // GRID 30
        if (app->fd_grid30 > 0) {
            write(app->fd_grid30, app->process->grid2->grid_ratio_30, 4 * sizeof(int16_t) * line->grid->num);
        }
    } else if (app->functions == 2 && (line->frame % (app->recording_perst * app->frames_pers) == 0)) {
        write(app->fd_stream, app->process->grid2->sky, 2 * app->size * sizeof(uint16_t));
    }

    gettimeofday(&line->tvb1, NULL);

    return 0;
}

//
static void f641_free_broadcasting(void *appli, void* ressources) {
    struct f641_broadcast_ressources *res = (struct f641_broadcast_ressources*)ressources;
    if (res) {
        close(res->fd_fb);
        free(res);
    }
}


//
void f641_attrib_broadcasting(struct f641_thread_operations *ops) {
        ops->init = f641_init_broadcasting;
        ops->updt = NULL;
        ops->exec = f641_exec_broadcasting;
        ops->free = f641_free_broadcasting;
}

/******************************
 *      GRID THREAD
 ******************************/
static int f641_exec_grid(void *appli, void *ressources, struct f640_stone *stone) {
    struct f641_appli *app = (struct f641_appli*)appli;
    struct f640_line *line = (struct f640_line*) stone->private;

    gettimeofday(&line->tvg0, NULL);
    /*
     *  SUM GRID 2
     */
    // Level 10
    int AVG = 0, MIN = 32700, MAX = -32700;
    int c, i, p;
    int del = 0, avg = 0, min = 32767, max = -32767;
    int32_t *v;
    int16_t *a = app->process->grid2->grid_ratio_10;
    for(c = 0 ; c < app->process->grid->num ; c++) {
        max = -32767;
        min = 0x7FFE;
        avg = 0;
        v = app->process->grid2->grid_ratio_0 + app->process->grid->num * (line->frame & app->process->grid2->mask0) + c;
        for(i = 0 ; i < app->process->grid2->nb01 ; i++) {
            if (*v > max) max = *v;
            if (*v < min) min = *v;
            avg += *v;

            v -= app->process->grid->num;
            if ( v < app->process->grid2->grid_ratio_0 ) v = app->process->grid2->grid_ratio_0 + app->process->grid->num * app->process->grid2->mask0 + c;
        }
        avg /= app->process->grid2->nb01;
        del = 0;
        v = app->process->grid2->grid_ratio_0 + app->process->grid->num * (line->frame & app->process->grid2->mask0) + c;
        for(i = 0 ; i < app->process->grid2->nb01 ; i++) {
            del += (*v - avg) * (*v -avg);
            v -= app->process->grid->num;
            if ( v < app->process->grid2->grid_ratio_0 ) v = app->process->grid2->grid_ratio_0 + app->process->grid->num * app->process->grid2->mask0 + c;
        }
        del = sqrt(del) / app->process->grid2->nb01;
        //
        *(a++) = del;
        *(a++) = max;
        *(a++) = min;
        *(a++) = avg;

        AVG += avg;
        if (MIN > min) MIN = min;
        if (MAX < max) MAX = max;
    }
    AVG /= app->process->grid->num;
    if (DEBUG) printf(F640_RESET F640_FG_RED "\tLOOP 10 => cells I0 [ %d < %d < %d ]\n" F640_RESET, MIN, AVG, MAX);

    // Histo
    if ( (line->frame & app->process->grid2->mask01) == app->process->grid2->mask01 ) {
        memcpy(app->process->grid2->grid_ratio_1 + 4 * app->process->grid->num * app->process->grid2->index1, app->process->grid2->grid_ratio_10, 4 * app->process->grid->num * sizeof(int16_t));
        if (DEBUG) printf(F640_RESET F640_FG_RED "\tLOOP 0 => cells I %d [ %d < %d < %d ]\n" F640_RESET, app->process->grid2->index1, MIN, AVG, MAX);

        // Loop 1
        AVG = 0;
        MIN = 0x7FFE;
        MAX = -32767;
        int16_t *w;
        a = app->process->grid2->grid_ratio_20;
        avg = 0;
        for(c = 0 ; c < app->process->grid->num ; c++) {
            max = -32767;
            min = 0x7FFE;
            avg = 0;
            w = app->process->grid2->grid_ratio_1 + (c << 2);
            for(i = 0 ; i < app->process->grid2->nb1 ; i++) {
                if (w[1] > max) max = w[1];
                if (w[2] < min) min = w[2];
                avg += w[3];          // Average of average

                w += (app->process->grid->num << 2);
            }
            avg /= app->process->grid2->nb1;
            del = 0;
            w = app->process->grid2->grid_ratio_1 + (c << 2);
            for(i = 0 ; i < app->process->grid2->nb1 ; i++) {
                del += (w[3] - avg) * (w[3] - avg);          // Average of average
                w += (app->process->grid->num << 2);
            }
            del = sqrt(del) / app->process->grid2->nb1;
            //
            *(a++) = del;
            *(a++) = max;
            *(a++) = min;
            *(a++) = avg;

            AVG += avg;
            if (MIN > min) MIN = min;
            if (MAX < max) MAX = max;
        }
        AVG /= app->process->grid->num;

        app->process->grid2->index1 = (app->process->grid2->index1 + 1) & app->process->grid2->mask1;
        if (app->process->grid2->index1 == 0) {
            memcpy(app->process->grid2->grid_ratio_2 + 4 * app->process->grid->num * app->process->grid2->index2, app->process->grid2->grid_ratio_20, 4 * app->process->grid->num * sizeof(int16_t));

            // Loop 2
            AVG = 0;
            MIN = 0x7FFE;
            MAX = -32767;
            a = app->process->grid2->grid_ratio_30;
            for(c = 0 ; c < app->process->grid->num ; c++) {
                max = -32767;
                min = 0x7FFE;
                avg = 0;
                w = app->process->grid2->grid_ratio_2 + 4 * c;
                for(i = 0 ; i < app->process->grid2->nb2 ; i++) {
                    if (w[1] > max) max = w[1];
                    if (w[2] < min) min = w[2];
                    avg += w[3];          // Average of average

                    w += app->process->grid->num << 2;
                }
                avg /= app->process->grid2->nb2;
                del = 0;
                w = app->process->grid2->grid_ratio_2 + 4 * c;
                for(i = 0 ; i < app->process->grid2->nb2 ; i++) {
                    del += (w[3] - avg) * (w[3] - avg);
                    w += app->process->grid->num << 2;
                }
                del = sqrt(del) / app->process->grid2->nb2;
                //
                *(a++) = del;
                *(a++) = max;
                *(a++) = min;
                *(a++) = avg;

                AVG += avg;
                if (MIN > min) MIN = min;
                if (MAX < max) MAX = max;
            }
            AVG /= app->process->grid->num;

            app->process->grid2->index2 = (app->process->grid2->index2 + 1) & app->process->grid2->mask2;
//            printf(F640_RESET F640_BOLD F640_FG_RED "\t\tLoop I => cells II %d [ %d < %d < %d ]\n" F640_RESET, app->process->grid2->index2, MIN, AVG, MAX);
            if (app->process->grid2->index2 == 0) {
                memcpy(app->process->grid2->grid_ratio_3 + 4 * app->process->grid->num * app->process->grid2->index3, app->process->grid2->grid_ratio_30, 4 * app->process->grid->num * sizeof(int16_t));

                // Loop 3
                app->process->grid2->index3 = (app->process->grid2->index3 + 1) & app->process->grid2->mask3;
//                printf(F640_RESET F640_BOLD F640_FG_RED "\t\t\tLoop II => cells III %d add [ %d < %d < %d ]\n" F640_RESET, app->process->grid2->index3, MIN, AVG, MAX);
            }
        }
    }

    gettimeofday(&line->tvg1, NULL);

    return 0;
}
//
void f641_attrib_griding(struct f641_thread_operations *ops) {
        ops->init = NULL;
        ops->updt = NULL;
        ops->exec = f641_exec_grid;
        ops->free = NULL;
}


/******************************
 *      LOGGING THREAD
 ******************************/
struct f641_logging_ressources {
    long size_in;
    long size_out;
    double t;
    double t0;
    double td;
    double t1;
    double t2;
    double te;
    double tg;
    double tb;
    double t3;
    double t4;
    struct timeval tv1;
    struct timeval tv2;
    struct timeval tv3;
    struct timeval tvs;
    struct timeval tvp;
    struct timeval tva;
    struct timespec ts1;
    struct timeval tsys;
    struct timeval tcod;
    struct tm tml;
    time_t time70;
    clock_t times1;
    clock_t times2;
    unsigned long jiffs, ms;
    int show_freq;

    long rms;
    long grid_min_value;
    long grid_max_value;
    int recorded_one;
    long recorded_frames;
};
//
static void* f641_init_logging(void *appli) {
    struct f641_appli *app = (struct f641_appli*)appli;
    struct f641_logging_ressources *res = calloc(1, sizeof(struct f641_logging_ressources));
    res->show_freq = app->frames_pers;
    return res;
}
//
static void f641_free_logging(void *appli, void* ressources) {
    if (ressources) free(ressources);
}
//
static int f641_exec_logging(void *appli, void *ressources, struct f640_stone *stone) {
    struct f641_appli *app = (struct f641_appli*)appli;
    struct f641_logging_ressources *res = (struct f641_logging_ressources*) ressources;
    struct f640_line *line = (struct f640_line*) stone->private;
    struct tm tm1;
    struct timeval tmp;

    gettimeofday(&line->tv40, NULL);

    // Agreg
    res->size_in += line->buf.bytesused;
    if (line->flaged) {
        res->size_out += line->buf.bytesused;
        res->recorded_frames++;
        res->recorded_one++;
    }
    res->rms += *(line->rms);
    if (res->grid_min_value > *(line->grid_min)) res->grid_min_value = *(line->grid_min);
    if (res->grid_max_value < *(line->grid_max)) res->grid_max_value = *(line->grid_max);

    // Times
    res->t  += f640_duration (line->tv40, line->tv10);
    res->td += f640_duration (line->tvd1, line->tvd0);   // decode
//    timersub(&line->tv11, &line->tv10, &line->tv11); timeradd(&res->tv1, &line->tv11, &res->tv1); @@@buggy
    res->t1 += f640_duration (line->tv11, line->tv10);   // watch
    res->te += f640_duration (line->tve1, line->tve0);   // edge
//    timersub(&line->tv21, &line->tv20, &line->tv21); timeradd(&res->tv2, &line->tv21, &res->tv2);
    res->t2 += f640_duration (line->tv21, line->tv20);   // convert
    res->tg += f640_duration (line->tvg1, line->tvg0);   // grid
    res->tb += f640_duration (line->tvb1, line->tvb0);   // broadc
//    timersub(&line->tv31, &line->tv30, &line->tv31); timeradd(&res->tv3, &line->tv31, &res->tv3);
    res->t3 += f640_duration (line->tv31, line->tv30);   // record
    res->t4 += f640_duration (line->tvd1, line->tvd0)
       +  f640_duration (line->tv11, line->tv10)
       +  f640_duration (line->tve1, line->tve0)
       +  f640_duration (line->tv21, line->tv20)
       +  f640_duration (line->tv31, line->tv30);

    timersub(&line->tv00, &line->buf.timestamp, &tmp);
    timeradd(&res->tsys, &tmp, &res->tsys);

    timersub(&line->tv40, &line->tv00, &tmp);
    timeradd(&res->tcod, &tmp, &res->tcod);


//    printf("Line[%d, %lu/%ld] %lX : td = %.3fs , t1 = %.3fs , te = %.3fs , t2 = %.3fs , t3 = %.3fs , tt = %.3fs\n"
//            , line->index, line->frame, stone->frame, stone->status, res->td, res->t1, res->te, res->t2, res->t3, res->t4);
//    printf("-------------------------------------------------------------\n");


    if (DEBUG) printf("\t\t\t\t\t\t\t\tRELEASE : dequeue %d, frame %lu\n", line->index, line->frame);
    if (line->frame % res->show_freq == 0) {
        gettimeofday(&res->tv2, NULL);
        localtime_r(&res->tv2.tv_sec, &tm1);
        timersub(&line->tv00, &line->buf.timestamp, &res->tvs);
        timersub(&line->tv40, &line->tv00, &res->tvp);
        timersub(&res->tv2, &line->buf.timestamp, &res->tva);
        res->times2 = clock();// / CLOCKS_PER_SECOND ~ 1 000 000;
        timersub(&res->tv2, &res->tv1, &res->tv3);
//            time(&time70);
//            jiffs = clock_t_to_jiffies(times);
//            ms = jiffies_to_msecs(jiffs);


        if (app->functions == 0) {
            int rows, cols;
            getmaxyx(stdscr, rows, cols);     /* get the number of rows and columns */
            move(rows - 5, 0);

            printw("%02d:%02d:%02d  %3.2f - %3.1fHz - %3.2fMo/s"
                    , tm1.tm_hour, tm1.tm_min, tm1.tm_sec
                    , 1. * (res->times2 - res->times1) / (1000000 * res->tv3.tv_sec + res->tv3.tv_usec)
                    , 1. * (res->show_freq * 1000000) / (1000000 * res->tv3.tv_sec + res->tv3.tv_usec)
                    , 1000000. * res->size_in / (1024 * 1024 * (1000000 * res->tv3.tv_sec + res->tv3.tv_usec))
            );

            move(rows - 4, 0);
            if ( res->recorded_one ) {
                printw("dec %2.0f "
                        "con %2.0f  rec %2.0f %2.0f +%2ld"
                        " (%.1fs %.1fMo) : %ldms + %ldms"
                        , 1000*res->td / res->show_freq                                                 // t decode
                        , 1000*res->t2 / res->show_freq                                                 // t converse
                        , 1000*res->tb / res->show_freq                                                 // t broadcast
                        , 1000*res->t3 / res->show_freq                                                 // t record
                        , res->recorded_frames
                        , 1. * app->process->recorded_frames / app->frames_pers                         // recorded time
                        , 1. * res->size_out / (1024 * 1024)                                            // recorded size
                        , (1000000 * res->tsys.tv_sec + res->tsys.tv_usec) / (1000 * res->show_freq)    // t sys
                        , (1000000 * res->tcod.tv_sec + res->tcod.tv_usec) / (1000 * res->show_freq)    // t cod
                );
            } else {
                printw("dec %2.0f "
                        "con %2.0f  rec %2.0f %2.0f +%2ld"
                        " (%.1fs %.1fMo) : %ldms + %ldms"
                        , 1000*res->td / res->show_freq                                                 // t decode
                        , 1000*res->t2 / res->show_freq                                                 // t converse
                        , 1000*res->tb / res->show_freq                                                 // t broadcast
                        , 1000*res->t3 / res->show_freq                                                 // t record
                        , res->recorded_frames
                        , 1. * app->process->recorded_frames / app->frames_pers                         // recorded time
                        , 1. * res->size_out / (1024 * 1024)                                            // recorded size
                        , (1000000 * res->tsys.tv_sec + res->tsys.tv_usec) / (1000 * res->show_freq)    // t sys
                        , (1000000 * res->tcod.tv_sec + res->tcod.tv_usec) / (1000 * res->show_freq)    // t cod
                );
            }
            refresh();                      /* Print it on to the real screen */
        } else {
            printf("\n");
            printf("%02d:%02d:%02d %3lu%|%3.1fHz %3.2fMo/s||"
                    , tm1.tm_hour, tm1.tm_min, tm1.tm_sec
                    , 100 * (res->times2 - res->times1) / (1000000 * res->tv3.tv_sec + res->tv3.tv_usec)
                    , 1. * (res->show_freq * 1000000) / (1000000 * res->tv3.tv_sec + res->tv3.tv_usec)
                    , 1000000. * res->size_in / (1024 * 1024 * (1000000 * res->tv3.tv_sec + res->tv3.tv_usec))
            );

            if ( res->recorded_one ) {
                printf("%2d|¤de %2.0f|%2d|¤wa %2.0f|%2ld " F640_RESET F640_BOLD F640_FG_RED "%4ld" F640_RESET
                        "|%2d|¤ed %2.0f %2.0f|%2d|¤cv %2.0f|%2d|¤rc %2.0f %2.0f " F640_RESET F640_BOLD F640_FG_RED "+%2ld" F640_RESET
                        "|%2d|%2d|%.1fs %.1fMo|%ld %ld"
                        , /*snaped*/0,    1000*res->td / res->show_freq//res->t4
                        , /*decoded*/0 ,  1000*res->t1 / res->show_freq, res->rms / res->show_freq, res->grid_max_value
                        , /*watched*/0,   1000*res->te / res->show_freq, 1000*res->tg / res->show_freq
                        , /*edged*/0,     1000*res->t2 / res->show_freq
                        , /*converted*/0, 1000*res->tb / res->show_freq, 1000*res->t3 / res->show_freq, res->recorded_frames
                        , /*recorded*/0
                        , /*released*/0
                        , 1. * app->process->recorded_frames / app->frames_pers
                        , 1. * res->size_out / (1024 * 1024)
                        , (1000000 * res->tsys.tv_sec + res->tsys.tv_usec) / (1000 * res->show_freq)
                        , (1000000 * res->tcod.tv_sec + res->tcod.tv_usec) / (1000 * res->show_freq)
                );
            } else {
                printf("%2d|¤de %2.0f|%2d|¤wa %2.0f|%2ld %4ld"
                        "|%2d|¤ed %2.0f %2.0f|%2d|¤cv %2.0f|%2d|¤rc %2.0f %2.0f +%2ld"
                        "|%2d|%2d|%.1fs %.1fMo|%ld %ld"
                        , /*snaped*/0,    1000*res->td / res->show_freq//res->t4
                        , /*decoded*/0 ,  1000*res->t1 / res->show_freq, res->rms / res->show_freq, res->grid_max_value
                        , /*watched*/0,   1000*res->te / res->show_freq, 1000*res->tg / res->show_freq
                        , /*edged*/0,     1000*res->t2 / res->show_freq
                        , /*converted*/0, 1000*res->tb / res->show_freq, 1000*res->t3 / res->show_freq, res->recorded_frames
                        , /*recorded*/0
                        , /*released*/0
                        , 1. * app->process->recorded_frames / app->frames_pers
                        , 1. * res->size_out / (1024 * 1024)
                        , (1000000 * res->tsys.tv_sec + res->tsys.tv_usec) / (1000 * res->show_freq)
                        , (1000000 * res->tcod.tv_sec + res->tcod.tv_usec) / (1000 * res->show_freq)
                );
            }
        }

        res->size_in  = 0;
        res->t  = 0;
        res->td = 0;
        res->t1 = 0;
        res->te = 0;
        res->tg = 0;
        res->tb = 0;
        res->t2 = 0;
        res->t3 = 0;
        res->t4 = 0;
        res->rms = 0;
        res->grid_min_value = 0xFFFFFFFFFFL;
        res->grid_max_value = 0;
        res->recorded_frames = 0;
        res->recorded_one = 0;
        res->tv1.tv_sec = res->tv2.tv_sec; res->tv1.tv_usec = res->tv2.tv_usec;
        res->times1 = res->times2;
        timerclear(&res->tsys);
        timerclear(&res->tcod);
    }

    gettimeofday(&line->tv41, NULL);

    return 0;
}

void f641_attrib_logging(struct f641_thread_operations *ops) {
        ops->init = f641_init_logging;
        ops->updt = NULL;
        ops->exec = f641_exec_logging;
        ops->free = f641_free_logging;
}

/**********************************************
 *  COPY from f640
 */
struct f640_grid *f641_make_grid(int width, int height, int factor) {
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
struct f640_grid2* f641_make_grid2(struct f640_grid *grid) {
    int i;
    struct f640_grid2 *grid2 = calloc(1, sizeof(struct f640_grid2));
    if (!grid) {
        printf("ENOMEM in allocating grid2, returning\n");
        return NULL;
    }
    // Level 0
    grid2->nb0    = 32;
    grid2->shift0 = 5;
    grid2->mask0  = 31;
    grid2->grid_ratio_0 = calloc(grid2->nb0, grid->num * sizeof(int32_t));

    // Transition
    grid2->nb01    = 16;
    grid2->shift01 = 4;
    grid2->mask01  = 15;

    // Level 1
    grid2->nb1    = 8;
    grid2->shift1 = 3;
    grid2->mask1  = 7;
    grid2->grid_ratio_10 = calloc(1,          4 * grid->num * sizeof(int16_t));
    grid2->grid_ratio_1  = calloc(grid2->nb1, 4 * grid->num * sizeof(int16_t));

    // Level 2
    grid2->nb2    = 16;
    grid2->shift2 = 4;
    grid2->mask2  = 15;
    grid2->grid_ratio_20 = calloc(1,          4 * grid->num * sizeof(int16_t));
    grid2->grid_ratio_2  = calloc(grid2->nb2, 4 * grid->num * sizeof(int16_t));

    // Level 3
    grid2->nb3    = 32;
    grid2->shift3 = 5;
    grid2->mask3  = 31;
    grid2->grid_ratio_30 = calloc(1,          4 * grid->num * sizeof(int16_t));
    grid2->grid_ratio_3  = calloc(grid2->nb3, 4 * grid->num * sizeof(int16_t));

    // Synchro
    pthread_mutex_init(&grid2->mutex, NULL);
    grid2->index0 = 0;
    grid2->index1 = 0;
    grid2->index2 = 0;
    grid2->index3 = 0;

    //
    grid2->sky = calloc(grid->size, sizeof(long));

    //
    return grid2;
}

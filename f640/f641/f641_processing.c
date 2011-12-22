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
static int f641_exec_watch_422p(void *appli, void *ressources, struct f640_stone *stone) {
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

    return 0;
}

/******************************
 *      TAGGING THREAD
 ******************************/
static int f641_exec_tagging(void *appli, void *ressources, struct f640_stone *stone) {
    struct f641_appli *app = (struct f641_appli*)appli;
    struct f640_line *line = (struct f640_line*) stone->private;
    int k, ix;

//    pthread_mutex_lock(&app->process->grid->mutex_coefs);
    for(k = 0 ; k < app->process->grid->num ; k++) {
        if ( app->process->grid->grid_ratio[(k << 1) + 1] * line->grid_values[k] > app->process->grid->grid_ratio[k << 1] * line->grid_th) {
            int i, j;
            ix = app->process->grid->grid_index[k];
            //f640_draw_rect(line->yuv, ix, line->grid->wlen, line->grid->hlen);
            f640_draw_rect(line->rgb, ix, line->grid->wlen, line->grid->hlen);
            line->flaged = 1;
        }
    }
//    pthread_mutex_unlock(&app->process->grid->mutex_coefs);

    ix = f640_draw_number(line->rgb, line->grid->width - 5, line->grid->height - 5, line->tv00.tv_sec - app->process->tv0.tv_sec);
    ix = f640_draw_number(line->rgb, ix - 20, line->grid->height - 5, line->frame);
    ix = f640_draw_number(line->rgb, ix - 20, line->grid->height - 5, *(line->grid_max));


    return 0;
}

/******************************
 *      BROADCAST THREAD
 ******************************/
static int f641_exec_broadcasting(void *appli, void *ressources, struct f640_stone *stone) {
    struct f641_appli *app = (struct f641_appli*)appli;
    struct f640_line *line = (struct f640_line*) stone->private;


    // Grid
    if (app->fd_grid > 0) {
        write(app->fd_grid, line->width, sizeof(long) * (6 + line->grid->num));
    }
    // Edge
    if (app->fd_edge > 0) {
        write(app->fd_edge, line->gry->data, line->gry->data_size);
    }
    // RGB
    write(app->fd_stream, line->rgb->data, line->rgb->data_size);

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


    return 0;
}


/******************************
 *      GRID THREAD
 ******************************/
static int f641_exec_grid(void *appli, void *ressources, struct f640_stone *stone) {
    struct f641_appli *app = (struct f641_appli*)appli;
    struct f640_line *line = (struct f640_line*) stone->private;


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
            printf(F640_RESET F640_BOLD F640_FG_RED "\t\tLoop I => cells II %d [ %d < %d < %d ]\n" F640_RESET, app->process->grid2->index2, MIN, AVG, MAX);
            if (app->process->grid2->index2 == 0) {
                memcpy(app->process->grid2->grid_ratio_3 + 4 * app->process->grid->num * app->process->grid2->index3, app->process->grid2->grid_ratio_30, 4 * app->process->grid->num * sizeof(int16_t));

                // Loop 3
                app->process->grid2->index3 = (app->process->grid2->index3 + 1) & app->process->grid2->mask3;
                printf(F640_RESET F640_BOLD F640_FG_RED "\t\t\tLoop II => cells III %d add [ %d < %d < %d ]\n" F640_RESET, app->process->grid2->index3, MIN, AVG, MAX);
            }
        }
    }


    return 0;
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
    return grid2;
}

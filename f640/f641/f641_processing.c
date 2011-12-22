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


/*
 *      WATCHING 422P THREAD
 */

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

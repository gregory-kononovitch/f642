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
    for(i = 0 ; i < grid->size ; i++) index[i] = ( i % grid->width ) / grid->wlen + ( ( i / grid->width ) / grid->hlen ) * grid->cols; // @@@ buggy

    return grid;
}


/*
 *
 */
static struct f640_line *f640_lineup;

int f640_make_lineup(v4l2_buffer_t *buffers, int nbuffers, struct f640_grid *grid, struct output_stream *stream) {
    int i;
    f640_lineup = calloc(nbuffers, sizeof(struct f640_line));
    if (!f640_lineup) {
        printf("ENOMEM allocating lineup, returning\n");
        return NULL;
    }

    for(i = 0 ; i < nbuffers ; i++) {
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
            return -ENOMEM;
        }
        f640_lineup[i].grid_th = 70;

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
            return -ENOMEM;
        }

        f640_lineup->stream         = stream;
    }
    return 0;
}


/*
 *
 */
int f640_enqueue(int v4l2_index) {
    return -1;
}

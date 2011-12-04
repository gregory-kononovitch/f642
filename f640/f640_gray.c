/*
 * file    : f640_gray.c
 * project : f640
 *
 * Created on: Dec 3, 2011
 * Author and copyright (C) 2011 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */



#include "f640_graphics.h"


/*
 * dir : 1 -> left - right | up - down
 * @@@ to manage : Checks about 0 <= index < size & w > 0
 */
static int f640_draw_hline(struct f640_image *image, int index, int w, int dir) {
    image->data[index] = 0;
    while( --w ) {
        index += dir;
        image->data[index] = 0;
    }
    return index;
}
static int f640_draw_vline(struct f640_image *image, int index, int h, int dir) {
    image->data[index] = 0;
    while( --h ) {
        index += dir * image->width;
        image->data[index] = 0;
    }
    return index;
}


/*
 *
 */
struct f640_image *f640_create_gry_image(int width, int height) {
    struct f640_image* im = calloc(1, sizeof(struct f640_image));
    if (im) {
        im->data = calloc(1, width * height);
        if (im->data) {
            im->width   = width;
            im->height  = height;
            im->size    = width * height;

            im->data_size = 1 * width * height;

            im->gops.f640_draw_hline = f640_draw_hline;
            im->gops.f640_draw_vline = f640_draw_vline;

            im->glyphs.width  = 9;
            im->glyphs.height = 12;

            return im;
        }
        printf("ENOMEM for image data\n");
        free(im);
        return NULL;
    }
    printf("ENOMEM for image\n");
    return NULL;
}

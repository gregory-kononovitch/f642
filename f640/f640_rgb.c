/*
 * file    : f640_rgb.c
 * project : f640
 *
 * Created on: Dec 3, 2011
 * Author and copyright (C) 2011 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */



#include "f640_graphics.h"


struct pixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};
typedef struct pixel pixel;

struct f640_rgb_image {
    F640_IMAGE()

    //
    pixel       *pixels;
};

/*
 * dir : 1 -> left - right | up - down
 * @@@ to manage : Checks about 0 <= index < size & w > 0
 */
static int f640_draw_hline(struct f640_image *image, int index, int w, int dir) {
    //memset(&image->pixels[index], 0, 3);
    image->data[3 * index + 0] = 0;
    image->data[3 * index + 1] = 0;
    image->data[3 * index + 2] = 0xFF;
    while( --w ) {
        index += dir;
        image->data[3 * index + 0] = 0;
        image->data[3 * index + 1] = 0;
        image->data[3 * index + 2] = 0xFF;
    }
    return index;
}
static int f640_draw_vline(struct f640_image *image, int index, int h, int dir) {
    image->data[3 * index + 0] = 0;
    image->data[3 * index + 1] = 0xFF;
    image->data[3 * index + 2] = 0;
    while( --h ) {
        index += dir * image->width;
        image->data[3 * index + 0] = 0;
        image->data[3 * index + 1] = 0xFF;
        image->data[3 * index + 2] = 0;
    }
    return index;
}


/*
 *
 */
struct f640_image *f640_create_rgb_image(int width, int height) {
    struct f640_rgb_image* im = (struct f640_rgb_image*)calloc(1, sizeof(struct f640_rgb_image));
    if (im) {
        im->data = (uint8_t*)calloc(3, width * height);
        im->pixels = (pixel*)im->data;
        if (im->data) {
            im->width   = width;
            im->height  = height;
            im->size    = width * height;

            im->data_size = 3 * width * height;

            im->gops.f640_draw_hline = f640_draw_hline;
            im->gops.f640_draw_vline = f640_draw_vline;

            im->glyphs.width  = 9;
            im->glyphs.height = 12;

            return (struct f640_image*) im;
        }
        printf("ENOMEM for image data\n");
        free(im);
        return NULL;
    }
    printf("ENOMEM for image\n");
    return NULL;
}

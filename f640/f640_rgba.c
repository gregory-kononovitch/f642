/*
 * file    : f640_rgba.c
 * project : f640
 *
 * Created on: Dec 5, 2011
 * Author and copyright (C) 2011 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */



#include "f640_graphics.h"


union pixel {
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };
    int rgba;
};
typedef union pixel pixel;

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
    //memset(&image->pixels[index], 0, 4);
    image->data[(index << 2) + 0] = 0xFF;   // Alpha
    image->data[(index << 2) + 1] = 0;      // Blue
    image->data[(index << 2) + 2] = 0;      // Green
    image->data[(index << 2) + 3] = 0;      // Red
    while( --w ) {
        index += dir;
        image->data[(index << 2) + 0] = 0xFF;
        image->data[(index << 2) + 1] = 0;
        image->data[(index << 2) + 2] = 0;
        image->data[(index << 2) + 3] = 0;
    }
    return index;
}
static int f640_draw_vline(struct f640_image *image, int index, int h, int dir) {
    image->data[(index << 2) + 0] = 0xFF;
    image->data[(index << 2) + 1] = 0;
    image->data[(index << 2) + 2] = 0;
    image->data[(index << 2) + 3] = 0;
    while( --h ) {
        index += dir * image->width;
        image->data[(index << 2) + 0] = 0xFF;
        image->data[(index << 2) + 1] = 0;
        image->data[(index << 2) + 2] = 0;
        image->data[(index << 2) + 3] = 0;
    }
    return index;
}


/*
 *
 */
struct f640_image *f640_create_rgba_image(int width, int height) {
    struct f640_rgb_image* im = calloc(1, sizeof(struct f640_rgb_image));
    if (im) {
        im->data = calloc(4, width * height);
        im->pixels = (pixel*)im->data;
        if (im->data) {
            im->width   = width;
            im->height  = height;
            im->size    = width * height;

            im->data_size = 4 * width * height;

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

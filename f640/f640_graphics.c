/*
 * file    : f640_graphics.c
 * project : f640
 *
 * Created on: Dec 3, 2011
 * Author and copyright (C) 2011 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#include "f640_graphics.h"

/*
 *
 */
int f640_draw_rect(struct f640_image *image, int index, int distx, int disty) {
    index = image->gops.f640_draw_hline(image, index, distx,  1);
    index = image->gops.f640_draw_vline(image, index, disty,  1);
    index = image->gops.f640_draw_hline(image, index, distx, -1);
    index = image->gops.f640_draw_vline(image, index, disty, -1);
    return index;
}


int f640_draw_digit(struct f640_image *image, int x, int y, int value) {
    x = f640_get_index(x, y, image->width);
    switch(value) {
        case 0 :
            x = image->gops.f640_draw_hline(image, x, image->glyphs.width,  1);
            x = image->gops.f640_draw_vline(image, x, image->glyphs.height, -1);
            x = image->gops.f640_draw_hline(image, x, image->glyphs.width, -1);
            x = image->gops.f640_draw_vline(image, x, image->glyphs.height,  1);
            break;
        case 1 :
            x += image->glyphs.width/2;
            x = image->gops.f640_draw_vline(image, x, image->glyphs.height, -1);
            break;
        case 2 :
                image->gops.f640_draw_hline(image, x, image->glyphs.width,  1);
            x = image->gops.f640_draw_vline(image, x, image->glyphs.height/2, -1);
            x = image->gops.f640_draw_hline(image, x, image->glyphs.width,  1);
            x = image->gops.f640_draw_vline(image, x, image->glyphs.height/2, -1);
            x = image->gops.f640_draw_hline(image, x, image->glyphs.width, -1);
            break;
        case 3 :
            x = image->gops.f640_draw_hline(image, x, image->glyphs.width,  1);
            x = image->gops.f640_draw_vline(image, x, image->glyphs.height/2, -1);
                image->gops.f640_draw_hline(image, x, image->glyphs.width,  -1);
            x = image->gops.f640_draw_vline(image, x, image->glyphs.height/2, -1);
            x = image->gops.f640_draw_hline(image, x, image->glyphs.width, -1);
            break;
        case 4 :
            x += image->glyphs.width/2;
            x = image->gops.f640_draw_vline(image, x, image->glyphs.height/3, -1);
                image->gops.f640_draw_hline(image, x, image->glyphs.width/2,  1);
                image->gops.f640_draw_vline(image, x, image->glyphs.height/3, -1);
            x = image->gops.f640_draw_hline(image, x, image->glyphs.width/2, -1);
            x = image->gops.f640_draw_vline(image, x, 2*image->glyphs.height/3, -1);
            break;
        case 5 :
            x = image->gops.f640_draw_hline(image, x, image->glyphs.width,  1);
            x = image->gops.f640_draw_vline(image, x, image->glyphs.height/2, -1);
            x = image->gops.f640_draw_hline(image, x, image->glyphs.width, -1);
            x = image->gops.f640_draw_vline(image, x, image->glyphs.height/2, -1);
            x = image->gops.f640_draw_hline(image, x, image->glyphs.width,  1);
            break;
        case 6 :
                image->gops.f640_draw_vline(image, x, image->glyphs.height/2, -1);
            x = image->gops.f640_draw_hline(image, x, image->glyphs.width,  1);
            x = image->gops.f640_draw_vline(image, x, image->glyphs.height/2, -1);
            x = image->gops.f640_draw_hline(image, x, image->glyphs.width,  -1);
            x = image->gops.f640_draw_vline(image, x, image->glyphs.height/2, -1);
            x = image->gops.f640_draw_hline(image, x, image->glyphs.width,   1);
            break;
        case 7 :
            x += image->glyphs.width;
            x = image->gops.f640_draw_vline(image, x, image->glyphs.height/2, -1);
                image->gops.f640_draw_hline(image, x, image->glyphs.width/2, -1);
            x = image->gops.f640_draw_vline(image, x, image->glyphs.height/2, -1);
            x = image->gops.f640_draw_hline(image, x, image->glyphs.width, -1);
            break;
        case 8 :
                image->gops.f640_draw_vline(image, x, image->glyphs.height, -1);
            x = image->gops.f640_draw_hline(image, x, image->glyphs.width,  1);
            x = image->gops.f640_draw_vline(image, x, image->glyphs.height, -1);
            x = image->gops.f640_draw_hline(image, x, image->glyphs.width, -1);
            x = image->gops.f640_draw_vline(image, x, image->glyphs.height/2,1);
            x = image->gops.f640_draw_hline(image, x, image->glyphs.width,  1);
            break;
        case 9 :
            x = image->gops.f640_draw_hline(image, x, image->glyphs.width,  1);
            x = image->gops.f640_draw_vline(image, x, image->glyphs.height, -1);
            x = image->gops.f640_draw_hline(image, x, image->glyphs.width, -1);
            x = image->gops.f640_draw_vline(image, x, image->glyphs.height/2,1);
            x = image->gops.f640_draw_hline(image, x, image->glyphs.width,  1);
            break;
    }
    return x;
}

int f640_draw_number(struct f640_image *image, int x, int y, uint64_t value) {
    if (value) {
        while (value) {
            x -= image->glyphs.width + 2;
            f640_draw_digit(image, x, y, value % 10);
            value /= 10;
        }
    } else {
        x -= image->glyphs.width + 2;
        f640_draw_digit(image, x, y, 0);
    }
    return x;
}



/*
 *
 */
void f640_yuv_to_rgb(struct f640_image *yuv, int index, struct f640_image *rgb) {
    int r, g, b;

    if (!index) {
        yuv->ptr = yuv->data + 2;
        rgb->ptr = rgb->data + 3;
    } else if ( index & 0x02 ) {
//        r = *(yuv->ptr) + 1.370705 * (*(yuv->ptr - 1)-128);
//        g = *(yuv->ptr) - 0.698001 * (*(yuv->ptr - 1)-128) - 0.337633 * (*(yuv->ptr + 1)-128);
//        b = *(yuv->ptr) + 1.732446 * (*(yuv->ptr + 1)-128);
//        yuv->ptr += 2;

        r = (*(yuv->ptr) << 8) + 351 * (*(yuv->ptr - 1)-128);
        g = (*(yuv->ptr) << 8) - 179 * (*(yuv->ptr - 1)-128) - 86 * (*(yuv->ptr + 1)-128);
        b = (*(yuv->ptr) << 8) + 444 * (*(yuv->ptr + 1)-128);
        yuv->ptr += 2;

        r = r >> 8;
        g = g >> 8;
        b = b >> 8;

//        if(r > 255) r = 255;
//        if(g > 255) g = 255;
//        if(b > 255) b = 255;
//
//        if(r < 0) r = 0;
//        if(g < 0) g = 0;
//        if(b < 0) b = 0;

        *(rgb->ptr++) = r;
        *(rgb->ptr++) = g;
        *(rgb->ptr++) = b;
    } else {
//        r = *(yuv->ptr) + 1.370705 * (*(yuv->ptr + 1)-128);
//        g = *(yuv->ptr) - 0.698001 * (*(yuv->ptr + 1)-128) - 0.337633 * (*(yuv->ptr - 1)-128);
//        b = *(yuv->ptr) + 1.732446 * (*(yuv->ptr - 1)-128);
//        yuv->ptr += 2;

        r = (*(yuv->ptr) << 8) + 351 * (*(yuv->ptr + 1)-128);
        g = (*(yuv->ptr) << 8) - 179 * (*(yuv->ptr + 1)-128) - 86 * (*(yuv->ptr - 1)-128);
        b = (*(yuv->ptr) << 8) + 444 * (*(yuv->ptr - 1)-128);
        yuv->ptr += 2;

        r = r >> 8;
        g = g >> 8;
        b = b >> 8;

//        if(r > 255) r = 255;
//        if(g > 255) g = 255;
//        if(b > 255) b = 255;
//
//        if(r < 0) r = 0;
//        if(g < 0) g = 0;
//        if(b < 0) b = 0;

        *(rgb->ptr++) = r;
        *(rgb->ptr++) = g;
        *(rgb->ptr++) = b;
    }
}


void f640_full_yuv_to_rgb(struct f640_image *yuv, int index, struct f640_image *rgb) {
    int i = 1, r, g, b;

    yuv->ptr = yuv->data + 2;
    rgb->ptr = rgb->data + 3;

    while( i < yuv->size) {
        //
        r = (*(yuv->ptr) << 8) + 351 * (*(yuv->ptr + 1)-128);
        g = (*(yuv->ptr) << 8) - 179 * (*(yuv->ptr + 1)-128) - 86 * (*(yuv->ptr - 1)-128);
        b = (*(yuv->ptr) << 8) + 444 * (*(yuv->ptr - 1)-128);
        yuv->ptr += 2;

        r = r >> 8;
        g = g >> 8;
        b = b >> 8;

        *(rgb->ptr++) = r;
        *(rgb->ptr++) = g;
        *(rgb->ptr++) = b;

        //
        r = (*(yuv->ptr) << 8) + 351 * (*(yuv->ptr - 1)-128);
        g = (*(yuv->ptr) << 8) - 179 * (*(yuv->ptr - 1)-128) - 86 * (*(yuv->ptr + 1)-128);
        b = (*(yuv->ptr) << 8) + 444 * (*(yuv->ptr + 1)-128);
        yuv->ptr += 2;

        r = r >> 8;
        g = g >> 8;
        b = b >> 8;

        *(rgb->ptr++) = r;
        *(rgb->ptr++) = g;
        *(rgb->ptr++) = b;

        //
        i += 2;
    }
}



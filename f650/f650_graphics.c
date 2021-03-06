/*
 * file    : f650_graphics.c
 * project : f640
 *
 * Created on: Aug 27, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#include "f650.h"




/*
 *
 */
int bgra_alloc650(bgra650 *img, int width, int height) {
    if (!img) return -1;
    memset(img, 0, sizeof(bgra650));
    img->data   = calloc(width * height, sizeof(int32_t));
    img->width  = width;
    img->height = height;
    img->size   = width * height;
    img->x0 = 0;
    img->y0 = 0;
    img->sx = 1;
    img->sy = 1;
    return img->data != NULL ? 0 : -1;
}

int bgra_link650(bgra650 *img, char *data, int width, int height) {
    if (!img) return -1;
    img->data   = (uint32_t*)data;
    img->width  = width;
    img->height = height;
    img->size   = width * height;
    img->x0 = 0;
    img->y0 = 0;
    img->sx = 1;
    img->sy = 1;
    return img->data != NULL ? 0 : -1;
}

int bgraz_alloc650(bgraz650 *img, int width, int height) {
    if (!img) return -1;
    if (bgra_alloc650((bgra650*)img, width, height) != 0) return -1;
    img->zi = malloc(img->size * sizeof(double));
    memset(img->zi, 0x52, img->size << 3);
    img->res = NULL;
    return img->zi != NULL ? 0 : -1;
}

void bgra_origin650(bgra650 *img, double x0, double y0) {
    img->x0 = x0;
    img->y0 = y0;
}

void bgra_scale650(bgra650 *img, double sx, double sy) {
    img->sx = sx;
    img->sy = sy;
}

void bgra_clear650(bgra650 *img) {
    memset(img->data, 0, img->size << 2);
}

void memset2a650(void *align_x16, uint32_t fill, uint64_t size);
void bgra_clear2650(bgra650 *img) {
    memset2a650(img->data, 0, img->size >> 1);
}

void bgra_gray650(bgra650 *img, uint8_t gray) {
    memset(img->data, gray, img->size << 2);
}

void bgraz_gray650(bgraz650 *img, uint8_t gray) {
    memset(img->data, gray, img->size << 2);
    memset(img->zi, 0x52, img->size << 3);
}

/*
 *  > fill2
 */
void bgra_fill650(bgra650 *img, uint32_t color) {
    memseta650(img->data, color, img->size << 4);
}
// < fill1
void bgra_fill2650(bgra650 *img, uint32_t color) {
    memset2a650(img->data, color, img->size >> 1);
}

/*
 * a650_draw_line(&img1, 100, 20, -10, 30) != a650_draw_line(&img1, -10, 20, 100, 30) 110 pixels / 111 pixels
 */
int bgra_compare650(bgra650 *img1, bgra650 *img2) {
    if (img1->width != img2->width || img1->height != img2->height) return -1;
    int32_t *p1 = img1->data;
    int32_t *p2 = img2->data;
    int i, equ = 0, dif = 0;
    int pix1 = 0, pix2 = 0;

    for(i = 0 ; i < img1->size ; i++) {
        if (*p1 == *p2) {
            if (*p1 != 0) equ++;
//            equ++;
        } else {
            dif++;
        }
        if (*p1 != 0) pix1++;
        if (*p2 != 0) pix2++;
        p1++;
        p2++;
    }
    printf("ImgCompare : equal = %d ; different = %d ; img1 = %d ; img2 = %d\n", equ, dif, pix1, pix2);
    return dif != 0;
}
/*
 *
 */
int draw_linef650(bgra650 *img, double x1, double y1, double x2, double y2, uint32_t color) {
    double a, b, x, y;
    int i = 0;
    //
    x1 = img->sx * (x1 - img->x0) + 0.5 * img->width;
    x2 = img->sx * (x2 - img->x0) + 0.5 * img->width;
    if (x1 < 0 && x2 < 0) return -1;    // voir x1>= 0 || x2 >= 0
    if (x1 >= img->width && x2 >= img->width) return -1;    // idem
    //
    y1 = .5 * img->height - img->sy * (y1 - img->y0);
    y2 = .5 * img->height - img->sy * (y2 - img->y0);
    if (y1 < 0 && y2 < 0) return -1;
    if (y1 >= img->height && y2 >= img->height) return -1;
    //
    if (abs(x2 - x1) > abs(y2 - y1)) {
        if (x1 > x2) {
            a = x1 ; x1 = x2 ; x2 = a;
            a = y1 ; y1 = y2 ; y2 = a;
        }
        a = (y2 - y1) / (x2 - x1);
        b = y1 - a * x1;
        if (x1 < 0) { x1 = 0 ; y1 = b;}
        if (x2 >= img->width) { x2 = img->width ; y2 = a * x2 + b;}
        //
        for(x = x1 ; x <= x2 ; x += .65) {
            y = a * x + b;
            if (y < 0 || y >= img->height) continue;
            img->data[(int)x + img->width * ((int)y)] = color;
        }
    } else {//if (Math.abs(x2 - x1) < Math.abs(y2 - y1)) {
        if (y1 > y2) {
            a = x1 ; x1 = x2 ; x2 = a;
            a = y1 ; y1 = y2 ; y2 = a;
        } else if (y1 == y2) {
            return -1;
        }
        a = (x2 - x1) / (y2 - y1);
        b = x1 - a * y1;
        if (y1 < 0) { y1 = 0 ; x1 = b;}
        if (y2 >= img->height) { y2 = img->height ; x2 = a * y2 + b;}
        //
        for(y = y1 ; y <= y2 ; y += .65) {
            x = a * y + b;
            if (x < 0 || x >= img->width) continue;
            img->data[(int)x + img->width * ((int)y)] = color;
        }
    }
    return i;
}


/*
 *
 */
int draw_zlinef650(bgraz650 *img, double x1, double y1, double z1, double x2, double y2, double z2, uint32_t color) {
    double a, b, x, y, c, d, z;
    int i = 0;
    //
    x1 = img->sx * (x1 - img->x0) + 0.5 * img->width;
    x2 = img->sx * (x2 - img->x0) + 0.5 * img->width;
    if (x1 < 0 && x2 < 0) return -1;    // voir x1>= 0 || x2 >= 0
    if (x1 >= img->width && x2 >= img->width) return -1;    // idem
    //
    y1 = .5 * img->height - img->sy * (y1 - img->y0);
    y2 = .5 * img->height - img->sy * (y2 - img->y0);
    if (y1 < 0 && y2 < 0) return -1;
    if (y1 >= img->height && y2 >= img->height) return -1;
    //
    if (abs(x2 - x1) > abs(y2 - y1)) {
        if (x1 > x2) {
            a = x1 ; x1 = x2 ; x2 = a;
            a = y1 ; y1 = y2 ; y2 = a;
        }
        a = (y2 - y1) / (x2 - x1);
        b = y1 - a * x1;
        c = (z2 - z1) / (x2 - x1);
        d = z1 - c * x1;
        if (x1 < 0) { x1 = 0 ; y1 = b;}
        if (x2 >= img->width) { x2 = img->width ; y2 = a * x2 + b;}
        //
        for(x = x1 ; x <= x2 ; x += .65) {
            y = a * x + b;
            if (y < 0 || y >= img->height) continue;
            i = (int)x + img->width * ((int)y);
            z = c * x + d;
            if (img->zi[i] < z) continue;
            img->data[i] = color;
            img->zi[i] = z;
        }
    } else {//if (Math.abs(x2 - x1) < Math.abs(y2 - y1)) {
        if (y1 > y2) {
            a = x1 ; x1 = x2 ; x2 = a;
            a = y1 ; y1 = y2 ; y2 = a;
        } else if (y1 == y2) {
            return -1;
        }
        a = (x2 - x1) / (y2 - y1);
        b = x1 - a * y1;
        c = (z2 - z1) / (y2 - y1);
        d = z1 - c * y1;
        if (y1 < 0) { y1 = 0 ; x1 = b;}
        if (y2 >= img->height) { y2 = img->height ; x2 = a * y2 + b;}
        //
        for(y = y1 ; y <= y2 ; y += .65) {
            x = a * y + b;
            if (x < 0 || x >= img->width) continue;
            i = (int)x + img->width * ((int)y);
            z = c * y + d;
            if (img->zi[i] < z) continue;
            img->data[i] = color;
            img->zi[i] = z;
        }
    }
    return i;
}

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



double f650_test_asm(image *img, double x) {
    if (x >= 0) {
        x = img->width;
    }
    return x;
}

/*
 *
 */
int f650_draw_line(image *img, double x1, double y1, double x2, double y2) {
    double a, b, x, y;
    int i = 0;
    //
    img->x1 = x1 = img->sx * (x1 - img->x0) + 0.5 * img->width;
    img->x2 = x2 = img->sx * (x2 - img->x0) + 0.5 * img->width;
    if (x1 < 0 && x2 < 0) return -1;    // voir x1>= 0 || x2 >= 0
    if (x1 >= img->width && x2 >= img->width) return -1;    // idem
    //
    img->y1 = y1 = .5 * img->height - img->sy * (y1 - img->y0);
    img->y2 = y2 = .5 * img->height - img->sy * (y2 - img->y0);
    if (y1 < 0 && y2 < 0) return -1;
    if (y1 >= img->height && y2 >= img->height) return -1;
    //
    if (abs(x2 - x1) > abs(y2 - y1)) {
        if (x1 > x2) {
            a = x1 ; x1 = x2 ; x2 = a;
            a = y1 ; y1 = y2 ; y2 = a;
        }
        img->a = a = (y2 - y1) / (x2 - x1);
        img->b = b = y1 - a * x1;
        if (x1 < 0) { x1 = 0 ; y1 = b;}
        if (x2 >= img->width) { x2 = img->width ; y2 = a * x2 + b;}
        //
        for(x = x1 ; x <= x2 ; x += .65) {
            y = a * x + b;
            if (y < 0 || y >= img->height) continue;
            img->data[(int)x + img->width * ((int)y)] = img->color;
        }
    } else {//if (Math.abs(x2 - x1) < Math.abs(y2 - y1)) {
        if (y1 > y2) {
            a = x1 ; x1 = x2 ; x2 = a;
            a = y1 ; y1 = y2 ; y2 = a;
        } else if (y1 == y2) {
            return -1;
        }
        img->a = a = (x2 - x1) / (y2 - y1);
        img->b = b = x1 - a * y1;
        if (y1 < 0) { y1 = 0 ; x1 = b;}
        if (y2 >= img->height) { y2 = img->height ; x2 = a * y2 + b;}
        //
        for(y = y1 ; y <= y2 ; y += .65) {
            x = a * y + b;
            if (x < 0 || x >= img->width) continue;
            img->data[(int)x + img->width * ((int)y)] = img->color;
        }
    }
    return i;
}

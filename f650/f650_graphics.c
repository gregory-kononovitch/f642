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



double f650_line(image *img, double x) {
    if (x > img->x0) {
        x = img->width;
    }
    return x;
}

int f650_draw_line(image *img, double x1, double y1, double x2, double y2) {
    if (x1 > x2) {
        double t = x1;
        x1 = x2;
        x2 = t;
        double a = (y2 - y1) / (x2 - x1);
    }
    return 0;
}

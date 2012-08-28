/*
 * file    : f650_graphics.c
 * project : f640
 *
 * Created on: Aug 27, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>


typedef struct f650_int_rgb {
    int32_t     *img;
    int         width;
    int         height;
    int         size;
} image;

double f650_line(double x) {
    if (x < 0) {
        x = -x;
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

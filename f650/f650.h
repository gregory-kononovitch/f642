/*
 * file    : f650.h
 * project : f640
 *
 * Created on: Aug 28, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#ifndef F650_H_
#define F650_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>

#include <math.h>

typedef struct {
    int32_t     *data;          // 0
    int16_t     width;          // 8
    int16_t     height;         // 10
    int32_t     size;           // 12
    //
    double      x0;             // 16
    double      y0;             // 24
    double      sx;             // 32
    double      sy;             // 40
    // For ASM test
    double      x1;             // 48
    double      y1;             // 56
    double      x2;             // 64
    double      y2;             // 72
    double      a;              // 80
    double      b;              // 88
    int         index;          // 96
    //
    int         color;          // 100
} image;

// asm
int64_t ReadTSC();

// init
int  f650_new_image(image *img, int width, int height);
void f650_img_origin(image *img, double x0, double y0);
void f650_img_scale(image *img, double sx, double sy);

// Util
int f650_img_compare(image *img1, image *img2);

// For testing
void t650_clear_test(image *img);

// Draw line
int    f650_draw_line(image *img, double x1, double y1, double x2, double y2);
double a650_draw_line(image *img, double x1, double y1, double x2, double y2);


#endif /* F650_H_ */

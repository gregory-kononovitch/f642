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
    uint32_t    *data;          // 0
    int16_t     width;          // 8
    int16_t     height;         // 10
    int32_t     size;           // 12
    //
    double      x0;             // 16
    double      y0;             // 24
    double      sx;             // 32
    double      sy;             // 40
} bgra650;


typedef union {
    struct {
        double x;
        double y;
        double z;
        double t;
    };
    struct {
        double array[4];
    };
    struct {
        double r;
        double a;
        double z;
        double t;
    } cyl;
} vect650;


#define BLACK650    0xff000000
#define WHITE650    0xffffffff
#define RED650      0xffff0000
#define GREEN650    0xff00ff00
#define BLUE650     0xff0000ff
#define ORANGE650   0xffff7f00
#define YELLOW650   0xffffff00
#define CYAN650     0xff00ffff
#define MAGENTA650  0xffff00ff
typedef union {
    uint32_t value;
    struct {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
        uint8_t alpha;
    };
} color650;









// asm
int64_t ReadTSC();

// init
int  f650_alloc_image(bgra650 *img, int width, int height);
void f650_img_origin(bgra650 *img, double x0, double y0);
void f650_img_scale(bgra650 *img, double sx, double sy);

// Util
int f650_img_compare(bgra650 *img1, bgra650 *img2);

// For testing
void f650_img_clear(bgra650 *img);
void f650_img_fill(bgra650 *img, uint32_t color);

// Draw line
int a650_draw_line(bgra650 *img, double x1, double y1, double x2, double y2, uint32_t color);
int f650_draw_line(bgra650 *img, double x1, double y1, double x2, double y2, uint32_t color);


#endif /* F650_H_ */

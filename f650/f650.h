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


typedef struct {
    int32_t     *data;
    int16_t     width;
    int16_t     height;
    int32_t     size;
} image;

// asm
int64_t ReadTSC();
double a650_draw_line(image *img, double x1, double y1, double x2, double y2);


#endif /* F650_H_ */

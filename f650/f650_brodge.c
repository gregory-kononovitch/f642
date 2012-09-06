/*
 * file    : f650_brodge.c
 * project : f640
 *
 * Created on: Sep 6, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#include "f650.h"


/**
 * Simple struct for simple brodges
 */
float (*ffx11)(float d, float p);

typedef struct {
    float       x;
    float       y;          // 4
    ffx11       *intens;    // 8
    float       m;          // 16
    float       r;          // 20
    float       g;          // 24
    float       b;          // 28
} bsource650;

typedef struct {
    float       *img;
    int         width;      // 8
    int         height;     // 12
    //
    bsource650  *sources;   // 16
    int         nb_src;     // 24
    //
    int         res;        // 28
} brodge650;


long brodga650(brodge650 *brodge);

int main() {
    int width  = 512;
    int height = 288;
    //
    bsource650 src1;
    brodge650  brodge1;

    // Source 1
    float cosi(float d, float p) {
        p = (float)cos(p * d);
        return p > 0 ? p : -p;
    }
    src1.x = .5f * width;
    src1.y = .5f * height;
    src1.intens = &cosi;
    src1.m = 1.f;
    src1.r = 1.f;
    src1.g = 1.f;
    src1.b = 1.f;

    // Brodge
    brodge1.img = calloc(sizeof(float), 3 * width * height);
    brodge1.width  = width;
    brodge1.height = height;
    brodge1.sources = &src1;
    brodge1.nb_src = 1;

    //
    long l1, l2, l;
    struct timeval tv1, tv2;

    l1 = ReadTSC();
    l  = brodga650(&brodge1);
    l2 = ReadTSC();

    printf("Brodge return %ld for %ld µops\n", l, (l2 - l1));
    //
    return 0;
}

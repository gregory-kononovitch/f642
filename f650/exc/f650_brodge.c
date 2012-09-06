/*
 * file    : f650_brodge.c
 * project : f640
 *
 * Created on: Sep 6, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#include "../f650.h"


/**
 * Simple struct for simple brodges
 */
typedef float (*ffx11)(float d, float p);

typedef struct {
    float       x;
    float       y;          // 4
    float (*i)(float d, float p);  // 8
    float       m;          // 16
    float       r;          // 20
    float       g;          // 24
    float       b;          // 28
    //
    float       p;          // 32
    char        res[12];    // 36
} bsource650;

typedef struct {
    float       *img;
    int         width;      // 8
    int         height;     // 12
    //
    bsource650  *sources;   // 16
    int         nb_src;     // 24
    //
    int         size;        // 28
} brodge650;


long brodga650(brodge650 *brodge, bgra650 *img);

int main() {
    int i;
    int width  = 512;
    int height = 288;
    //
    bsource650 src1;
    brodge650  brodge1;
    bgra650 img;

    // Source 1
    float cosi(float d, float p) {
        p = (float)cos(p * d);
        return p > 0 ? p : -p;
    }
    src1.x = 3;//.5f * width;
    src1.y = 3;//.5f * height;
    src1.i = &cosi;
    src1.p = 2. * 3.14159 / 150.f;
    src1.m = 1.f;
    src1.r = 0.25f;
    src1.g = 1.f;
    src1.b = 0.f;

    // Brodge
    brodge1.img = calloc(sizeof(float), 3 * width * height);
    brodge1.width  = width;
    brodge1.height = height;
    brodge1.sources = &src1;
    brodge1.nb_src = 1;
    brodge1.size = width * height;

    // Image
    bgra_alloc650(&img, width, height);

    //
    long l1, l2, l;
    struct timeval tv1, tv2;

    l1 = ReadTSC();
    l  = brodga650(&brodge1, &img);
    l2 = ReadTSC();

    printf("Brodge return %ld for %ld µops [ %ld ; %ld ; %ld ] : %fs\n", l, (l2 - l1)
            , (l2 - l1) / brodge1.nb_src
            , (l2 - l1) / brodge1.nb_src / brodge1.height
            , (l2 - l1) / brodge1.nb_src / brodge1.width / brodge1.height
            , 1.5e-9 * (l2 - l1)
    );
    //
    for(i = 0 ; i < 8 ; i++) {
        printf("%d : %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f\n", i
                , brodge1.img[width * i + 0], brodge1.img[width * i + 1]
                , brodge1.img[width * i + 2], brodge1.img[width * i + 3]
                , brodge1.img[width * i + 4], brodge1.img[width * i + 5]
                , brodge1.img[width * i + 6], brodge1.img[width * i + 7]
        );
    }
    //
    return 0;
}

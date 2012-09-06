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
    bsource650  **sources;  // 16
    int         nb_src;     // 24
    //
    int         size;        // 28
} brodge650;

static bsource650   src[3];
static brodge650    brodge1;


long brodga650(brodge650 *brodge, bgra650 *img);

int brodge_init(int width, int height) {
    int i;
    // Source 1
    float cosi(float d, float p) {
        p = (float)cos(p * d);
        return p > 0 ? p : -p;
    }
    src[0].x = 200.;//.5f * width;
    src[0].y = 200.;//.5f * height;
    src[0].i = &cosi;
    src[0].p = 1. / 1000.f;
    src[0].m = 1.f;
    src[0].r = 0.25f;
    src[0].g = 1.f;
    src[0].b = 0.f;
    //
    src[1].x = 500.;//.5f * width;
    src[1].y = 300.;//.5f * height;
    src[1].i = &cosi;
    src[1].p = 1. / 150.f;
    src[1].m = 1.f;
    src[1].r = 1.0f;
    src[1].g = 0.35f;
    src[1].b = 0.f;
    //
    src[2].x = 400.;//.5f * width;
    src[2].y = 100.;//.5f * height;
    src[2].i = &cosi;
    src[2].p = 1. / 250.f;
    src[2].m = .5f;
    src[2].r = 0.5f;
    src[2].g = 0.f;
    src[2].b = 1.f;

    // Brodge
    brodge1.img = calloc(sizeof(float), 3 * width * height);
    brodge1.width  = width;
    brodge1.height = height;
    brodge1.sources = malloc(3 * sizeof(void*));
    brodge1.sources[0] = &src[0];
    brodge1.sources[1] = &src[1];
    brodge1.sources[2] = &src[2];
    brodge1.nb_src = 3;
    brodge1.size = width * height;

    //
    return 0;
}

int brodge_exec(bgra650 *img) {
    int i, c;
    long l1, l2, l;
    struct timeval tv1, tv2;

    brodge1.sources[0]->r = 1. / 100.;

    gettimeofday(&tv1, NULL);
    l1 = ReadTSC();
    l  = brodga650(&brodge1, img);
    l2 = ReadTSC();
    gettimeofday(&tv2, NULL);
    timersub(&tv2, &tv1, &tv2);

    printf("Brodge return %ld for %ld µops [ %ld ; %ld ; %ld ] : %.3f ms : %ld.%06lu s = %.0f ms\n", l, (l2 - l1)
            , (l2 - l1) / brodge1.nb_src
            , (l2 - l1) / brodge1.nb_src / brodge1.height
            , (l2 - l1) / brodge1.nb_src / brodge1.width / brodge1.height
            , 1000 * 1.5e-9 * (l2 - l1)
            , tv2.tv_sec, tv2.tv_usec
            , 1000. * tv2.tv_usec
    );
    //
    return 0;
}


int main0() {
    bgra650 bgra;
    //
    brodge_init(800, 448);
    // Image
    bgra_alloc650(&bgra, brodge1.width, brodge1.height);
    //
    brodge_exec(&bgra);
    //
    int i, c;
    for(c = 0 ; c < 3 ; c++) {
        int off = c * brodge1.width * brodge1.height;
        for(i = 0 ; i < 8 ; i++) {
            printf("%d : %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f\n", i
                    , brodge1.img[off + brodge1.width * i + 0], brodge1.img[off + brodge1.width * i + 1]
                    , brodge1.img[off + brodge1.width * i + 2], brodge1.img[off + brodge1.width * i + 3]
                    , brodge1.img[off + brodge1.width * i + 4], brodge1.img[off + brodge1.width * i + 5]
                    , brodge1.img[off + brodge1.width * i + 6], brodge1.img[off + brodge1.width * i + 7]
            );
        }
        printf("\n");
    }

    printf("\n");
    for(i = 0 ; i < 8 ; i++) {
        printf("%d : %8X %8X %8X %8X %8X %8X %8X %8X\n", i
                , bgra.data[brodge1.width * i + 0], bgra.data[brodge1.width * i + 1]
                , bgra.data[brodge1.width * i + 2], bgra.data[brodge1.width * i + 3]
                , bgra.data[brodge1.width * i + 4], bgra.data[brodge1.width * i + 5]
                , bgra.data[brodge1.width * i + 6], bgra.data[brodge1.width * i + 7]
        );
    }

    return 0;
}

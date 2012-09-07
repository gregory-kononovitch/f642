/*
 * file    : brodge650.c
 * project : f640
 *
 * Created on: Sep 7, 2012 (fork of f650_brodge.c)
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "brodge650.h"

static float lini(float d, float p) {
    if (p != 0) d /= p;
    else d = 0;
    return d > 0 ? d : -d;
}

brodge650 *brodge_init(int width, int height, int nb_src) {
    int i;
    brodge650 *brodge = calloc(1, sizeof(brodge650));
    bsource650 **src;
    if (!brodge) return NULL;
    //
    // Brodge
    brodge->img = calloc(sizeof(float), 3 * width * height);
    brodge->width  = width;
    brodge->height = height;
    brodge->nb_src = nb_src;
    brodge->size = width * height;
    //
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand((unsigned int)tv.tv_usec);
    //
    src = calloc(nb_src, sizeof(void*));
    brodge->sources = src;
    for(i = 0 ; i < nb_src ; i++) {
        vect650 v;
        src[i] = calloc(nb_src, sizeof(bsource650));
        random650(&v);
        src[i]->x = (.5 + v.x) * width;
        src[i]->y = (.5 + v.y) * height;
        src[i]->i = &lini;
        src[i]->p = 1. / (100 + 0.25 * v.z * width);
        src[i]->m = 1.f;
        src[i]->r = .5 * (1. + v.x);
        src[i]->g = .5 * (1. + v.y);
        src[i]->b = .5 * (1. + v.z);
        //
        src[i]->parm = calloc(1, sizeof(vect650));
        ((vect650*)src[i]->parm)->x = src[i]->x;
        ((vect650*)src[i]->parm)->y = src[i]->y;
        ((vect650*)src[i]->parm)->z = v.z / 100.;
        ((vect650*)src[i]->parm)->t = 0;
    }
    //
    return brodge;
}

void brodge_rebase(brodge650 *brodge) {
    int i;
    for(i = 0 ; i < brodge->nb_src ; i++) {
        vect650 v;
        random650(&v);
        brodge->sources[i]->x = (.5 + v.x) * brodge->width;
        brodge->sources[i]->y = (.5 + v.y) * brodge->height;
        brodge->sources[i]->i = &lini;
        brodge->sources[i]->p = 1. / (100 + 0.25 * v.z * brodge->width);
        brodge->sources[i]->m = 1.f;
        brodge->sources[i]->r = .5 * (1. + v.x);
        brodge->sources[i]->g = .5 * (1. + v.y);
        brodge->sources[i]->b = .5 * (1. + v.z);
        //
        ((vect650*)brodge->sources[i]->parm)->x = brodge->sources[i]->x;
        ((vect650*)brodge->sources[i]->parm)->y = brodge->sources[i]->y;
        ((vect650*)brodge->sources[i]->parm)->z = v.z / 100.;
        ((vect650*)brodge->sources[i]->parm)->t = 0;
    }
}

void brodge_free(brodge650 **brodge) {
    int i;

    for(i = 0 ; i < (*brodge)->nb_src ; i++) {
        if ((*brodge)->sources[i]->parm) free((*brodge)->sources[i]->parm);
        free((*brodge)->sources[i]);
    }
    free((*brodge)->sources);
    free((*brodge)->img);
    free(*brodge);
    *brodge = NULL;
}

int brodge_anim(brodge650 *brodge) {
    int i;
    vect650 alea;

    for(i = 0 ; i < brodge->nb_src ; i++) {
        random650(&alea);
        ((vect650*)brodge->sources[i]->parm)->t++;
        brodge->sources[i]->x = ((vect650*)brodge->sources[i]->parm)->x * cos(6.3 * ((vect650*)brodge->sources[i]->parm)->t * ((vect650*)brodge->sources[i]->parm)->z);
        brodge->sources[i]->y = ((vect650*)brodge->sources[i]->parm)->y * sin(6.3 * ((vect650*)brodge->sources[i]->parm)->t * ((vect650*)brodge->sources[i]->parm)->z);
//        brodge->sources[i].p = 1. / (alea.z + 1. / brodge->sources[i].p);
//        if (brodge->sources[i].p > .1) {
//            brodge->sources[i].p = 1. / (3. + 1. / brodge->sources[i].p);
//        }
        brodge->sources[i]->h  = 5. * brodge->sources[i]->e;
        brodge->sources[i]->e += 1;
        //
        random650(&alea);
//        brodge->sources[i].m = 1.f;
        brodge->sources[i]->r *= (1 + 0.05 * alea.x);
        brodge->sources[i]->g *= (1 + 0.05 * alea.y);
        brodge->sources[i]->b *= (1 + 0.05 * alea.z);
        float max;
        if (brodge->sources[i]->r > brodge->sources[i]->g) {
            if (brodge->sources[i]->r > brodge->sources[i]->b) max = brodge->sources[i]->r;
            else max = brodge->sources[i]->b;
        } else {
            if (brodge->sources[i]->g > brodge->sources[i]->b) max = brodge->sources[i]->g;
            else max = brodge->sources[i]->b;
        }
        brodge->sources[i]->r /= max;
        brodge->sources[i]->g /= max;
        brodge->sources[i]->b /= max;
    }

    return 0;
}

int brodge_exec(brodge650 *brodge, bgra650 *img) {
    long l1, l2, l;
    struct timeval tv1, tv2;

    //
    memset(brodge->img, 0, 3 * brodge->width * brodge->height * sizeof(float));

    //
    gettimeofday(&tv1, NULL);
    l1 = ReadTSC();
    l  = brodga650(brodge, img);
    l2 = ReadTSC();
    gettimeofday(&tv2, NULL);
    timersub(&tv2, &tv1, &tv2);

    printf("Brodge return %ld for %ld µops [ %.3fM ; %ld ; %ld ] : %.3f ms := %.0f ms\n", l, (l2 - l1)
            , 0.000001 * (l2 - l1) / brodge->nb_src
            , (l2 - l1) / brodge->nb_src / brodge->height
            , (l2 - l1) / brodge->nb_src / brodge->width / brodge->height
            , 1000 * 1.5e-9 * (l2 - l1)
            , .001 * tv2.tv_usec
    );
    //
    return 0;
}

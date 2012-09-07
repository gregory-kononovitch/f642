/*
 * file    : brodge650.h
 * project : f640
 *
 * Created on: Sep 7, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#ifndef BRODGE650_H_
#define BRODGE650_H_


#include "../f650.h"


typedef struct {
    float       x;
    float       y;          // 4
    float       (*i)(float d, float p);  // 8
    float       m;          // 16
    float       r;          // 20
    float       g;          // 24
    float       b;          // 28
    //
    float       p;          // 32
    float       h;          // 36
    float       e;          // 40
    // types
    int         flags;      // 44
    // axis
    float       a;          // 48
    float       b;          // 52
    // conic
    float       sx;         // 56
    float       sy;         // 60
    //
    void        *func;
    void        *parm;
} bsource650;

typedef struct {
    float       *img;
    int         width;      // 8
    int         height;     // 12
    //
    bsource650  **sources;  // 16
    int         nb_src;     // 24
    //
    int         flags;      // 28
} brodge650;


extern long brodga650(brodge650 *brodge, bgra650 *img);

brodge650 *brodge_init(int width, int height, int nb_src);
void brodge_free(brodge650 **brodge);
int brodge_anim(brodge650 *brodge);
void brodge_rebase(brodge650 *brodge);
int brodge_exec(brodge650 *brodge, bgra650 *img);

#endif /* BRODGE650_H_ */

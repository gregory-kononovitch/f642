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
    float       blue;       // 28
    //
    float       p;          // 32   wavelen
    float       h;          // 36   height
    float       e;          // 40   atenuation
    // types / transformations
#define BRDG_AXIS           1 << 0
#define BRDG_SCALE          1 << 1
//
#define BRDG_XLINE          1 << 8
#define BRDG_YLINE          1 << 9
#define BRDG_PSQUARE        1 << 10
#define BRDG_MSQUARE        1 << 11
#define BRDG_ELLIPSE        1 << 12
#define BRDG_PARABOLE       1 << 13
#define BRDG_HYPERBOLE      1 << 14
//
#define BRDG_XYPOLYNOME     1 << 16
//
#define BRDG_SIMPLE         BRDG_HYPERBOLE

    int         flags;      // 44
    // axis
    float       cos;        // 48
    float       sin;        // 52
    // conic / scaling
    float       a;          // 56
    float       b;          // 60
    // higher
    float       *axibyi;    // polynome coeff x, y
    int         plen;
    // trigo
    char        res[4];     //
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

//
extern long brodga650(brodge650 *brodge, bgra650 *img);
//
brodge650 *brodge_init(int width, int height, int nb_src);
void brodge_free(brodge650 **brodge);
//
void brodge_init_src(brodge650 *brodge, bsource650 *src);
void brodge_turn_src(brodge650 *brodge, bsource650 *src, float cos, float sin);
void brodge_scale_src(brodge650 *brodge, bsource650 *src, float a, float b);

//
int brodge_anim(brodge650 *brodge);
void brodge_rebase(brodge650 *brodge);
int brodge_exec(brodge650 *brodge, bgra650 *img);

#endif /* BRODGE650_H_ */

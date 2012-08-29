/*
 * c642_pictu2.c
 *
 * Date Oct 11, 2011
 * Author and copyright (C) 2011 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <linux/random.h>
#include <linux/time.h>
#include <asm/bitops.h>

#include "../../a051/src/a051_proc.h"
#include "c642_internal.h"

struct tmp1 {
    double x;
    double y;
};
typedef struct tmp1 cpoint;

#define C642_NUM_AT 5
struct c642_pict {
    int     width;
    int     height;
    int     size;

    int     num_at;
    cpoint  P[C642_NUM_AT][3];

    int     num_fc;
    double  F[C642_NUM_AT][3];

    u8      *picture;
    struct timespec sig; //current_kernel_time();

    struct a051_proc_env *env;
};

/*
 *             //////////////////////////////////////
 */
double q(cpoint *p) {
    return p->x * p->y;
}

double _dist_(const cpoint *p1, const cpoint *p2) {
    long d1 = 10000*(p2->x - p1->x)*(p2->x - p1->x);
    long d2 = 10000*(p2->y - p1->y)*(p2->y - p1->y);
    return 1. * ( d1 + d2 ) / 100.;
}

double _random_() {
    double d = 1.0 * random32();
    return d / 0x100000000L;
}

double _abs_(double d) {
    return d > 0 ? d : -d;
}

double _random2_(double d1, double d2) {
    return random32() < 0x7FFFFFFF ? d1 : d2;
}

double _random3_(double d1, double d2, double d3) {
    u32 r = random32();
    return  r < 0x55555555 ? d1 : r < 0xAAAAAAAA ? d2 : d3;
}

double _random3m_(double d1, double d2, double d3, int m1, int m2, int m3) {
    u64 r = random32();
    return  r < 0xFFFFFFFF * m1 / (m1+m2+m3) ? d1 : r < 0xFFFFFFFF * (m1+m2)/(m1+m2+m3) ? d2 : d3;
}
//
double _max2_(double d1, double d2) {
    if ( d1 > d2 ) return d1;
    return d2;
}

double _max3_(double d1, double d2, double d3) {
    if ( d1 > d2 ) {
        if ( d1 > d3 ) return d1;
        else return d3;
    } else {
        if ( d2 > d3 ) return d2;
        else return d3;
    }
}
////
double _moy2_(double d1, double d2) { return (d1+d2)/2; }
double _moy3_(double d1, double d2, double d3) { return (d1+d2+d3)/3; }
double _moy2m_(double d1, double d2, int m1, int m2) { return (m1*d1+m2*d2)/(m1+m2); }
double _moy3m_(double d1, double d2, double d3, int m1, int m2, int m3) { return (m1*d1+m2*d2+m3*d3)/(m1+m2+m3); }
////
int _imin_(int *c) {
    if ( c[0] < c[1] ) {
        if (c[0] < c[2] ) return 0;
        return 2;
    } else {
        if (c[1] < c[2] ) return 1;
        return 2;
    }
}
int _imax_(int *c) {
    if ( c[0] > c[1] ) {
        if (c[0] > c[2] ) return 0;
        return 2;
    } else {
        if (c[1] > c[2] ) return 1;
        return 2;
    }
}


/////*
//// *             000000000000000000000000000000000000000
//// */
////double osc1(cpoint p0, cpoint p, double pas) {
////    double d = dist(p0, p);
////    d = Math.cos( 6.28230 * d / pas );
////    return d > 0 ? d : -d;
////}
double osc2(const cpoint *p0, const cpoint *p, double pas) {
    double d = _dist_(p0, p) / pas;
    return d - (long)d;
}
////double osc3(cpoint p0, cpoint p, double pas) {
////    double dx = (p.x - p0.x); if (dx<0) dx = -dx;
////    dx = dx/pas - (int)(dx/pas);
////    double dy = (p.y - p0.y); if (dy<0) dy = -dy;
////    dy = dy/pas - (int)(dy/pas);
////    return (0.2 + max(dx,dy)) / 1.2;
////}
double osc4(cpoint *p0, cpoint *p, double pas) {
    double d = _max2_(_abs_(p->x - p0->x), _abs_(p->y - p0->y));
    d = d / pas;
    return d - (long)d;
}


struct c642_pict *c642_create(int width, int height)
{
    struct c642_pict *pict;

    pict = kmalloc(sizeof(struct c642_pict), GFP_KERNEL);
    if (!pict) {
        printk("Memo alloc ko\n");
        return NULL;
    }
    pict->size   = 3 * width * height;
    pict->width  = width;
    pict->height = height;
    pict->num_at = C642_NUM_AT;
    pict->num_fc = C642_NUM_AT;

    //
    pict->picture = kmalloc(pict->size, GFP_KERNEL);
    if (!pict->picture) {
        printk("Memo alloc ko\n");
        kfree(pict);
        return NULL;
    }

    //
    pict->env = a051_proc_env_alloc("bird", "brodge2", 3 * pict->size / 1024 + 4);
    if (!pict->env) {
        printk("Memo env alloc ko\n");
        kfree(pict->picture);
        kfree(pict);
        return NULL;
    }

    printk("Struct created\n");
    return pict;
}

void c642_destroy(struct c642_pict *pict)
{
    a051_proc_env_free(pict->env);
    kfree(pict->picture);
    kfree(pict);
}

void c642_esqu(struct c642_pict *pict)
{
    int c, n;

    for(c = 0 ; c < 3 ; c++) {
        for(n = 0 ; n < pict->num_at ; n++) {
            pict->P[n][c].x = 4 * pict->width  * ( _random_() - 0.5 );
            pict->P[n][c].y = 4 * pict->height * ( _random_() - 0.5 );

            printk("ESQU : P[%d][%d].x = %d | P[%d][%d].y = %d\n", n, c, (int) pict->P[n][c].x, n, c, (int) pict->P[n][c].y);
        }
        for(n = 0 ; n < pict->num_fc ; n++) {
            pict->F[n][c]   = 2 * pict->width * _random_();
            printk("F[%d][%d] = %d\n", n, c, (int)pict->F[n][c]);
        }
    }
}

double coln1(struct c642_pict *pict, int c, cpoint *p, int *at, int *m)
{
    double d = _moy2m_(
              osc2(&pict->P[at[0]][c], p, pict->F[at[0]][c])
            , osc4(&pict->P[at[1]][c], p, pict->F[at[1]][c]), m[0], m[1]
    );

    printk("COLN1(%d,%d) = %d\n", (int)p->x, (int)p->y, (int)(256*d));
    return d;
}

void brodge1(struct c642_pict *pict, int *at, int *m)
{
    u8 *im, *color;
    int x, y;
    u8 c;
    cpoint p[1];
    //°\\//
    im = pict->picture;
    for (y = 0 ; y < pict->height ; y++ ) {
        for (x = 0 ; x < pict->width ; x++ ) {
            c = 256 * coln1(pict, 0, p, at, m);       *(im++) = c;
            c = 256 * coln1(pict, 1, p, at, m);       *(im++) = c;
            c = 256 * coln1(pict, 2, p, at, m);       *(im++) = c;
        }
    }
}


void brodge(struct c642_pict *pict)
{
    int o;
    int at[2], m[2];
    double d;
    struct timespec ls = current_kernel_time();

    c642_esqu(pict);

    //°\\ 0: osc1 , 1: osc2 , 2: osc3
    for(o = 0; o < 2; o++) {
        d = pict->num_at * _random_();
        at[o] = (int)d;
    }
    d = 4L * _random_();    m[0] = (int)d;
    d = 4L * _random_();    m[1] = (int)d;

    brodge1(pict, at, m);

    pict->sig = current_kernel_time();
    d = (pict->sig.tv_nsec - ls.tv_nsec);
    if ( d < 0 ) {
        printk("Brodge  ' %lX '  taked  %ld.%lds.\n", pict->sig.tv_sec, pict->sig.tv_sec - ls.tv_sec - 1, (1000000000 - d)/1000000);
    } else {
        printk("Brodge  ' %lX '  taked  %ld.%lds.\n", pict->sig.tv_sec, pict->sig.tv_sec - ls.tv_sec, d/1000000);
    }

    // Sig

    // Export
    d = a051_data_write(pict->env, pict->picture, pict->size);
    printk("Brodge  ' %lX '  send %ld bytes\n", 0x7F1FFFFFFF & (1L * pict->sig.tv_sec * 1000L + pict->sig.tv_nsec / 1000000L), d);
}

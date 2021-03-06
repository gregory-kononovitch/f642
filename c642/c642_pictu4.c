/*
 * c642_pictu.c
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
    long x;
    long y;
};
typedef struct tmp1 cpoint;

#define C642_NUM_AT 5
struct c642_pict {
    u32     width;
    u32     height;
    u32     size;

    u32     num_at;
    cpoint  P[C642_NUM_AT][3];

    u32     num_fc;
    long    F[C642_NUM_AT][3];

    u8      *picture;
    struct timespec sig; //current_kernel_time();

    struct a051_proc_env *env;
};

//
void _random_(_frac_ *f) {
    f->num = random32();
    f->den = 0xFFFFFFFF;
    if (f->num == 0xFFFFFFFF ) f->num = 0xFFFFFFFE;
}

///*
// *             //////////////////////////////////////
// */
long _dist_(const cpoint *p1, const cpoint *p2) {
    return int_sqrt( (p2->x - p1->x)*(p2->x - p1->x) + (p2->y - p1->y)*(p2->y - p1->y) );
}

long _abs_(long d) {
    return d > 0 ? d : -d;
}

long _random2_(long d1, long d2) {
    return random32() < 0x7FFFFFFF ? d1 : d2;
}

long _random3_(long d1, long d2, long d3) {
    u32 r = random32();
    return  r < 0x55555555 ? d1 : r < 0xAAAAAAAA ? d2 : d3;
}

long _random3m_(long d1, long d2, long d3, int m1, int m2, int m3) {
    u64 r = random32();
    return  r < 0xFFFFFFFF * m1 / (m1+m2+m3) ? d1 : r < 0xFFFFFFFF * (m1+m2)/(m1+m2+m3) ? d2 : d3;
}
//
long _max2_(long d1, long d2) {
    if ( d1 > d2 ) return d1;
    return d2;
}

long _max3_(long d1, long d2, long d3) {
    if ( d1 > d2 ) {
        if ( d1 > d3 ) return d1;
        else return d3;
    } else {
        if ( d2 > d3 ) return d2;
        else return d3;
    }
}
////
long _moy2_(long d1, long d2) { return (d1+d2)/2; }
long _moy3_(long d1, long d2, long d3) { return (d1+d2+d3)/3; }
long _moy2m_(long d1, long d2, int m1, int m2) { return (m1*d1+m2*d2)/(m1+m2); }
long _moy3m_(long d1, long d2, long d3, int m1, int m2, int m3) { return (m1*d1+m2*d2+m3*d3)/(m1+m2+m3); }

void _moy2mp_(_frac_ *res, _frac_ *d1, _frac_ *d2, int m1, int m2) {
    res->num = 1L * (m1*d1->num*d2->den + m2*d1->den*d2->num);
    res->den = 1L * d1->den * d2->den * (m1 + m2);
}

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
//
//
/////*
//// *             000000000000000000000000000000000000000
//// */
////long osc1(cpoint p0, cpoint p, long pas) {
////    long d = dist(p0, p);
////    d = Math.cos( 6.28230 * d / pas );
////    return d > 0 ? d : -d;
////}
void osc2(_frac_ *res, cpoint *p0, cpoint *p, long pas) {
    res->num = _dist_(p0, p) % pas;
    res->den = pas;
}
////long osc3(cpoint p0, cpoint p, long pas) {
////    long dx = (p.x - p0.x); if (dx<0) dx = -dx;
////    dx = dx/pas - (int)(dx/pas);
////    long dy = (p.y - p0.y); if (dy<0) dy = -dy;
////    dy = dy/pas - (int)(dy/pas);
////    return (0.2 + max(dx,dy)) / 1.2;
////}
void osc4(_frac_ *res, cpoint *p0, cpoint *p, long pas) {
    res->num = _max2_(_abs_(p->x - p0->x), _abs_(p->y - p0->y)) % pas;
    res->den = pas;
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
    pict->picture = kmalloc(3*width * height, GFP_KERNEL);
    if (!pict->picture) {
        printk("Memo alloc ko\n");
        kfree(pict);
        return NULL;
    }

    //
    pict->env = a051_proc_env_alloc("bird", "brodge", 3 * width * height / 1024);
    if (!pict->env) {
        printk("Memo env alloc ko\n");
        kfree(pict->picture);
        kfree(pict);
        return NULL;
    }

    printk("Struct created\n");
    return pict;
}
//
void c642_destroy(struct c642_pict *pict)
{
    a051_proc_env_free(pict->env);
    kfree(pict->picture);
    kfree(pict);
}
//
void c642_esqu(struct c642_pict *pict)
{
    int c, n;
    _frac_ coef;

    for(c = 0 ; c < 3 ; c++) {
        for(n = 0 ; n < pict->num_at ; n++) {
            _random_(&coef);
            pict->P[n][c].x = 2L * pict->width * coef.num / coef.den;
            _random_(&coef);
            pict->P[n][c].y = 2L * pict->height * coef.num / coef.den;
        }
        for(n = 0 ; n < pict->num_fc ; n++) {
            _random_(&coef);
            pict->F[n][c]   = 2L * pict->width * coef.num / coef.den;
        }
    }
}
//
void coln1(_frac_ *res, struct c642_pict *pict, int c, cpoint *p, int *at, int *m)
{
    _frac_ res2, res4;
    osc2(&res2, &pict->P[at[0]][c], p, pict->F[at[0]][c]);
    osc4(&res4, &pict->P[at[1]][c], p, pict->F[at[1]][c]);

    _moy2mp_(res, &res2, &res4, m[0], m[1]);
}

void brodge1(struct c642_pict *pict, int *at, int *m)
{
    _frac_ col;
    u8 *im;
    long x, y;
    cpoint p[1];
    //°\\//
    im = pict->picture;
    for (y = 0 ; y < pict->height ; y++ ) {
        for (x = 0 ; x < pict->width ; x++ ) {
            p->x = x;   p->y = y;
            coln1(&col, pict, 0, p, at, m);
            *(im++) = 1L * 0x100 * col.num / col.den;

            coln1(&col, pict, 1, p, at, m);
            *(im++) = 1L * 0x100 * col.num / col.den;

            coln1(&col, pict, 2, p, at, m);
            *(im++) = 1L * 0x100 * col.num / col.den;
        }
    }
}


void brodge(struct c642_pict *pict)
{
    int o;
    int at[2], m[2];
    long d;
    struct timespec ls = current_kernel_time();

    c642_esqu(pict);

    //°\\ 0: osc1 , 1: osc2 , 2: osc3
    for(o = 0; o < 2; o++) {
        d = 1L * pict->num_at * random32();
        at[o] = d >> 32;
    }
    d = 4L * random32();    m[0] = d >> 32;
    d = 4L * random32();    m[1] = d >> 32;

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

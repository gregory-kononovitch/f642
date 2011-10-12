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



void _add_(_num_ *d1, _num_ *d2, _num_ *r) {
    long ret;
    r->plin  = d1->plin + d2->plin;
    ret      = 0L + d1->frac + d2->frac;
    r->plin += (ret >> 32);
    r->frac  = (ret & 0xFFFFFFFF);

}

void _sub_(_num_ *d1, _num_ *d2, _num_ *r) {
    long ret;
    if ( d1->plin > d2->plin || (d1->plin == d2->plin && d1->frac > d2->frac) ) {
        r->plin  = d1->plin - d2->plin;
        ret      = 0L + d1->frac - d2->frac;
        if ( ret < 0 ) {
            r->plin--;
            r->frac= 0x100000000 + ret;

        } else {
            r->frac  = ret;
        }
    } else {
        r->plin  = d2->plin - d1->plin;
        ret      = 0L + d2->frac - d1->frac;
        if ( ret < 0 ) {
            r->plin--;
            r->frac= 0x100000000 + ret;

        } else {
            r->frac  = ret;
        }
    }
}

void _mul_(_num_ *d1, _num_ *d2, _num_ *r) {
    u64 ret, frac;
    r->plin  = d1->plin * d2->plin;
    //
    ret      = 1L * d1->frac * d2->frac;
    frac     = ret >> 32;
    //
    ret      = 1L * d1->plin * d2->frac;
    r->plin += ret >> 32;
    frac    += ret & 0xFFFFFFFF;
    //
    ret      = 1L * d1->frac * d2->plin;
    r->plin += ret >> 32;
    frac    += ret & 0xFFFFFFFF;
    //
    r->plin += frac >> 32;
    r->frac  = frac & 0xFFFFFFFF;
}

// @@@ kata ...
void _div_(_num_ *d1, _num_ *d2, _num_ *r) {
    u64 res, den;
    _num_ t,tt;
    if ( d2->plin == 0 && d2->frac == 0 ) {
        r->plin = 0xFFFFFFFF; r->frac = 0xFFFFFFFF;
        return;
    }
    if ( d1->plin > d2->plin || (d1->plin == d2->plin && d1->frac > d2->frac) ) {
        res = d1->plin;     res = (res << 32);    res += d1->frac;
        den = d2->plin;     den = (den << 32);    den += d2->frac;
        r->plin = res / den;
    } else {
        r->plin = 0;
    }
    if ( d2->plin == 0 ) {
        r->frac = 0;
        return;
    }
    t.plin = r->plin;  t.frac = 0;
    _mul_(&t, d2, &tt); _sub_(d1, &tt, &t);
    res = t.plin;       res = (res << 32);     res += t.frac;
    r->frac = res / d2->plin;

}

void _random_(_num_ *d) {
    d->plin = 0;
    d->frac = random32();
}

struct tmp1 {
    _num_ x;
    _num_ y;
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
    _num_   F[C642_NUM_AT][3];

    u8      *picture;
    struct timespec sig; //current_kernel_time();

    struct a051_proc_env *env;
};
//
///*
// *             //////////////////////////////////////
// */
void _dist_(const cpoint *p1, const cpoint *p2, _num_ *r) {
    long res;
    _num_ r1, r2, r3;
    _sub_(&p2->x, &p1->x, &r1);
    _mul_(&r1, &r1, &r2);
    _sub_(&p2->y, &p1->y, &r1);
    _mul_(&r1, &r1, &r3);
    _add_(&r2, &r3, &r1);
    res = int_sqrt( (r1.plin << 32) + r1.frac );
    r->plin = res >> 16;
    r->frac = 0;
}
//
//long _abs_(long d) {
//    return d > 0 ? d : -d;
//}
//
//long _random2_(long d1, long d2) {
//    return random32() < 0x7FFFFFFF ? d1 : d2;
//}
//
//long _random3_(long d1, long d2, long d3) {
//    u32 r = random32();
//    return  r < 0x55555555 ? d1 : r < 0xAAAAAAAA ? d2 : d3;
//}
//
//long _random3m_(long d1, long d2, long d3, int m1, int m2, int m3) {
//    u64 r = random32();
//    return  r < 0xFFFFFFFF * m1 / (m1+m2+m3) ? d1 : r < 0xFFFFFFFF * (m1+m2)/(m1+m2+m3) ? d2 : d3;
//}
////
//long _max2_(long d1, long d2) {
//    if ( d1 > d2 ) return d1;
//    return d2;
//}
//
//long _max3_(long d1, long d2, long d3) {
//    if ( d1 > d2 ) {
//        if ( d1 > d3 ) return d1;
//        else return d3;
//    } else {
//        if ( d2 > d3 ) return d2;
//        else return d3;
//    }
//}
//////
//long _moy2_(long d1, long d2) { return (d1+d2)/2; }
//long _moy3_(long d1, long d2, long d3) { return (d1+d2+d3)/3; }
//long _moy2m_(long d1, long d2, int m1, int m2) { return (m1*d1+m2*d2)/(m1+m2); }
//long _moy3m_(long d1, long d2, long d3, int m1, int m2, int m3) { return (m1*d1+m2*d2+m3*d3)/(m1+m2+m3); }
//////
//int _imin_(int *c) {
//    if ( c[0] < c[1] ) {
//        if (c[0] < c[2] ) return 0;
//        return 2;
//    } else {
//        if (c[1] < c[2] ) return 1;
//        return 2;
//    }
//}
//int _imax_(int *c) {
//    if ( c[0] > c[1] ) {
//        if (c[0] > c[2] ) return 0;
//        return 2;
//    } else {
//        if (c[1] > c[2] ) return 1;
//        return 2;
//    }
//}
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
//u32 osc2(const cpoint *p0, const cpoint *p, long pas) {
//    u32 *o;
//    long d = _dist_(p0, p) / pas;
//    o = (u32*) &d;
//    return *o;
//}
////long osc3(cpoint p0, cpoint p, long pas) {
////    long dx = (p.x - p0.x); if (dx<0) dx = -dx;
////    dx = dx/pas - (int)(dx/pas);
////    long dy = (p.y - p0.y); if (dy<0) dy = -dy;
////    dy = dy/pas - (int)(dy/pas);
////    return (0.2 + max(dx,dy)) / 1.2;
////}
//u32 osc4(cpoint *p0, cpoint *p, long pas) {
//    u32 *o;
//    long d = _max2_(_abs_(p->x - p0->x), _abs_(p->y - p0->y));
//    d = d/pas;
//    o = (u32*) &d;
//    return *o;
//}
//
//
//struct c642_pict *c642_create(int width, int height)
//{
//    struct c642_pict *pict;
//
//    pict = kmalloc(sizeof(struct c642_pict), GFP_KERNEL);
//    if (!pict) {
//        printk("Memo alloc ko\n");
//        return NULL;
//    }
//    pict->size   = 3 * width * height;
//    pict->width  = width;
//    pict->height = height;
//    pict->num_at = C642_NUM_AT;
//    pict->num_fc = C642_NUM_AT;
//
//    //
//    pict->picture = kmalloc(3*width * height, GFP_KERNEL);
//    if (!pict->picture) {
//        printk("Memo alloc ko\n");
//        kfree(pict);
//        return NULL;
//    }
//
//    //
//    pict->env = a051_proc_env_alloc("bird", "brodge", 3 * width * height / 1024);
//    if (!pict->env) {
//        printk("Memo env alloc ko\n");
//        kfree(pict->picture);
//        kfree(pict);
//        return NULL;
//    }
//
//    printk("Struct created\n");
//    return pict;
//}
//
//void c642_destroy(struct c642_pict *pict)
//{
//    a051_proc_env_free(pict->env);
//    kfree(pict->picture);
//    kfree(pict);
//}
//
//void c642_esqu(struct c642_pict *pict)
//{
//    int c, n;
//
//    for(c = 0 ; c < 3 ; c++) {
//        for(n = 0 ; n < pict->num_at ; n++) {
//            pict->P[n][c].x = (pict->width  << 32) * 2 * random32();
//            pict->P[n][c].y = (pict->height << 32) * 2 * random32();
//        }
//        for(n = 0 ; n < pict->num_fc ; n++) {
//            pict->F[n][c]   = (pict->width  << 32) * 2 * random32();
//        }
//    }
//}
//
//u32 coln1(struct c642_pict *pict, int c, cpoint *p, int *at, int *m)
//{
//    u32 *color;
//    long d = _moy2m_(
//              osc2(&pict->P[at[0]][c], p, pict->F[at[0]][c])
//            , osc4(&pict->P[at[1]][c], p, pict->F[at[1]][c]), m[0], m[1]
//    );
//    color = (u32*) &d;
//    return *color;
//}
//
//void brodge1(struct c642_pict *pict, int *at, int *m)
//{
//    u8 *im, *color;
//    long x, y;
//    u32 c;
//    cpoint p[1];
//    //°\\//
//    color = (u8*) &c;   color = color + 3;
//    im = pict->picture;
//    for (y = 0 ; y < pict->height ; y++ ) {
//        for (x = 0 ; x < pict->width ; x++ ) {
//            p->x = x << 32 ; p->y = y << 32;
//            c = coln1(pict, 0, p, at, m); *(im++) = *color;
//            c = coln1(pict, 1, p, at, m); *(im++) = *color;
//            c = coln1(pict, 2, p, at, m); *(im++) = *color;
//        }
//    }
//}
//
//
//void brodge(struct c642_pict *pict)
//{
//    int o;
//    int at[2], m[2];
//    long d;
//    struct timespec ls = current_kernel_time();
//
//    c642_esqu(pict);
//
//    //°\\ 0: osc1 , 1: osc2 , 2: osc3
//    for(o = 0; o < 2; o++) {
//        d = 1L * pict->num_at * random32();
//        at[o] = d >> 32;
//    }
//    d = 4L * random32();    m[0] = d >> 32;
//    d = 4L * random32();    m[1] = d >> 32;
//
//    brodge1(pict, at, m);
//
//    pict->sig = current_kernel_time();
//    d = (pict->sig.tv_nsec - ls.tv_nsec);
//    if ( d < 0 ) {
//        printk("Brodge  ' %lX '  taked  %ld.%lds.\n", pict->sig.tv_sec, pict->sig.tv_sec - ls.tv_sec - 1, (1000000000 - d)/1000000);
//    } else {
//        printk("Brodge  ' %lX '  taked  %ld.%lds.\n", pict->sig.tv_sec, pict->sig.tv_sec - ls.tv_sec, d/1000000);
//    }
//
//    // Sig
//
//    // Export
//    d = a051_data_write(pict->env, pict->picture, pict->size);
//    printk("Brodge  ' %lX '  send %ld bytes\n", 0x7F1FFFFFFF & (1L * pict->sig.tv_sec * 1000L + pict->sig.tv_nsec / 1000000L), d);
//}

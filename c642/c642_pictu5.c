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


u16 pas_mask[16] = {0,1,(1<<2)-1,(1<<3)-1,(1<<4)-1,(1<<5)-1,(1<<6)-1,(1<<7)-1
        ,(1<<8)-1,(1<<9)-1,(1<<10)-1,(1<<11)-1,(1<<12)-1,(1<<13)-1,(1<<14)-1,(1<<15)-1
};

struct tmp1 {
    int x;
    int y;
};
typedef struct tmp1 cpoint;

#define C642_NUM_AT 5
struct c642_pict {
    int     width;
    int     height;
    int     size;

    u8      num_at;
    cpoint  P[C642_NUM_AT][3];

    u8      num_fc;
    u8      F[C642_NUM_AT][3];

    u8      *picture;
    struct timespec sig; //current_kernel_time();

    struct a051_proc_env *env;
};

//
u8 _random_(void) {
    return random32() >> 24;
}

///*
// *             //////////////////////////////////////
// */
u32 _dist_(const cpoint *p1, const cpoint *p2) {
    return int_sqrt( (p2->x - p1->x)*(p2->x - p1->x) + (p2->y - p1->y)*(p2->y - p1->y) );
}

u32 _abs_(int d) {
    return d > 0 ? d : -d;
}

int _random2_(int d1, int d2) {
    return random32() < 0x7FFFFFFF ? d1 : d2;
}

int _random3_(int d1, int d2, int d3) {
    u32 r = random32();
    return  r < 0x55555555 ? d1 : r < 0xAAAAAAAA ? d2 : d3;
}
// @@@ ..
int _random3m_(int d1, int d2, int d3, u8 m1, u8 m2, u8 m3) {
    u64 r = random32();
    return  r < 0x100000000L * m1 / (m1+m2+m3) ? d1 : r < 0x100000000L * (m1+m2)/(m1+m2+m3) ? d2 : d3;
}
//
int _max2_(int d1, int d2) {
    if ( d1 > d2 ) return d1;
    return d2;
}

int _max3_(int d1, int d2, int d3) {
    if ( d1 > d2 ) {
        if ( d1 > d3 ) return d1;
        else return d3;
    } else {
        if ( d2 > d3 ) return d2;
        else return d3;
    }
}
// @@@ ..
int _moy2_(int d1, int d2) { return (d1+d2) >> 1; }
int _moy3_(int d1, int d2, int d3) { return (d1+d2+d3)/3; }
int _moy2m_(int d1, int d2, int m1, int m2, int m) {
    if (m1) {
        if (m2) {
            return ( (d1 << m1) + (d2 << m2) ) >> m;
        } else {
            return d1;
        }
    } else {
        if (m2) {
            return d2;
        } else {
            return 0;
        }
    }
}
int _moy3m_(int d1, int d2, int d3, int m1, int m2, int m3) { return (m1*d1+m2*d2+m3*d3)/(m1+m2+m3); }

u8 _moy2mp_(u8 d1, u8 d2, int m1, int m2) {
    return (m1 * d1 + m2 * d2) / (m1 + m2);
}

////
int _imin_(u8 *c) {
    if ( c[0] < c[1] ) {
        if (c[0] < c[2] ) return 0;
        return 2;
    } else {
        if (c[1] < c[2] ) return 1;
        return 2;
    }
}
int _imax_(u8 *c) {
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
u8 osc2(cpoint *p0, cpoint *p, int pas) {
    return _dist_(p0, p) & pas_mask[pas];
}
////long osc3(cpoint p0, cpoint p, long pas) {
////    long dx = (p.x - p0.x); if (dx<0) dx = -dx;
////    dx = dx/pas - (int)(dx/pas);
////    long dy = (p.y - p0.y); if (dy<0) dy = -dy;
////    dy = dy/pas - (int)(dy/pas);
////    return (0.2 + max(dx,dy)) / 1.2;
////}
u8 osc4(cpoint *p0, cpoint *p, int pas) {
    return _max2_(_abs_(p->x - p0->x), _abs_(p->y - p0->y)) & pas_mask[pas];
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
    pict->env = a051_proc_env_alloc("anim", "brodge2", 3 * width * height / 1024);
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
    u8 r;

    for(c = 0 ; c < 3 ; c++) {
        for(n = 0 ; n < pict->num_at ; n++) {
            r = _random_();
            if ( r & 0x80 ) pict->P[n][c].x = ( 3 * pict->width  * r ) >> 8;
            else pict->P[n][c].x = -( 3 * pict->width  * r ) >> 8;

            r = _random_();
            if ( r & 0x80 ) pict->P[n][c].y = ( 3 * pict->height * r ) >> 8;
            else pict->P[n][c].y = -( 3 * pict->height  * r ) >> 8;
        }
        for(n = 0 ; n < pict->num_fc ; n++) {
            pict->F[n][c]   = (_random_() >> 5) + 3;
        }
    }
}
//
u8 coln1(struct c642_pict *pict, int c, cpoint *p, int *at, int *m)
{
    return _moy2m_(
              osc2(&pict->P[at[0]][c], p, pict->F[at[0]][c])
            , osc4(&pict->P[at[1]][c], p, pict->F[at[1]][c])
            , m[0], m[1], m[2]
    );
}

void brodge1(struct c642_pict *pict, int *at, int *m)
{
    u8 *im;
    int x, y;
    cpoint p[1];
    //°\\//
    im = pict->picture;
    for (y = 0 ; y < pict->height ; y++ ) {
        for (x = 0 ; x < pict->width ; x++ ) {
            p->x = x;   p->y = y;
            *(im++) = coln1(pict, 0, p, at, m);
            *(im++) = coln1(pict, 1, p, at, m);
            *(im++) = coln1(pict, 2, p, at, m);
        }
    }
}


void brodge(struct c642_pict *pict)
{
    int at[2], m[3];
    long d;
    struct timespec ls = current_kernel_time();

    c642_esqu(pict);

    //°\\ 0: osc1 , 1: osc2 , 2: osc3
    at[0] = random32() & 0x01;
    at[1] = random32() & 0x01;

    m[0]  = random32() >> 29;
    m[1]  = random32() >> 29;
    m[2]  = m[0] + m[1];
    if (!m[2]) {
        m[ random32() >> 31 ] = 1;
        m[2] = 1;
    }

    brodge1(pict, at, m);

    pict->sig = current_kernel_time();
    d = (pict->sig.tv_nsec - ls.tv_nsec);
    printk("Brodge taked : ");
    if ( d < 0 ) {
        printk("Brodge taked : %ld.%lds.\n", pict->sig.tv_sec - ls.tv_sec - 1, (1000000000 + d)/1000000);
    } else {
        printk("Brodge taked : %ld.%lds.\n", pict->sig.tv_sec - ls.tv_sec, d/1000000);
    }

    // Sig

    // Export
    d = a051_data_write(pict->env, pict->picture, pict->size);
    printk("Brodge  ' %lX '  send %ld bytes\n", 0x7F1FFFFFFF & (1L * pict->sig.tv_sec * 1000L + pict->sig.tv_nsec / 1000000L), d);
}

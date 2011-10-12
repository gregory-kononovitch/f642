/*
 * p643_brdg.c
 *
 * Date Oct 12, 2011
 * Author and copyright (C) 2011 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include <math.h>

#include <f051_profile.h>

#include "p643_brdg.h"

#define C642_NUM_AT 3


uint16_t pas_mask[16] = {
     0                  // unused
    ,1                  // unused
    ,(1 <<  2) -1       // to small (4)
    ,(1 <<  3) -1       // 8
    ,(1 <<  4) -1       // 16
    ,(1 <<  5) -1       // 32
    ,(1 <<  6) -1       // 64
    ,(1 <<  7) -1       // 128
    ,(1 <<  8) -1       // 256
    ,(1 <<  9) -1       // 512
    ,(1 << 10) -1       // 1024
    ,(1 << 11) -1       // 2048
    ,(1 << 12) -1       // 4096
    ,(1 << 13) -1       // (8192)
    ,(1 << 14) -1       // (16384)
    ,(1 << 15) -1       // (32768)
};

struct _point_ {
    int x;
    int y;
};
typedef struct _point_ cpoint;

struct c642_pict {
    int     width;
    int     height;
    int     size;

    int     num_at;
    cpoint  P[C642_NUM_AT][3];

    int     num_fc;
    uint8_t F[C642_NUM_AT][3];

    uint8_t *picture;
    struct timeval sig;

    struct f051_log_env *a051_env;
};

//
uint8_t hu_random(void) {
    return rand() >> 23;
}

///*
// *             //////////////////////////////////////
// */
int hu_dist(const cpoint *p1, const cpoint *p2) {
    return sqrt( (p2->x - p1->x)*(p2->x - p1->x) + (p2->y - p1->y)*(p2->y - p1->y) );
}

int hu_abs(int d) {
    return d > 0 ? d : -d;
}

int hu_random2(int d1, int d2) {
    return rand() < (RAND_MAX / 2) ? d1 : d2;
}

int hu_random3(int d1, int d2, int d3) {
    int r = rand();
    return  r < 0x2AAAAAAA ? d1 : r < 0x55555554 ? d2 : d3;
}

int hu_random2m(int d1, int d2, int m1, int m2, int m) {
    if (m1 && m2 ) {
        return (rand() < ((0x7FFFFFFFL << m1) >> m)) ? d1 : d2;
    } else if (m1) {
        return d1;
    } else if (m2) {
        return m2;
    }
    return  0;
}
//
int hu_max2(int d1, int d2) {
    if ( d1 > d2 ) return d1;
    return d2;
}

int hu_max3(int d1, int d2, int d3) {
    if ( d1 > d2 ) {
        if ( d1 > d3 ) return d1;
        return d3;
    } else {
        if ( d2 > d3 ) return d2;
        return d3;
    }
}

int hu_moy2(int d1, int d2) { return (d1+d2) >> 1; }
int hu_moy2m(int d1, int d2, int m1, int m2, int m) {
    if (m1 && m2) return ( (d1 << m1) + (d2 << m2) ) >> m;
    else if (m1) return d1;
    else if (m2) return d2;
    return 0;
}

uint8_t hu_moy2mp(int d1, int d2, int m1, int m2, int m) {
    if ( m1 && m2 ) return ( (d1 << m1) + (d2 << m2) ) >> m;
    else if ( m1 ) return d1;
    else if ( m2 ) return d2;
    return 0;
}

////
int hu_imin(uint8_t *c) {
    if ( c[0] < c[1] ) {
        if (c[0] < c[2] ) return 0;
        return 2;
    } else {
        if (c[1] < c[2] ) return 1;
        return 2;
    }
}
int hu_imax_(uint8_t *c) {
    if (c[0] > c[1] ) {
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
uint8_t osc2(cpoint *p0, cpoint *p, int pas) {
//    printf("OSC2: dist (%d,%d) - (%d,%d) = %d | pas %d (%d) | %d | %d | = %d\n"
//            , p->x, p->y, p0->x, p0->y
//            , hu_dist(p0, p)
//            , pas , 1 << pas
//            , (hu_dist(p0, p) & pas_mask[pas])
//            , (hu_dist(p0, p) & pas_mask[pas]) << 8
//            , ( (hu_dist(p0, p) & pas_mask[pas]) << 8 ) >> pas
//    );
    return ( (hu_dist(p0, p) & pas_mask[pas]) << 8 ) >> pas;
}
////long osc3(cpoint p0, cpoint p, long pas) {
////    long dx = (p.x - p0.x); if (dx<0) dx = -dx;
////    dx = dx/pas - (int)(dx/pas);
////    long dy = (p.y - p0.y); if (dy<0) dy = -dy;
////    dy = dy/pas - (int)(dy/pas);
////    return (0.2 + max(dx,dy)) / 1.2;
////}
uint8_t osc4(cpoint *p0, cpoint *p, int pas) {
    return ( (hu_max2(hu_abs(p->x - p0->x), hu_abs(p->y - p0->y)) & pas_mask[pas]) << 8 ) >> pas;
}


struct c642_pict *c642_create(int width, int height)
{
    struct c642_pict *pict;

    pict = malloc(sizeof(struct c642_pict));
    if (!pict) {
        printf("Memo alloc ko\n");
        return NULL;
    }
    pict->size   = 3 * width * height;
    pict->width  = width;
    pict->height = height;
    pict->num_at = C642_NUM_AT;
    pict->num_fc = C642_NUM_AT;

    //
    pict->picture = malloc(pict->size);
    if (!pict->picture) {
        printf("Memo alloc ko\n");
        free(pict);
        return NULL;
    }

    //
    pict->a051_env = f051_init_data_env("bird", "brodge", 3 * pict->size);
    if (!pict->a051_env) {
        printf("Proc env alloc ko\n");
        free(pict->picture);
        free(pict);
        return NULL;
    }

    printf("Struct pict created\n");
    return pict;
}
//
void c642_destroy(struct c642_pict *pict)
{
    f051_end_env(pict->a051_env);
    free(pict->picture);
    free(pict);
}
//
void c642_esqu(struct c642_pict *pict)
{
    int c, n;
    uint8_t r;

    for(c = 0 ; c < 3 ; c++) {
        for(n = 0 ; n < pict->num_at ; n++) {
            r = hu_random();
            if ( r & 0x80 ) pict->P[n][c].x = ( 3 * pict->width  * r ) >> 8;
            else pict->P[n][c].x = -( 3 * pict->width  * r ) >> 8;

            r = hu_random();
            if ( r & 0x80 ) pict->P[n][c].y = ( 3 * pict->height * r ) >> 8;
            else pict->P[n][c].y = -( 3 * pict->height  * r ) >> 8;

            printf("P%d %5d  %5d | ", n, pict->P[n][c].x, pict->P[n][c].y);
        }
        for(n = 0 ; n < pict->num_fc ; n++) {
            pict->F[n][c]   = (hu_random() >> 5) + 3;
            printf("F%d %2u | ", n, pict->F[n][c]);
        }
        printf("\n");
    }
}
//
uint8_t coln1(struct c642_pict *pict, int c, cpoint *p, int *at, int *m)
{
    return hu_moy2m(
              osc2(&pict->P[at[0]][c], p, pict->F[at[0]][c])
            , osc4(&pict->P[at[1]][c], p, pict->F[at[1]][c])
            , m[0], m[1], m[2]
    );
}

void brodge1(struct c642_pict *pict, int *at, int *m)
{
    uint8_t *im;
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

            //printf("COL(%d,%d) = %3u  ,  %3u  ,  %3u\n", x, y, *(im-3), *(im-2), *(im-1));
        }
    }
}


void brodge(struct c642_pict *pict)
{
    int at[2], m[3];
    long d;
    struct timeval ls;

    gettimeofday(&ls, NULL);

    c642_esqu(pict);

    //°\\ 0: osc1 , 1: osc2 , 2: osc3
    at[0] = 0;  //hu_random() & 0x01;
    at[1] = 1;  //hu_random() & 0x01;

    m[0]  = 1;
    m[1]  = 0;
    m[2]  = 1;
//    m[0]  = hu_random() >> 6;
//    m[1]  = hu_random() >> 6;
//    m[2]  = m[0] + m[1];
//    if (!m[2]) {
//        m[ hu_random() & 0x01 ] = 1;
//        m[2] = 1;
//    }

    printf("at[0] = %d , at[1] = %d | m[0] = %d , m[1] = %d , m[2] = %d\n",
            at[0], at[1], m[0], m[1], m[2]
    );

    brodge1(pict, at, m);

    gettimeofday(&pict->sig, NULL);
    d = (pict->sig.tv_usec - ls.tv_usec);
    printf("Brodge taked : ");
    if ( d < 0 ) {
        printf("%ld.%lds.\n", pict->sig.tv_sec - ls.tv_sec - 1, (1000000 + d)/1000);
    } else {
        printf("%ld.%lds.\n", pict->sig.tv_sec - ls.tv_sec, d/1000);
    }

    // Sig

    // Export
    d = f051_send_data(pict->a051_env, pict->picture, pict->size);
    printf("Brodge  ' %lX '  send %ld bytes\n", 0x7F1FFFFFFF & (1L * pict->sig.tv_sec * 1000L + pict->sig.tv_usec / 1000L), d);
}


int main( int argc , char *argv[] )
{
    struct c642_pict *pict;

    //
    pict = c642_create(700, 350);
    if (!pict) {
        printf("Pict not created, exiting");
        return -1;
    }

    //
    int n = 1;
    while( n-- ) {
        brodge(pict);
        //usleep(1000 * 1000);
    }

    //
    c642_destroy(pict);

    //
    return 0;
}

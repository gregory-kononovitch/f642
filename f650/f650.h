/*
 * file    : f650.h
 * project : f640
 *
 * Created on: Aug 28, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#ifndef F650_H_
#define F650_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>

#include <math.h>

#include "f650_fonts.h"

#define ASM650

typedef struct {
    uint32_t    *data;          // 0
    int16_t     width;          // 8
    int16_t     height;         // 10
    int32_t     size;           // 12
    //
    double      x0;             // 16
    double      y0;             // 24
    double      sx;             // 32
    double      sy;             // 40
} bgra650;

typedef struct {
    uint32_t    *data;          // 0
    int16_t     width;          // 8
    int16_t     height;         // 10
    int32_t     size;           // 12
    //
    double      x0;             // 16
    double      y0;             // 24
    double      sx;             // 32
    double      sy;             // 40
    //
    double      *zi;            // 48
    void        *res;           // 64
} bgraz650;

// init
int  bgra_alloc650(bgra650 *img, int width, int height);
int  bgraz_alloc650(bgraz650 *img, int width, int height);
void bgra_origin650(bgra650 *img, double x0, double y0);
void bgra_scale650(bgra650 *img, double sx, double sy);

// Util
int bgra_compare650(bgra650 *img1, bgra650 *img2);

//
void bgra_clear650(bgra650 *img);
void bgra_gray650(bgra650 *img, uint8_t gray);
void bgra_fill650(bgra650 *img, uint32_t color);
void bgraz_gray650(bgraz650 *img, uint8_t gray);

//
long asm_tst1650(bgra650 *img, double x1, double y1, double x2, double y2, uint32_t color);

// Draw point
long draw_point2a650(bgra650 *img, double x1, double y1, uint32_t color);
long draw_char2a650(bgra650 *img, double x1, double y1, font650 *font, int c, uint32_t color);


// Draw line
int draw_linea650(bgra650 *img, double x1, double y1, double x2, double y2, uint32_t color);
long draw_line2a650(bgra650 *img, double x1, double y1, double x2, double y2, uint32_t color);
long draw_line3a650(bgra650 *img, double x1, double y1, double x2, double y2, uint32_t color);
int draw_linef650(bgra650 *img, double x1, double y1, double x2, double y2, uint32_t color);
#ifdef ASM650
//#define draw_line650(img, x1, y1, x2, y2, c) do {                   \
//    printf("Line (%.6f, %.6f)-(%.6f, %.6f)\n", x1, y1, x2, y2);     \
//    draw_line2a650(img, x1, y1, x2, y2, c);                         \
//    } while(0)
#define draw_line650(img, x1, y1, x2, y2, c) draw_line2a650(img, x1, y1, x2, y2, c)
#else
#define draw_line650(img, x1, y1, x2, y2, c) draw_linef650(img, x1, y1, x2, y2, c)
#endif

// Draw zline
int draw_zlinea650(bgraz650 *img, double x1, double y1, double z1, double x2, double y2, double z2, uint32_t color);
int draw_zlinef650(bgraz650 *img, double x1, double y1, double z1, double x2, double y2, double z2, uint32_t color);
#ifdef ASM650
#define draw_zline650(img, x1, y1, z1, x2, y2, z2, c) draw_zlinea650(img, x1, y1, z1, x2, y2, z2, c)
#else
#define draw_zline650(img, x1, y1, z1, x2, y2, z2, c) draw_zlinef650(img, x1, y1, z1, x2, y2, z2, c)
#endif

/*
 *
 */
#define BLACK650    0xff000000
#define WHITE650    0xffffffff
#define RED650      0xffff0000
#define GREEN650    0xff00ff00
#define BLUE650     0xff0000ff
#define ORANGE650   0xffff7f00
#define YELLOW650   0xffffff00
#define CYAN650     0xff00ffff
#define MAGENTA650  0xffff00ff

#define DGRAY650    0xff404040
#define GRAY650     0xff808080
#define LGRAY650    0xffC0C0C0

typedef union {
    uint32_t value;
    struct {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
        uint8_t alpha;
    };
} color650;

typedef union _vect650_ vect650;

union _vect650_ {
    struct {
        double x;
        double y;
        double z;
        double t;
    };
    struct {
        double array[4];
    };
    struct {
        double r;
        double a;
        double z;
        double t;
    } cyl;
    struct {
        double x;
        double y;
        double z;
        vect650 *next;
    } nist;
    struct {
        double x1;
        double y1;
        double x2;
        double y2;
    } seg2d;
    struct {
        double x;
        double y;
        double z;
        vect650 *u;
    } line;
    struct {
        double x;
        double y;
        double z;
        vect650 *n;
    } plan;
};

extern vect650 O650;
extern vect650 I650;
extern vect650 J650;
extern vect650 K650;

#define set650(u, x, y, z) do{ (u)->x = x ; (u)->y = y ; (u)->z = z; } while(0)
#define dump650(s1, u, s2) do {printf(s1 "(%.6f, %.6f)" s2, (u)->x, (u)->y);} while(0)

void random650(vect650 *u);
void turn2d650(vect650 *u, double rad);

double norma650(vect650 *u);
double normf650(vect650 *u);
#ifdef ASM650
#define norm650(u) norma650(u)
#else
#define norm650(u) normf650(u)
#endif

double unita650(vect650 *u);
double unitf650(vect650 *u);
#ifdef ASM650
#define unit650(u) unita650(u)
#else
#define unit650(u) unitf650(u)
#endif

void adda650(vect650 *u, vect650 *v);
void addf650(vect650 *u, vect650 *v);
#ifdef ASM650
#define add650(u) adda650(u)
#else
#define add650(u) addf650(u)
#endif

void suba650(vect650 *u, vect650 *v);
void subf650(vect650 *u, vect650 *v);
#ifdef ASM650
#define sub650(u) suba650(u)
#else
#define sub650(u) subf650(u)
#endif

void mul_and_adda650(vect650 *u, double a, vect650 *v);
void mul_and_addf650(vect650 *u, double a, vect650 *v);
#ifdef ASM650
#define mul_and_add650(u, a, v) mul_and_adda650(u, a, v)
#else
#define mul_and_add650(u, a, v) mul_and_addf650(u, a, v)
#endif

void mul_and_suba650(vect650 *u, double a, vect650 *v);
void mul_and_subf650(vect650 *u, double a, vect650 *v);
#ifdef ASM650
#define mul_and_sub650(u, a, v) mul_and_suba650(u, a, v)
#else
#define mul_and_sub650(u, a, v) mul_and_subf650(u, a, v)
#endif

double scala650(vect650 *u, vect650 *v);
double scal2a650(vect650 *u, vect650 *v);
double scalf650(vect650 *u, vect650 *v);
#ifdef ASM650
#define scal650(u, v) scala650(u, v)
#else
#define scal650(u, v) scalf650(u, v)
#endif

double vecta650(vect650 *u, vect650 *v, vect650 *w);
double vectf650(vect650 *u, vect650 *v, vect650 *w);
#ifdef ASM650
#define vect650(u, v, w) vecta650(u, v, w)
#else
#define vect650(u, v, w) vectf650(u, v, w)
#endif


//
typedef union {
    struct {
        vect650 origin;
        vect650 xAxis;
        vect650 yAxis;
        vect650 zAxis;
    };
    struct {
        double array[16];
    };
} ref650;

vect650 *change_vecta650(ref650 *ref, vect650 *u);
vect650 *change_vectf650(ref650 *ref, vect650 *u);
#ifdef ASM650
#define change_vect650(ref, u) change_vecta650(ref, u)
#else
#define change_vect650(ref, u) change_vectf650(ref, u)
#endif

vect650 *change_pointa650(ref650 *ref, vect650 *p);
vect650 *change_pointf650(ref650 *ref, vect650 *p);
#ifdef ASM650
#define change_point650(ref, u) change_pointa650(ref, u)
#else
#define change_point650(ref, u) change_pointf650(ref, u)
#endif


/*
 *
 */
typedef struct {
    ref650 ecran;
    double foc;     // focale
//    double tiq;     // point
//    double res;     // pix / m (ex 1600 / fac)
    double coef;    // (tiq = -foc * len / (len - foc)  |  = tiq * 1600 / fac = tiq * res
} persp650;

void setup_persp650(persp650 *cam, double foc, double res);

vect650 *compute_pixa650(persp650 *cam, vect650 *rea, vect650 *pix);
vect650 *compute_pixf650(persp650 *cam, vect650 *rea, vect650 *pix);
#ifdef ASM650
#define compute_pix650(cam, r, p) compute_pixa650(cam, r, p)
#else
#define compute_pix650(cam, r, p) compute_pixf650(cam, r, p)
#endif


/*
 *      fb0
 */
typedef struct {
    void    *start;
    int     fd;
    int     len;
} fb650;

fb650   *fb_open650();
void    fb_close650(fb650 **fb);
int     fb_draw650(fb650 *fb, bgra650 *img);

// asm
int64_t ReadTSC();


/*
 * asm tests
 */
double fx(double x);

// Sqrt !! longer
double sqrta050(double val);
#ifdef ASM650
#define sqrt650(x) sqrt(x)
#else
#define sqrt650(x) sqrt(x)
#endif

double sqrta050f(double val, void *p);
// Trig
double cosa050(double rad);
#ifdef ASM650
#define cos650(x) cosa050(x)
#else
#define cos650(x) cos(x)
#endif

float cosa050f(float rad);
//
double sina050(double rad);
#ifdef ASM650
#define sin650(x) sina050(x)
#else
#define sin650(x) sin(x)
#endif

float sina050f(float rad);
// return sin
double cossina050(double rad, double *res);
#ifdef ASM650
#define cossin650(x, res) cossina050(x, res)
#else
#define cossin650(x, res)  cossina050(x, res)
#endif

float cossina050f(float rad, float *res);

// PI / digit
void *adda050(char *number1, char *number2, int len);
void *mula050(char *number1, int len1, uint32_t fac);
void *diva050(char *number1, int len1, uint32_t div);
// PI / ulong
void *addla050(uint64_t *number1, char *uint64_t, int len);
void *mulla050(uint64_t *number1, int len1, uint64_t fac);
void *divla050(uint64_t *number1, int len1, uint64_t div);

//
int ax2bxca650(double a, double b, double c, double *res);
int ax2bxca650f(float a, float b, float c, float *res);


//
double polya650(double *coe, int len, double x);
double poly2a650(double *coe, int len2, double x);




/*
 *
 */
void *memseta650(void *align_x16, uint32_t col, size_t size);

#endif /* F650_H_ */

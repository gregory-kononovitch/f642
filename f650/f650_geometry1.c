/*
 * file    : f650_geometry1.c
 * project : f640
 *
 * Created on: Aug 28, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#include "f650.h"

vect650 O650 = {0.0, 0.0, 0.0, 0.0};
vect650 I650 = {1.0, 0.0, 0.0, 0.0};
vect650 J650 = {0.0, 1.0, 0.0, 0.0};
vect650 K650 = {0.0, 0.0, 1.0, 0.0};

void random650(vect650 *u) {
    u->x = 2. * ( 0.5 - 1. * rand() / RAND_MAX);
    u->y = 2. * ( 0.5 - 1. * rand() / RAND_MAX);
    u->z = 2. * ( 0.5 - 1. * rand() / RAND_MAX);
}

void turn2d650(vect650 *u, double rad) {
    u->x = cos(rad);
    u->y = sin(rad);
}


double normf650(vect650 *u) {
    return sqrt(u->x * u->x + u->y * u->y + u->z * u->z);
}

double unitf650(vect650 *u) {
    double n = sqrt(u->x * u->x + u->y * u->y + u->z * u->z);
    if (n == 0) return 0;
    n = 1. / n;
    u->x *= n;
    u->y *= n;
    u->z *= n;
    return n;
}

void addf650(vect650 *u, vect650 *v) {
    u->x += v->x;
    u->y += v->y;
    u->z += v->z;
}

void subf650(vect650 *u, vect650 *v) {
    u->x -= v->x;
    u->y -= v->y;
    u->z -= v->z;
}

void mulf650(vect650 *u, double a) {
    u->x *= a;
    u->y *= a;
    u->z *= a;
}


void mul_and_addf650(vect650 *u, double a, vect650 *v) {
    u->x += a * v->x;
    u->y += a * v->y;
    u->z += a * v->z;
}

void mul_and_subf650(vect650 *u, double a, vect650 *v) {
    u->x -= a * v->x;
    u->y -= a * v->y;
    u->z -= a * v->z;
}


double scalf650(vect650 *u, vect650 *v) {
    return u->x * v->x + u->y * v->y + u->z * v->z;
}

double vectf650(vect650 *u, vect650 *v, vect650 *w) {
    w->x = u->y * v->z - u->z * v->y;
    w->y = u->z * v->x - u->x * v->z;
    w->z = u->x * v->y - u->y * v->x;
    return w->z;
}

vect650 *change_vectf650(ref650 *ref, vect650 *u) {
    double x = ref->xAxis.x * u->x + ref->xAxis.y * u->y + ref->xAxis.z * u->z;
    double y = ref->yAxis.x * u->x + ref->yAxis.y * u->y + ref->yAxis.z * u->z;
    double z = ref->zAxis.x * u->x + ref->zAxis.y * u->y + ref->zAxis.z * u->z;
    u->x = x;
    u->y = y;
    u->z = z;
    return u;
}

vect650 *change_pointf650(ref650 *ref, vect650 *p) {
    p->x -= ref->origin.x;
    p->y -= ref->origin.y;
    p->z -= ref->origin.z;
    double x = ref->xAxis.x * p->x + ref->xAxis.y * p->y + ref->xAxis.z * p->z;
    double y = ref->yAxis.x * p->x + ref->yAxis.y * p->y + ref->yAxis.z * p->z;
    double z = ref->zAxis.x * p->x + ref->zAxis.y * p->y + ref->zAxis.z * p->z;
    p->x = x;
    p->y = y;
    p->z = z;
    return p;
}


/*
 *
 */
void setup_persp650(persp650 *cam, double foc, double res) {
    cam->coef = -foc * res;     // infi
}

vect650 *compute_pixf650(persp650 *cam, vect650 *rea, vect650 *pix) {
    pix->x = cam->ecran.origin.x - rea->x;
    pix->y = cam->ecran.origin.y - rea->y;
    pix->z = cam->ecran.origin.z - rea->z;
    double pixz = cam->ecran.zAxis.x * pix->x + cam->ecran.zAxis.y * pix->y + cam->ecran.zAxis.z * pix->z;
    if (pixz >= 0) {
        pix->x = 0 ; pix->y = 0 ; pix->z = 0;
        return pix;
    }
    pixz  = cam->coef / pixz;
    double pixx = cam->ecran.xAxis.x * pix->x + cam->ecran.xAxis.y * pix->y + cam->ecran.xAxis.z * pix->z;
    pix->y = pixz * (cam->ecran.yAxis.x * pix->x + cam->ecran.yAxis.y * pix->y + cam->ecran.yAxis.z * pix->z);
    pix->x = pixz * pixx;
    pix->z = -pix->z;
}


/*
 *  FB
 */
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
fb650 *fb_open650() {
    int fd = open("/dev/fb0", O_RDWR);
    fb650 *fb = calloc(1, sizeof(fb650));
    if (!fb) return NULL;
    struct fb_fix_screeninfo fix;
    memset(&fix, 0, sizeof(fix));
    int i = ioctl(fd, FBIOGET_FSCREENINFO, &fix);
    if (i < 0) return NULL;
    fb->start = mmap(NULL, fix.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (fb->start == MAP_FAILED) {
     printf("MMAP failed, exiting\n");
     return NULL;
    }
    fb->len = fix.smem_len;
    return fb;
}

void fb_close650(fb650 **fb) {
    munmap((*fb)->start, (*fb)->len);
    free(*fb);
    *fb = NULL;
}

int fb_draw650(fb650 *fb, bgra650 *img) {
    memcpy(fb->start, img->data, img->size << 2);
    return 0;
}

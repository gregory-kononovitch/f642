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


void random650(vect650 *u) {
    u->x = 2. * ( 0.5 - 1. * rand() / RAND_MAX);
    u->y = 2. * ( 0.5 - 1. * rand() / RAND_MAX);
    u->z = 2. * ( 0.5 - 1. * rand() / RAND_MAX);
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

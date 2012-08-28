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

double scalf650(vect650 *u, vect650 *v) {
    return u->x * v->x + u->y * v->y + u->z * v->z;
}

double vectf650(vect650 *u, vect650 *v, vect650 *w) {
    w->x = u->y * v->z - u->z * v->y;
    w->y = u->z * v->x - u->x * v->z;
    w->z = u->x * v->y - u->y * v->x;
    return w->z;
}

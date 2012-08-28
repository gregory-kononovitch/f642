/*
 * file    : f650_asm.c
 * project : f640
 *
 * Created on: Aug 28, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#include "f650.h"


void f650_test_asm(image *img, double x) {
    int i = 100000;
    *(img->data + i) = 0;
}

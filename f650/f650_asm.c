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

static double pas = 0.65;
static uint16_t u = 18;
int f650_test_asm(uint16_t i) {
    int s = 0;
    uint16_t j = 0;

    if (s < 0) {
        s = s + i;
    }
    return j;
}

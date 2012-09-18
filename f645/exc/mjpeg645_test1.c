/*
 * file    : mjpeg645-test1.c
 * project : f640
 *
 * Created on: Sep 18, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */




#include "../mjpeg645.h"



int main() {
    int lenb = 100 * 1024;
    void *tmp = calloc(1, lenb);

    //
    FILE *filp = fopen("/home/greg/t509/u610-equa/mjpeg800x448-8.dat", "rb");
    fread(tmp, 1, lenb, filp);
    fclose(filp);

    //
    mjpeg645_img *src = alloc_mjpeg645_image(tmp, lenb);


    return 0;
}

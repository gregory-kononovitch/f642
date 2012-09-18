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
    mjpeg645_img src;
    memset(&src, 0, sizeof(mjpeg645_img));
    src.data = calloc(1, lenb);
    src.size = lenb;
    src.ptr  = src.data;

    //
    FILE *filp = fopen("/home/greg/t509/u610-equa/mjpeg800x448-8.dat", "rb");
    fread(src.data, 1, lenb, filp);
    fclose(filp);

    //


    return 0;
}

/*
 * file    : f650-test.c
 * project : f640
 *
 * Created on: Aug 27, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */



#include "f650.h"





int main() {
    int i;
    long l1, l2;
    double x = 0.1, a = 6.335544, b = -7.332;
    struct timeval tv1, tv2;

    x = a650_draw_line(NULL, 1., 1., -2., 4.5);
    printf("res = %f\n", x);


//    //
//    x = 0.1;
//    gettimeofday(&tv1, NULL);
//    l1 = ReadTSC();
//    for(i = 0 ; i < 10000000 ; i++) {
//        x = f650_f1(x, a, b);
//    }
//    l2 = ReadTSC();
//    gettimeofday(&tv2, NULL);
//    timersub(&tv2, &tv1, &tv2);
//    printf("d = %ld / %ld = %f\n", tv2.tv_usec, l2 - l1, 1e6 * (l2 - l1) / tv2.tv_usec);
//
//    //
//    x = 0.1;
//    gettimeofday(&tv1, NULL);
//    l1 = ReadTSC();
//    for(i = 0 ; i < 10000000 ; i++) {
//        x = faxpb(x, a, b);
//    }
//    l2 = ReadTSC();
//    gettimeofday(&tv2, NULL);
//    timersub(&tv2, &tv1, &tv2);
//    printf("d = %ld / %ld = %f\n", tv2.tv_usec, l2 - l1, 1e6 * (l2 - l1) / tv2.tv_usec);
//
//    //
//    x = 0.1;
//    gettimeofday(&tv1, NULL);
//    l1 = ReadTSC();
//    for(i = 0 ; i < 10000000 ; i++) {
//        x = a * x + b;
//    }
//    l2 = ReadTSC();
//    gettimeofday(&tv2, NULL);
//    timersub(&tv2, &tv1, &tv2);
//    printf("d = %ld / %ld = %.3f MHz\n", tv2.tv_usec, l2 - l1, 1. * (l2 - l1) / tv2.tv_usec);

    return 0;
}

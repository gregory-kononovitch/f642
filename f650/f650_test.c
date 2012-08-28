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
    int i, r;
    long l1, l2;
    double x = 0.1, a = 6.335544, b = -7.332;
    struct timeval tv1, tv2;

    image img;
    f650_new_image(&img, 512, 288);
    f650_img_origin(&img, 100, 200);
    f650_img_scale(&img, 1., 1.);
    img.color = -1;

//    // x' = x + 156 / y' = 344 - y      KO: x
//    t650_clear_test(&img);
//    x = a650_draw_line(&img, 356.000001, -157., 356, 4.5);
//    printf("r %f, x1 %f, y1 %f, x2 %f, y2 %f, a %f, b %f\n", x, img.x1, img.y1, img.x2, img.y2, img.a, img.b);
//    t650_clear_test(&img);
//    x = f650_draw_line(&img, 356.000001, -157., 356, 4.5);
//    printf("r %f, x1 %f, y1 %f, x2 %f, y2 %f, a %f, b %f\n", x, img.x1, img.y1, img.x2, img.y2, img.a, img.b);
//
//    printf("\n");
//
//    // x' = x + 156 / y' = 344 - y      OK:
//    t650_clear_test(&img);
//    x = a650_draw_line(&img, 200.000001, 195, 250, 204.5);
//    printf("r %f, x1 %f, y1 %f, x2 %f, y2 %f, a %f, b %f\n", x, img.x1, img.y1, img.x2, img.y2, img.a, img.b);
//    t650_clear_test(&img);
//    x = f650_draw_line(&img, 200.000001, 195, 250, 204.5);
//    printf("r %f, x1 %f, y1 %f, x2 %f, y2 %f, a %f, b %f\n", x, img.x1, img.y1, img.x2, img.y2, img.a, img.b);
//
//    printf("\n");
//
//    // x' = x + 156 / y' = 344 - y      OK:
//    t650_clear_test(&img);
//    x = a650_draw_line(&img, 250, 195, 200.000001, 204.5);
//    printf("r %f, x1 %f, y1 %f, x2 %f, y2 %f, a %f, b %f\n", x, img.x1, img.y1, img.x2, img.y2, img.a, img.b);
//    t650_clear_test(&img);
//    x = f650_draw_line(&img, 250, 195, 200.000001, 204.5);
//    printf("r %f, x1 %f, y1 %f, x2 %f, y2 %f, a %f, b %f\n", x, img.x1, img.y1, img.x2, img.y2, img.a, img.b);

    // x' = x + 156 / y' = 344 - y      OK:
    f650_img_origin(&img, 0, 0);
    t650_clear_test(&img);
    x = a650_draw_line(&img, -10, 20, 100, 30);
    printf("r %f, x1 %f, y1 %f, x2 %f, y2 %f, a %f, b %f i %d\n", x, img.x1, img.y1, img.x2, img.y2, img.a, img.b, img.index);
    printf("data[%d] = %d\n", img.index, img.data[img.index]);
    t650_clear_test(&img);
    x = f650_draw_line(&img, -10, 20, 100, 30);
    printf("r %f, x1 %f, y1 %f, x2 %f, y2 %f, a %f, b %f, i %d\n", x, img.x1, img.y1, img.x2, img.y2, img.a, img.b, img.index);


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

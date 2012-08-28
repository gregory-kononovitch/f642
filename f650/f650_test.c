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
    //
    image img1;
    f650_new_image(&img1, 512, 288);
    f650_img_origin(&img1, 100, 200);
    f650_img_scale(&img1, 1., 1.);
    img1.color = -1;
    //
    image img2;
    f650_new_image(&img2, 512, 288);
    f650_img_origin(&img2, 100, 200);
    f650_img_scale(&img2, 1., 1.);
    img2.color = -1;

//    // x' = x + 156 / y' = 344 - y      KO: x
//    t650_clear_test(&img1);
//    x = a650_draw_line(&img1, 356.000001, -157., 356, 4.5);
//    printf("r %f, x1 %f, y1 %f, x2 %f, y2 %f, a %f, b %f\n", x, img1.x1, img1.y1, img1.x2, img1.y2, img1.a, img1.b);
//    t650_clear_test(&img1);
//    x = f650_draw_line(&img1, 356.000001, -157., 356, 4.5);
//    printf("r %f, x1 %f, y1 %f, x2 %f, y2 %f, a %f, b %f\n", x, img1.x1, img1.y1, img1.x2, img1.y2, img1.a, img1.b);
//
//    printf("\n");
//
//    // x' = x + 156 / y' = 344 - y      OK:
//    t650_clear_test(&img1);
//    x = a650_draw_line(&img1, 200.000001, 195, 250, 204.5);
//    printf("r %f, x1 %f, y1 %f, x2 %f, y2 %f, a %f, b %f\n", x, img1.x1, img1.y1, img1.x2, img1.y2, img1.a, img1.b);
//    t650_clear_test(&img1);
//    x = f650_draw_line(&img1, 200.000001, 195, 250, 204.5);
//    printf("r %f, x1 %f, y1 %f, x2 %f, y2 %f, a %f, b %f\n", x, img1.x1, img1.y1, img1.x2, img1.y2, img1.a, img1.b);
//
//    printf("\n");
//
//    // x' = x + 156 / y' = 344 - y      OK:
//    t650_clear_test(&img1);
//    x = a650_draw_line(&img1, 250, 195, 200.000001, 204.5);
//    printf("r %f, x1 %f, y1 %f, x2 %f, y2 %f, a %f, b %f\n", x, img1.x1, img1.y1, img1.x2, img1.y2, img1.a, img1.b);
//    t650_clear_test(&img1);
//    x = f650_draw_line(&img1, 250, 195, 200.000001, 204.5);
//    printf("r %f, x1 %f, y1 %f, x2 %f, y2 %f, a %f, b %f\n", x, img1.x1, img1.y1, img1.x2, img1.y2, img1.a, img1.b);

    // x' = x + 156 / y' = 344 - y      OK:
    f650_img_origin(&img1, 0, 0);
    t650_clear_test(&img1);
    x = a650_draw_line(&img1, -10, 20, 100, 30);
    printf("r %f, x1 %f, y1 %f, x2 %f, y2 %f, a %f, b %f i %d\n", x, img1.x1, img1.y1, img1.x2, img1.y2, img1.a, img1.b, img1.index);
    printf("data[%d] = %d\n", img1.index, img1.data[img1.index]);

    f650_img_origin(&img2, 0, 0);
    t650_clear_test(&img2);
    x = f650_draw_line(&img2, -10, 20, 100, 30);
    printf("r %f, x1 %f, y1 %f, x2 %f, y2 %f, a %f, b %f, i %d\n", x, img2.x1, img2.y1, img2.x2, img2.y2, img2.a, img2.b, img2.index);

    printf("img1 / img2 = %d\n", f650_img_compare(&img1, &img2));

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

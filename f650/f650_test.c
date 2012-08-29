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





int test_line650() {
    int i, r;
    long l1, l2;
    double x = 0.1, a = 6.335544, b = -7.332;
    struct timeval tv1, tv2;
    //
    bgra650 img1;
    f650_alloc_image(&img1, 512, 288);
    f650_img_origin(&img1, 100, 200);
    f650_img_scale(&img1, 1., 1.);
    //
    bgra650 img2;
    f650_alloc_image(&img2, 512, 288);
    f650_img_origin(&img2, 100, 200);
    f650_img_scale(&img2, 1., 1.);

//    // x' = x + 156 / y' = 344 - y      KO: x
//    f650_img_clear(&img1);
//    x = draw_linea650(&img1, 356.000001, -157., 356, 4.5);
//    printf("r %f, x1 %f, y1 %f, x2 %f, y2 %f, a %f, b %f\n", x, img1.x1, img1.y1, img1.x2, img1.y2, img1.a, img1.b);
//    f650_img_clear(&img1);
//    x = draw_linef650(&img1, 356.000001, -157., 356, 4.5);
//    printf("r %f, x1 %f, y1 %f, x2 %f, y2 %f, a %f, b %f\n", x, img1.x1, img1.y1, img1.x2, img1.y2, img1.a, img1.b);
//
//    printf("\n");
//
//    // x' = x + 156 / y' = 344 - y      OK:
//    f650_img_clear(&img1);
//    x = draw_linea650(&img1, 200.000001, 195, 250, 204.5);
//    printf("r %f, x1 %f, y1 %f, x2 %f, y2 %f, a %f, b %f\n", x, img1.x1, img1.y1, img1.x2, img1.y2, img1.a, img1.b);
//    f650_img_clear(&img1);
//    x = draw_linef650(&img1, 200.000001, 195, 250, 204.5);
//    printf("r %f, x1 %f, y1 %f, x2 %f, y2 %f, a %f, b %f\n", x, img1.x1, img1.y1, img1.x2, img1.y2, img1.a, img1.b);
//
//    printf("\n");
//
//    // x' = x + 156 / y' = 344 - y      OK:
//    f650_img_clear(&img1);
//    x = draw_linea650(&img1, 250, 195, 200.000001, 204.5);
//    printf("r %f, x1 %f, y1 %f, x2 %f, y2 %f, a %f, b %f\n", x, img1.x1, img1.y1, img1.x2, img1.y2, img1.a, img1.b);
//    f650_img_clear(&img1);
//    x = draw_linef650(&img1, 250, 195, 200.000001, 204.5);
//    printf("r %f, x1 %f, y1 %f, x2 %f, y2 %f, a %f, b %f\n", x, img1.x1, img1.y1, img1.x2, img1.y2, img1.a, img1.b);

    // x' = x + 156 / y' = 344 - y      OK:
    f650_img_origin(&img1, 0, 0);
    f650_img_clear(&img1);
    l1 = ReadTSC();
    x = draw_linea650(&img1, 100, 20, -10, 30, WHITE650);
    l2 = ReadTSC();
    printf("tsca = %ld\n", l2 - l1);

    f650_img_origin(&img2, 0, 0);
    f650_img_clear(&img2);
    l1 = ReadTSC();
    x = draw_linef650(&img2, 100, 20, -10, 30, WHITE650);
    l2 = ReadTSC();
    printf("tscc = %ld\n", l2 - l1);

    printf("img1 / img2 = %d\n", f650_img_compare(&img1, &img2));

    //////////////////////////////////////////////
    // x' = x + 156 / y' = 344 - y      OK:
    f650_img_origin(&img1, 0, 0);
    f650_img_clear(&img1);
    l1 = ReadTSC();
    x = draw_linea650(&img1, -10, 20, -30, 30, WHITE650);
    l2 = ReadTSC();
    printf("tsca = %ld\n", l2 - l1);

    f650_img_origin(&img2, 0, 0);
    f650_img_clear(&img2);
    l1 = ReadTSC();
    x = draw_linef650(&img2, -10, 20, -30, 30, WHITE650);
    l2 = ReadTSC();
    printf("tscc = %ld\n", l2 - l1);

    printf("img1 / img2 = %d\n", f650_img_compare(&img1, &img2));


    //////////////////////////////////////////////
    // x' = x + 156 / y' = 344 - y      OK:
    f650_img_origin(&img1, 0, 0);
    f650_img_clear(&img1);
    l1 = ReadTSC();
    x = draw_linea650(&img1, -10, 20, -30, 60, WHITE650);
    l2 = ReadTSC();
    printf("tsca = %ld\n", l2 - l1);

    f650_img_origin(&img2, 0, 0);
    f650_img_clear(&img2);
    l1 = ReadTSC();
    x = draw_linef650(&img2, -10, 20, -30, 60, WHITE650);
    l2 = ReadTSC();
    printf("tscc = %ld\n", l2 - l1);

    printf("img1 / img2 = %d\n", f650_img_compare(&img1, &img2));

    /*
     *
     */
    gettimeofday(&tv1, NULL);
    l1 = ReadTSC();
    for(i = 0 ; i < 200000 ; i++) {
        draw_linea650(&img1
                , 1. * rand() / INT32_MAX * img1.width
                , 1. * rand() / INT32_MAX * img1.height
                , 1. * rand() / INT32_MAX * img1.width
                , 1. * rand() / INT32_MAX * img1.height
                , rand()
        );
    }
    l2 = ReadTSC();
    gettimeofday(&tv2, NULL);
    timersub(&tv2, &tv1, &tv2);
    printf("Last %lus.%06luµs for %ld µops\n", tv2.tv_sec, tv2.tv_usec, l2 - l1);

    /*
     *
     */
    gettimeofday(&tv1, NULL);
    l1 = ReadTSC();
    for(i = 0 ; i < 200000 ; i++) {
        draw_linef650(&img1
                , 1. * rand() / RAND_MAX * img1.width
                , 1. * rand() / RAND_MAX * img1.height
                , 1. * rand() / RAND_MAX * img1.width
                , 1. * rand() / RAND_MAX * img1.height
                , rand()
        );
    }
    l2 = ReadTSC();
    gettimeofday(&tv2, NULL);
    timersub(&tv2, &tv1, &tv2);
    printf("Last %lus.%06luµs for %ld µops\n", tv2.tv_sec, tv2.tv_usec, l2 - l1);


    /*
     *
     */
    f650_img_origin(&img1, 0, 0);
    f650_img_clear(&img1);
    f650_img_origin(&img2, 0, 0);
    f650_img_clear(&img2);
    long cpt = 0;

    gettimeofday(&tv1, NULL);
    l1 = ReadTSC();
    for(i = 0 ; i < 200000 ; i++) {
        double x1 = 1. * (0.5 - 1. * rand() / RAND_MAX) * img1.width;
        double y1 = 1. * (0.5 - 1. * rand() / RAND_MAX) * img1.height;
        double x2 = 1. * (0.5 - 1. * rand() / RAND_MAX) * img1.width;
        double y2 = 1. * (0.5 - 1. * rand() / RAND_MAX) * img1.height;

        cpt += draw_linea650(&img1, x1, y1, x2, y2, WHITE650);
        draw_linef650(&img2, x1, y1, x2, y2, WHITE650);
    }
    l2 = ReadTSC();
    gettimeofday(&tv2, NULL);
    timersub(&tv2, &tv1, &tv2);
    printf("Last %lus.%06luµs for %ld µops : img1 / img2 = %d for %ld pixels\n", tv2.tv_sec, tv2.tv_usec, l2 - l1, f650_img_compare(&img1, &img2), cpt);

    return 0;
}



int test_geo1() {
    int i;
    long l1, l2;
    double d;
    struct timeval tv1, tv2;
    vect650 u, v, w;

    //
    u.x = 12.443 ; u.y = 6.33234 ; u.z = -8.733;
    v.x = 1.683  ; v.y = -6.3434 ; v.z = -2.733;
    w.x = -1.443 ; w.y = 0.9034  ; w.z = 5.733;
    //
    printf("|u| = %f / %f\n", norma650(&u), normf650(&u));
    printf("|v| = %f / %f\n", norma650(&v), normf650(&v));
    printf("|w| = %f / %f\n", norma650(&w), normf650(&w));
    //
    l1 = ReadTSC();
    d = norma650(&u);
    l2 = ReadTSC();
    printf("A norm : %ld\n", l2 - l1);
    //
    l1 = ReadTSC();
    d = normf650(&u);
    l2 = ReadTSC();
    printf("C norm : %ld\n", l2 - l1);

    //
    l1 = ReadTSC();
    d = unita650(&u);
    l2 = ReadTSC();
    printf("A unit : %ld = %f\n", l2 - l1, u.x);
    //
    l1 = ReadTSC();
    d = unitf650(&u);
    l2 = ReadTSC();
    printf("C unit : %ld = %f\n", l2 - l1, u.x);

//    //
//    gettimeofday(&tv1, NULL);
//    l1 = ReadTSC();
//    for(i = 0 ; i < 1000000 ; i++) {
//        random650(&u);
//        d = unita650(&u);
//    }
//    l2 = ReadTSC();
//    gettimeofday(&tv2, NULL);
//    timersub(&tv2, &tv1, &tv2);
//    printf("A unit = %lds.%06luµs for %ld\n", tv2.tv_sec, tv2.tv_usec, l2 - l1);
//
//    //
//    gettimeofday(&tv1, NULL);
//    l1 = ReadTSC();
//    for(i = 0 ; i < 1000000 ; i++) {
//        random650(&u);
//        d = unitf650(&u);
//    }
//    l2 = ReadTSC();
//    gettimeofday(&tv2, NULL);
//    timersub(&tv2, &tv1, &tv2);
//    printf("C unit = %lds.%06luµs for %ld\n", tv2.tv_sec, tv2.tv_usec, l2 - l1);

    //
    l1 = ReadTSC();
    d = scala650(&u, &v);
    l2 = ReadTSC();
    printf("A scal : %ld = %f\n", l2 - l1, d);
    //
    l1 = ReadTSC();
    d  = scalf650(&u, &v);
    l2 = ReadTSC();
    printf("C scal : %ld = %f\n", l2 - l1, d);

    //
    l1 = ReadTSC();
    d  = vecta650(&u, &v, &w);
    l2 = ReadTSC();
    printf("A vect : %ld = %f\n", l2 - l1, d);
    //
    l1 = ReadTSC();
    d  = vectf650(&u, &v, &w);
    l2 = ReadTSC();
    printf("C vect : %ld = %f\n", l2 - l1, d);
    //
    l1 = ReadTSC();
    d  = vect650(&u, &v, &w);
    l2 = ReadTSC();
    printf("? vect : %ld = %f\n", l2 - l1, d);

    return 0;
}

int test_geo2() {
    vect650 plan;
    vect650 n;

    plan.plan.n = &n;
}

int test_ref() {
    persp650 cam;
    vect650 pos, qos, p1, p2, q1, q2;

    //
    setup_persp650(&cam, 0.05, 1600. / 0.15);
    cam.ecran.origin.x = 0.5 ; cam.ecran.origin.y = -9 ; cam.ecran.origin.z = +3;
    cam.ecran.zAxis.x = -cam.ecran.origin.x;
    cam.ecran.zAxis.y = -cam.ecran.origin.y;
    cam.ecran.zAxis.z = -cam.ecran.origin.z;
    unit650(&cam.ecran.zAxis);
    vect650(&cam.ecran.zAxis, &K650, &cam.ecran.xAxis);
    unit650(&cam.ecran.xAxis);
    vect650(&cam.ecran.zAxis, &cam.ecran.xAxis, &cam.ecran.yAxis);

    //
    p1.x = 0 ; p1.y = - 0.5 ; p1.z = 0;
    compute_pix650(&cam, &p1, &q1);
    p2.x = 0 ; p2.y = - 0.5 ; p2.z = 1.05;
    compute_pix650(&cam, &p2, &q2);
    //
    printf("q1 = (%f, %f, %f)\n", q1.x, q1.y, q1.z);
    printf("q2 = (%f, %f, %f)\n", q2.x, q2.y, q2.z);
    //
    p1.x = 0 ; p1.y = - 0.5 ; p1.z = 0;
    compute_pixa650(&cam, &p1, &q1);
    p2.x = 0 ; p2.y = - 0.5 ; p2.z = 1.05;
    compute_pixa650(&cam, &p2, &q2);
    //
    printf("q1 = (%f, %f, %f)\n", q1.x, q1.y, q1.z);
    printf("q2 = (%f, %f, %f)\n", q2.x, q2.y, q2.z);

    //
    p1.x = 1. ; p1.y = -0.5 ; p1.z = 0;
    change_vectf650(&cam.ecran, &p1);
    p2.x = 1. ; p2.y = -0.5 ; p2.z = 1.05;
    change_vectf650(&cam.ecran, &p2);
    //
    printf("p1 = (%f, %f, %f)\n", p1.x, p1.y, p1.z);
    printf("p2 = (%f, %f, %f)\n", p2.x, p2.y, p2.z);
    //
    p1.x = 1. ; p1.y = -0.5 ; p1.z = 0;
    change_vecta650(&cam.ecran, &p1);
    p2.x = 1. ; p2.y = -0.5 ; p2.z = 1.05;
    change_vecta650(&cam.ecran, &p2);
    //
    printf("p1 = (%f, %f, %f)\n", p1.x, p1.y, p1.z);
    printf("p2 = (%f, %f, %f)\n", p2.x, p2.y, p2.z);
}

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
int test3() {
    int i = 0;
    bgra650 img;
    int fd = open("/dev/fb0", O_RDWR);
     struct fb_fix_screeninfo fix;
     memset(&fix, 0, sizeof(fix));
     i = ioctl(fd, FBIOGET_FSCREENINFO, &fix);
     if (i < 0) return -1;
     void *start = mmap(NULL, fix.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
     if (start == MAP_FAILED) {
         printf("MMAP failed, exiting\n");
         return -1;
     }

    double t0, t;
    struct timeval tv0, tv;
    persp650 cam;
    vect650 pos, qos, p1, p2, q1, q2;

    //
    f650_alloc_image(&img, 1024, 600);
    //
    setup_persp650(&cam, 0.05, 1600. / 0.15);
    cam.ecran.origin.x = 0 ; cam.ecran.origin.y = -9 ; cam.ecran.origin.z = +3;
    cam.ecran.zAxis.x = -cam.ecran.origin.x;
    cam.ecran.zAxis.y = -cam.ecran.origin.y;
    cam.ecran.zAxis.z = -cam.ecran.origin.z;
    unit650(&cam.ecran.zAxis);
    vect650(&cam.ecran.zAxis, &K650, &cam.ecran.xAxis);
    unit650(&cam.ecran.xAxis);
    vect650(&cam.ecran.zAxis, &cam.ecran.xAxis, &cam.ecran.yAxis);
    //

    //
    gettimeofday(&tv0, NULL);
    while(1) {
        gettimeofday(&tv, NULL);
        timersub(&tv, &tv0, &tv);
        t = 1. * tv.tv_sec + 0.000001 * tv.tv_usec;
        //
        f650_img_gray(&img, 0x40);
        i++;
        //
        pos.x = +4.5 * t;
        pos.y = -2.0;
        pos.z = +0.3;
        cam.ecran.origin.x = pos.x;
        double h = pos.x - 10. * round(pos.x / 10.);

        //
        p1.x = pos.x + h ; p1.y = pos.y - 0.5 ; p1.z = 0;
        compute_pix650(&cam, &p1, &q1);
        p2.x = pos.x + h ; p2.y = pos.y - 0.5 ; p2.z = 1.05;
        compute_pix650(&cam, &p2, &q2);
        draw_line650(&img, q1.x, q1.y, q2.x, q2.y, ORANGE650);
        p1.x = pos.x + h ; p1.y = pos.y + 0.5 ; p1.z = 1.05;
        compute_pix650(&cam, &p1, &q1);
        draw_line650(&img, q1.x, q1.y, q2.x, q2.y, ORANGE650);
        p2.x = pos.x + h ; p2.y = pos.y + 0.5 ; p2.z = 0;
        compute_pix650(&cam, &p2, &q2);
        draw_line650(&img, q1.x, q1.y, q2.x, q2.y, ORANGE650);
        //
        memcpy(start, img.data, img.size << 2);
        usleep(50000);
    }

    return 0;
}

int main() {
    //test_line650();
    //test_geo1();
    //test_ref();
    test3();
    return 0;
}

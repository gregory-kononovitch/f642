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
    bgra_alloc650(&img1, 512, 288);
    bgra_origin650(&img1, 100, 200);
    bgra_scale650(&img1, 1., 1.);
    //
    bgra650 img2;
    bgra_alloc650(&img2, 512, 288);
    bgra_origin650(&img2, 100, 200);
    bgra_scale650(&img2, 1., 1.);

//    // x' = x + 156 / y' = 344 - y      KO: x
//    bgra_clear650(&img1);
//    x = draw_linea650(&img1, 356.000001, -157., 356, 4.5);
//    printf("r %f, x1 %f, y1 %f, x2 %f, y2 %f, a %f, b %f\n", x, img1.x1, img1.y1, img1.x2, img1.y2, img1.a, img1.b);
//    bgra_clear650(&img1);
//    x = draw_linef650(&img1, 356.000001, -157., 356, 4.5);
//    printf("r %f, x1 %f, y1 %f, x2 %f, y2 %f, a %f, b %f\n", x, img1.x1, img1.y1, img1.x2, img1.y2, img1.a, img1.b);
//
//    printf("\n");
//
//    // x' = x + 156 / y' = 344 - y      OK:
//    bgra_clear650(&img1);
//    x = draw_linea650(&img1, 200.000001, 195, 250, 204.5);
//    printf("r %f, x1 %f, y1 %f, x2 %f, y2 %f, a %f, b %f\n", x, img1.x1, img1.y1, img1.x2, img1.y2, img1.a, img1.b);
//    bgra_clear650(&img1);
//    x = draw_linef650(&img1, 200.000001, 195, 250, 204.5);
//    printf("r %f, x1 %f, y1 %f, x2 %f, y2 %f, a %f, b %f\n", x, img1.x1, img1.y1, img1.x2, img1.y2, img1.a, img1.b);
//
//    printf("\n");
//
//    // x' = x + 156 / y' = 344 - y      OK:
//    bgra_clear650(&img1);
//    x = draw_linea650(&img1, 250, 195, 200.000001, 204.5);
//    printf("r %f, x1 %f, y1 %f, x2 %f, y2 %f, a %f, b %f\n", x, img1.x1, img1.y1, img1.x2, img1.y2, img1.a, img1.b);
//    bgra_clear650(&img1);
//    x = draw_linef650(&img1, 250, 195, 200.000001, 204.5);
//    printf("r %f, x1 %f, y1 %f, x2 %f, y2 %f, a %f, b %f\n", x, img1.x1, img1.y1, img1.x2, img1.y2, img1.a, img1.b);

    // x' = x + 156 / y' = 344 - y      OK:
    bgra_origin650(&img1, 0, 0);
    bgra_clear650(&img1);
    l1 = ReadTSC();
    x = draw_linea650(&img1, 100, 20, -10, 30, WHITE650);
    l2 = ReadTSC();
    printf("tsca = %ld\n", l2 - l1);

    bgra_origin650(&img2, 0, 0);
    bgra_clear650(&img2);
    l1 = ReadTSC();
    x = draw_linef650(&img2, 100, 20, -10, 30, WHITE650);
    l2 = ReadTSC();
    printf("tscc = %ld\n", l2 - l1);

    printf("img1 / img2 = %d\n", bgra_compare650(&img1, &img2));

    //////////////////////////////////////////////
    // x' = x + 156 / y' = 344 - y      OK:
    bgra_origin650(&img1, 0, 0);
    bgra_clear650(&img1);
    l1 = ReadTSC();
    x = draw_linea650(&img1, -10, 20, -30, 30, WHITE650);
    l2 = ReadTSC();
    printf("tsca = %ld\n", l2 - l1);

    bgra_origin650(&img2, 0, 0);
    bgra_clear650(&img2);
    l1 = ReadTSC();
    x = draw_linef650(&img2, -10, 20, -30, 30, WHITE650);
    l2 = ReadTSC();
    printf("tscc = %ld\n", l2 - l1);

    printf("img1 / img2 = %d\n", bgra_compare650(&img1, &img2));


    //////////////////////////////////////////////
    // x' = x + 156 / y' = 344 - y      OK:
    bgra_origin650(&img1, 0, 0);
    bgra_clear650(&img1);
    l1 = ReadTSC();
    x = draw_linea650(&img1, -10, 20, -30, 60, WHITE650);
    l2 = ReadTSC();
    printf("tsca = %ld\n", l2 - l1);

    bgra_origin650(&img2, 0, 0);
    bgra_clear650(&img2);
    l1 = ReadTSC();
    x = draw_linef650(&img2, -10, 20, -30, 60, WHITE650);
    l2 = ReadTSC();
    printf("tscc = %ld\n", l2 - l1);

    printf("img1 / img2 = %d\n", bgra_compare650(&img1, &img2));

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
    bgra_origin650(&img1, 0, 0);
    bgra_clear650(&img1);
    bgra_origin650(&img2, 0, 0);
    bgra_clear650(&img2);
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
    printf("Last %lus.%06luµs for %ld µops : img1 / img2 = %d for %ld pixels\n", tv2.tv_sec, tv2.tv_usec, l2 - l1, bgra_compare650(&img1, &img2), cpt);

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

//    //
//    l1 = ReadTSC();
//    d = scal2a650(&u, &v);
//    l2 = ReadTSC();
//    printf("A scal2 : %ld = %f\n", l2 - l1, d);

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
    long l1, l2;
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
    struct timeval tv0, tv, tv1, tv2, tvt;
    persp650 cam;
    vect650 pos, qos, p1, p2, q1, q2;

    //
    bgra_alloc650(&img, 1024, 600);
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
    random650(&p1);
    p1.x *= 10;
    p1.x += pos.x;
    p1.y *= 5;
    p1.z *= 2;
    compute_pix650(&cam, &p1, &q1);
    random650(&p2);
    p2.x *= 10;
    p2.x += pos.x;
    p2.y *= 5;
    p2.z *= 2;
    l1 = ReadTSC();
    compute_pix650(&cam, &p2, &q2);
    l2 = ReadTSC();
    printf("compute_pix650 = %ld\n", l2 - l1);

    int col = rand();
    l1 = ReadTSC();
    draw_line650(&img, q1.x, q1.y, q2.x, q2.y, col);
    l2 = ReadTSC();
    printf("draw_line650 = %ld for %d\n", l2 - l1, i);

//    return 0;
    //
    long n = 0, nb =0, nbw = 0;
    gettimeofday(&tv0, NULL);
    tv1.tv_sec  = tv0.tv_sec;
    tv1.tv_usec = tv0.tv_usec;
    while(1) {
        gettimeofday(&tv, NULL);
        timersub(&tv, &tv0, &tv);
        t = 1. * tv.tv_sec + 0.000001 * tv.tv_usec;
        //
        bgra_gray650(&img, 0x20);    // 2.5ms
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
        for(i = 0 ; i < 500 ; i++) {  // 28kHz
            random650(&p1);
            p1.x = pos.x + 10 * p1.x;
//            p1.x += pos.x - 6;
            p1.y *= 8;
            p1.z *= 2;
            compute_pix650(&cam, &p1, &q1);
            random650(&p2);
            p2.x = pos.x + 10 * p2.x;
//            p2.x += pos.x + 6;
//            p2.y *= 8;
            p2.y = -8 * p1.y;
            p2.z *= 2;
            compute_pix650(&cam, &p2, &q2);
            draw_line650(&img, q1.x, q1.y, q2.x, q2.y, rand());
        }
        //
        memcpy(start, img.data, img.size << 2);
        //
        nb += 50000;
        gettimeofday(&tv2, NULL);
        timersub(&tv2, &tv1, &tvt);
        tv1.tv_sec  = tv2.tv_sec;
        tv1.tv_usec = tv2.tv_usec;
//        if (tvt.tv_usec < 45000) {
//            usleep(50000 - tvt.tv_usec);
//            nbw += 50000 - tvt.tv_usec;
//        }
        nbw += tvt.tv_usec;
        n++;
        //if (n % 20 == 0) printf("time = %.2f\n", 1. * nbw / n);
        if (n % 50 == 0) printf("time = %.2f\n", t);
    }

    return 0;
}

int math1() {
    int i;
    long l1, l2;
    double d[1000], y;

    l1 = ReadTSC();
    for(i = 0 ; i < 1000 ; i++) {
        d[i] = sin(i);
    }
    l2 = ReadTSC();
    printf("sin = %f for %ld µops\n", d[999], (l2 - l1) / 1000);

    l1 = ReadTSC();
    y  = polya650(d, 1000, 0.5);
    l2 = ReadTSC();
    printf("p(%f) = %f for %ld µops\n", 0.5, y, l2 - l1);

    l1 = ReadTSC();
    for(i = 0 ; i < 997 ; i++) {
        i += ax2bxca650(d[i], d[i+1], d[i+2], d);
    }
    l2 = ReadTSC();
    printf("%d racines [%f ; %f]for %ld µops\n", i, d[0], d[1], l2 - l1);

    //test_line650();
    //test_geo1();
    //test_ref();
    //test3();
    return 0;
}

double fx(double x) {return 3*x + 1;}

int trig() {
    int N = 1000000;
    long l1, l2;
    double d, r1 = 0, r2 = 0, F, d2[2];
    float df, rf1 = 0, rf2 = 0;
    struct timeval tv1, tv2;

    // Double cossin
    r1 = r2 = 0;
    l1 = ReadTSC();
    for(d = 0. ; d < 1000. ; d += 1.) {
        r1 += cossina050(d, d2);
        r1 += d2[0];
    }
    l2 = ReadTSC();
    printf("cossina : %ld\n", (l2 - l1) / 1000);

    l1 = ReadTSC();
    for(d = 0. ; d < 1000. ; d += 1.) {
        r2 += cos(d);
        r2 += sin(d);
    }
    l2 = ReadTSC();
    printf("cossin  : %ld\n", (l2 - l1) / 1000);

    printf("cossina = %f  | cossin = %f\n", r1, r2);

    // Double cos
    r1 = r2 = 0;
    l1 = ReadTSC();
    for(d = 0. ; d < 1000. ; d += 1.) {
        r1 += sqrta050f(d, &fx);
    }
    l2 = ReadTSC();
    printf("cosa : %ld\n", (l2 - l1) / 1000);

    l1 = ReadTSC();
    for(d = 0. ; d < 1000. ; d += 1.) {
        r2 += sqrt(d);
    }
    l2 = ReadTSC();
    printf("cos  : %ld\n", (l2 - l1) / 1000);

    printf("cosa = %f  | cos = %f\n", r1, r2);

    // Float
    l1 = ReadTSC();
    for(df = 0.f ; df < 1000.f ; df += 1.f) {
        rf1 += cosa050f(df);
    }
    l2 = ReadTSC();
    printf("cosa050f : %ld\n", (l2 - l1) / 1000);

    l1 = ReadTSC();
    for(df = 0.f ; df < 1000.f ; df += 1.f) {
        rf2 += cosf(df);
    }
    l2 = ReadTSC();
    printf("cosf  : %ld\n", (l2 - l1) / 1000);

    printf("cosa050f = %f  | cosf = %f\n", rf1, rf2);


    return 0;
}

static void dump(char *n, int i0, int i1, const char *name) {
    int i = 0;
    printf("%s = {", name);
    printf("%d", n[i0++]);
    for(i = i0 ; i < i1 ; i++) printf(", %d", n[i]);
    printf("}\n");
}

static char *cand(int len) {
    int i;
    char *p = malloc(len);
    for(i = 0 ; i < len ; i++) {
        p[i] = (char)(10L * rand() / RAND_MAX);
    }
    return p;
}

int pi() {
    int len = 1024 * 1024;
    long l1, l2;
    struct timeval tv1, tv2;
//    char n1[32] = {6, 8, 0, 1, 4, 5, 3, 8, 4, 0, 4, 8, 2, 9, 8, 6, 8, 2, 9, 8, 6, 6, 8, 0, 4, 0, 4, 8, 2, 9, 8, 6};
//    char n2[32] = {1, 6, 4, 7, 4, 8, 2, 9, 8, 6, 6, 8, 0, 1, 4, 5, 8, 0, 1, 4, 5, 3, 8, 4, 8, 2, 9, 8, 6, 6, 8, 0};
    char *n1 = cand(len);
    char *n2 = cand(len);
    //
    dump(n1, 0, 32, "n1");
    dump(n2, 0, 32, "n2");

    // Add
    gettimeofday(&tv1, NULL);
    l1 = ReadTSC();
    void *p = adda050(n1, n2, len);
    l2 = ReadTSC();
    gettimeofday(&tv2, NULL);
    timersub(&tv2, &tv1, &tv2);
    printf("---------------------------  ADD  -----------------------------\n");
    dump(n1, 0, 32, "n1");
    printf("n1 = %p for %ld µops and %lds.%06luµs\n", n1, l2 - l1, tv2.tv_sec, tv2.tv_usec);

    // Mul
    gettimeofday(&tv1, NULL);
    l1 = ReadTSC();
    p = mula050(n1, len, 3);
    l2 = ReadTSC();
    gettimeofday(&tv2, NULL);
    timersub(&tv2, &tv1, &tv2);
    printf("------------------------------  MUL  -------------------------------\n");
    dump(n1, 0, 32, "n1");

    printf("n1 = %p / %p for %ld µops and %lds.%06luµs\n", n1, p, l2 - l1, tv2.tv_sec, tv2.tv_usec);

    return 0;
}

void init_lnum(uint64_t *n, int len) {
    int i;
    for(i = 0 ; i < len ; i++) {
        double d = 1. * rand() / RAND_MAX;
        n[i] = (uint64_t)(d * 9.999999999999e10);
        if (i < 6) printf("%ld\n", n[i]);
    }
}

void dump_lnum(uint64_t *n, int len, int nb) {
    int i = 0;
    uint64_t k;
    printf("n =");
    while(i < nb) {
        int j = i / 11;
        if (i % 11 == 0) {
            k = 1;
            printf(" |");
        }
        else k *= 10;
        printf(" %ld", (n[j] % (10 * k)) / k);
        i++;
    }
    printf("\n");
}

int pi2() {
    int i, len = 96 * 1024;
    long l1, l2, l;
    uint64_t *n1 = malloc(len * sizeof(uint64_t));

    //
    init_lnum(n1, len);
    dump_lnum(n1, len, 40);
    //
    l1 = ReadTSC();
    void *p = mulla050(n1, len , 2);
    l2 = ReadTSC();
    dump_lnum(n1, len, 40);
    printf("p = %p / %p = %lu = %lu\n", p, n1, (uint64_t)p - (uint64_t)n1, (uint64_t)p);
    printf("    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
    printf("For %ld µops\n", l2 -l1);
}

int ax2() {
    int i, n = 0, N = 1000000;
    long l1, l2;
    struct timeval tv1, tv2;
    double *di = calloc(N, sizeof(double)), F;
    double ci[5 * 6] = {
              1., -3., 2., 1., 2.
            , -1., 3., -2., 1., 2.
            , 1., -2., 1., 1., 0
            , 1., -2., 5., 0, 0
            , 0., -1., 1., 1., 0
            , 0., 0., 1.
    };
    int ni[6] = {2, 2, 1, 0, 1, 0};

    printf("TEST ax2bxc :\n");
    for(i = 0 ; i < 6 ; i++) {
        n = ax2bxca650(ci[5*i + 0], ci[5*i + 1], ci[5*i + 2], di);
        if (n != ni[i]) printf("KO : nb != : %d / %d\n", n, ni[i]);
        else if (ni[i] == 2 && (ci[5*i + 3] != di[0] || ci[5*i + 4] != di[1])) printf("KO : %f != %f  | %f != %f\n", ci[5*i + 3], di[0], ci[5*i + 4], di[1]);
        else if (ni[i] == 1 && ci[5*i + 3] != di[0]) printf("KO : %f != %f\n", ci[5*i + 3], di[0]);
        else printf("OK : test %d\n", i);

    }

    l1 = ReadTSC();
    for(i = 0 ; i < N ; i += 1) {
        di[i] = cos(i);
    }
    l2 = ReadTSC();
    printf("cos : %ld\n", (l2 - l1) / N);

    //
    gettimeofday(&tv1, NULL);
    l1 = ReadTSC();
    for(i = 0 ; i < N ; i++) {
        n += ax2bxca650(di[i], di[i+1], di[i+2], di);
    }
    l2 = ReadTSC();
    gettimeofday(&tv2, NULL);
    timersub(&tv2, &tv1, &tv2);
    F = 1. * tv2.tv_sec + 1e-6 * tv2.tv_usec;
    F = N / F;
    printf("ax2bxc : %ld for %d (%f) racines = %.2fkHz\n", (l2 - l1) / N, n, 1. / N * n, F / 1000.);

    free(di);
    return 0;
}

int ax2f() {
    int i, n = 0, N = 1000000;
    long l1, l2;
    struct timeval tv1, tv2;
    float *di = calloc(N, sizeof(float));
    double F;
    float ci[5 * 6] = {
              1., -3., 2., 1., 2.
            , -1., 3., -2., 1., 2.
            , 1., -2., 1., 1., 0
            , 1., -2., 5., 0, 0
            , 0., -1., 1., 1., 0
            , 0., 0., 1.
    };
    int ni[6] = {2, 2, 1, 0, 1, 0};

    printf("TEST ax2bxc :\n");
    for(i = 0 ; i < 6 ; i++) {
        n = ax2bxca650f(ci[5*i + 0], ci[5*i + 1], ci[5*i + 2], di);
        if (n != ni[i]) printf("KO : nb != : %d / %d\n", n, ni[i]);
        else if (ni[i] == 2 && (ci[5*i + 3] != di[0] || ci[5*i + 4] != di[1])) printf("KO : %f != %f  | %f != %f\n", ci[5*i + 3], di[0], ci[5*i + 4], di[1]);
        else if (ni[i] == 1 && ci[5*i + 3] != di[0]) printf("KO : %f != %f\n", ci[5*i + 3], di[0]);
        else printf("OK : test %d\n", i);

    }

    l1 = ReadTSC();
    for(i = 0 ; i < N ; i += 1) {
        di[i] = cos(i);
    }
    l2 = ReadTSC();
    printf("cos : %ld\n", (l2 - l1) / N);

    //
    gettimeofday(&tv1, NULL);
    l1 = ReadTSC();
    for(i = 0 ; i < N ; i++) {
        n += ax2bxca650f(di[i], di[i+1], di[i+2], di);
    }
    l2 = ReadTSC();
    gettimeofday(&tv2, NULL);
    timersub(&tv2, &tv1, &tv2);
    F = 1. * tv2.tv_sec + 1e-6 * tv2.tv_usec;
    F = N / F;
    printf("ax2bxc : %ld for %d (%f) racines = %.2fkHz\n", (l2 - l1) / N, n, 1. / N * n, F / 1000.);

    free(di);
    return 0;
}

int std1() {
    int i;
    long l1, l2;
    struct timeval tv1, tv2;
    void *p;
    bgra650 img;
    bgra_alloc650(&img, 1024, 576);

    //
    memset(img.data, 0xFF, 10);
    printf("%08X %08X %08X %08X\n", img.data[0], img.data[1], img.data[2], img.data[3]);
    //
    printf("memseta :\n");
    gettimeofday(&tv1, NULL);
    i = 1000;
    l1 = ReadTSC();
    while (i-- > 0) p = memseta650(img.data, ORANGE650, img.size << 2);
    l2 = ReadTSC();
    gettimeofday(&tv2, NULL);
    timersub(&tv2, &tv1, &tv2);
    printf("memseta : %ld µops (%p - %p = %lu) for %lds.%06luµs\n", l2 - l1, img.data, p, (uint64_t)p - (uint64_t)img.data, tv2.tv_sec, tv2.tv_usec);
    printf("%08X %08X %08X %08X\n", img.data[0], img.data[1], img.data[2], img.data[3]);

    //
    printf("memset  :\n");
    gettimeofday(&tv1, NULL);
    i = 1000;
    l1 = ReadTSC();
    while (i-- > 0) p = memset(img.data, 0, img.size << 2);
    l2 = ReadTSC();
    gettimeofday(&tv2, NULL);
    timersub(&tv2, &tv1, &tv2);
    printf("memset  : %ld µops for %lds.%06luµs\n", l2 - l1, tv2.tv_sec, tv2.tv_usec);

    return 0;
}


int poly2() {
    int i;
    long l1, l2;
    double co1[22] = {1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1.};
    double co2[22] = {1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1.};
    double x, y;

    x = 0.;

    for(i = 0 ; i < 1 ; i++) {
        //
        l1 = ReadTSC();
        y = polya650(co1, sizeof(co1) / 8, x);
        l2 = ReadTSC();
        printf("polya  : p(%f) = %f for %ldµos\n", x, y, l2 - l1);

        //
        l1 = ReadTSC();
        y = poly2a650(co1, sizeof(co1) / 8, x);
        l2 = ReadTSC();
        printf("poly2a : p(%f) = %f for %ldµos\n", x, y, l2 - l1);
    }

    //
    vect650 p, p0;
    vect650 p1, p2;
    bgra650 img1, img2;
    bgra650 img;
    bgra_alloc650(&img, 1024, 600);
    bgra_origin650(&img, 0., 0.);
    bgra_scale650(&img, 1024. / 4., 600. / 4.);
    //bgra_fill650(&img, 0xff804020);
    printf("img init ok\n");
    draw_line650(&img, -10., 0., +10., 0., MAGENTA650);
    draw_line650(&img, 0., -10., 0., +10., MAGENTA650);
    //
    p0.x = -1;
    p0.y = polya650(co1, sizeof(co1) / 8, p0.x);
    for(p.x = -1. ; p.x <= 1. ; p.x += 0.1) {
        p.y = polya650(co1, sizeof(co1) / 8, p.x);
//        dump650("p0 : ", &p0, " ") ; dump650(" p : ", &p, "\n");
        draw_line650(&img, p0.x, p0.y, p.x, p.y, GREEN650);
        p0.x = p.x;
        p0.y = p.y;
    }
    printf("img drawn\n");
    //
//    for(i = 0 ; i < sizeof(co1)/8 ; i++) {
//        random650(&p);
//        co1[i] = sin650(p.x * 3.14159);
//        co2[i] = cos650(p.y * 3.14159);
//    }
//    co1[0] = +0.4;
//    co2[0] = -0.4;
    //
    p1.x = +461.01 / 256.;
    p1.y = +250.99 / 150.;
    p2.x = +261.01 / 256.;
    p2.y = +42.01 / 150.;
    //
    fb650 *fb = fb_open650();
    double pas = 5e-3;
    long c;
    while(1) {
        fb_draw650(fb, &img);
        //
        usleep(40000);
        c = rand();
        c = (c | 0xff000000) & 0xffffffff;

        //
        for(i = 0 ; i < sizeof(co1)/8 ; i++) {
            random650(&p);
            //
            //co1[i] = sin650(p.x * M_PI);
            if (p.x < 0) co1[i] += pas;
            else co1[i] -= pas;
            if (p.y < 0) co2[i] += pas;
            else co2[i] -= pas;
        }
//        co1[0] = +0.1;
//        co2[0] = -0.4;
        //
        bgra_clear650(&img);
        draw_line650(&img, -1.9, 0., +1.9, 0., MAGENTA650);
        draw_line650(&img, 0., -1.9, 0., +1.9, MAGENTA650);
        //
//        dump650("p0 : ", &p0, " ") ; dump650(" p : ", &p, "\n");
        draw_line650(&img, p1.x, p1.y, p2.x, p2.y, GREEN650);
//        dump650("p0 : ", &p0, " ") ; dump650(" p : ", &p, "\n");
        draw_line650(&img, p1.x, p1.y - .25, p2.x, p2.y - .25, RED650);

        //
        p0.x = +2;
        p0.y = polya650(co1, sizeof(co1) / 8, p0.x);
        for(p.x = +2. ; p.x >= -2. ; p.x -= .0005) {
            p.y = polya650(co1, sizeof(co1) / 8, p.x);

//            draw_line650(&img, p0.x - 0.015625, p0.y, p.x - 0.015625, p.y, c);
//            draw_line650(&img, p0.x, p0.y, p.x, p.y, c);
//            draw_line650(&img, p0.x + 0.015625, p0.y, p.x + 0.015625, p.y, c);

            draw_point2a650(&img, p.x, p.y, c);

            p0.x = p.x;
            p0.y = p.y;
        }
    }
    fb_close650(&fb);

    //
    return 0;
}

int geo2() {
    int i;
    long l1, l2;
    uint64_t l;
    double d, a;
    struct timeval tv1, tv2;
    vect650 p1, p2;
    bgra650 img1, img2;

    //
    bgra_alloc650(&img1, 1024, 600);
    bgra_clear650(&img1);
    bgra_alloc650(&img2, 1024, 600);
    bgra_clear650(&img2);

    a = 0.5;
    for(i = 0 ; i < 360 ; i += 10) {
        a = i * 3.14159 / 180.;
        turn2d650(&p1, i * 3.14159 / 180.);
        p1.x = 300. * cos(a);
        p1.y = 300. * sin(a);
        l1 = ReadTSC();
        l = draw_line2a650(&img2, p1.x, p1.y, -p1.x, -p1.y, WHITE650);
        l2 = ReadTSC();
        printf("draw2 %3d° = %ld pixels  for   %ld µops ", i, l, l2 - l1);
        l1 = ReadTSC();
        draw_linea650(&img2, p1.x, p1.y, -p1.x, -p1.y, WHITE650);
        l2 = ReadTSC();
        printf(" / %ld µops\n", l2 - l1);
    }

    //
    bgra_clear650(&img2);
    p1.x = +250.5;
    p1.y = +250.5;
    p2.x = -250.5;
    p2.y = -255.5;

    gettimeofday(&tv1, NULL);
    l1 = ReadTSC();
    for(i = 0 ; i < 30000 ; i++) {
        draw_linea650(&img1, p1.x, p1.y, p2.x, p2.y, WHITE650);
    }
    l2 = ReadTSC();
    gettimeofday(&tv2, NULL);
    timersub(&tv2, &tv1, &tv2);
    printf("draw  : %ld for %lds.%06luµs\n", l2 - l1, tv2.tv_sec, tv2.tv_usec);

    gettimeofday(&tv1, NULL);
    l1 = ReadTSC();
    for(i = 0 ; i < 30000 ; i++) {
        draw_line2a650(&img2, p1.x, p1.y, p2.x, p2.y, WHITE650);
    }
    l2 = ReadTSC();
    gettimeofday(&tv2, NULL);
    timersub(&tv2, &tv1, &tv2);
    printf("draw2 : %ld for %lds.%06luµs = %lu\n", l2 - l1, tv2.tv_sec, tv2.tv_usec, l);

    //
    printf("compare : %d\n", bgra_compare650(&img1, &img2));
    //
    printf("Turn :\n");
    a = 0.6;
    double rad = 0.5;
//    for(a = -3.14159 ; a < +3.14159 ; a += 1) {
//        turn2d650(&p1, rad);
        p1.x = 290. * cos(rad);
        p1.y = 290. * sin(rad);
        dump650("u", &p1, " : ");
        l1 = ReadTSC();
        draw_line650(&img2, p1.x, p1.y, -p1.x, -p1.y, WHITE650);
        l2 = ReadTSC();
        printf("draw2 : %ld for %f = %lu\n", l2 - l1, 180. * rad / 3.14159, l);
//    }
}

void gtk1() {
    int i, width = 1024, height = 600;
    vect650 p1, p2;
    bgra650 bgra;
    //
    bgra_alloc650(&bgra, width, height);
    bgra_origin650(&bgra, +width/2, +height/2);
    bgra_scale650(&bgra, 1, -1);

//    fb650 *fb = fb_open650();
    //
    bgra_fill650(&bgra, 0xff000000);
    long c;
//    printf("bgra clear ok\n");
    while(1) {
        bgra_fill650(&bgra, 0xff000000);
        for(i = 0 ; i < 1000 ; i++) {
            random650(&p1); p1.x = (1 + p1.x) * width/2 ; p1.y = (1 + p1.y) * height/2;
            random650(&p2); p2.x = (1 + p2.x) * width/2 ; p2.y = (1 + p2.y) * height/2;
//            dump650("p1 = ", &p1, ""); dump650(" ; p2 = ", &p2, "\n");
            //printf("bgra random ok\n");
            c = 0L + RAND_MAX + 0L + rand();
            c = ((c << 32) | 0xff000000) >> 32;
            draw_line2a650(&bgra, p1.x, p1.y, p2.x, p2.y, c);
            //printf("bgra draw line ok\n");
        }
        //abgr
        draw_line650(&bgra, 0, 0, width, 0, 0xffff00ff);
        draw_line650(&bgra, 0, height - 1, width, height - 1, 0xff0000ff);
        draw_line650(&bgra, 0, 0, 0, height, 0xffff00ff);
        draw_line650(&bgra, width - 1, 0, width - 1, height, 0xff0000ff);
        //
//        fb_draw650(fb, &bgra);
        //usleep(40000);
    }
}


int unit2() {
    //
    int width = 1024, height = 600;
    vect650 p1, p2;
    bgra650 bgra;
//    // Config poly2
//    bgra_alloc650(&bgra, 1024, 600);
//    bgra_origin650(&bgra, 0., 0.);
//    bgra_scale650(&bgra, 1024. / 4., 600. / 4.);
//
//    //
////    p0 : (1.500000, -106.566510)  p : (1.250000, 56.634899)
//    p1.x = 1.5;
//    p1.y = -106.566510;
//    p2.x = 1.25;
//    p2.y = 56.634899;
//    dump650("p1 = ", &p1, ""); dump650(" ; p2 = ", &p2, "\n");
//    draw_line2a650(&bgra, p1.x, p1.y, p2.x, p2.y, 0xFFFF0FF0);
//
//    printf("x1 = %f\n", ((double*)bgra.data)[0]);
//    printf("y1 = %f\n", ((double*)bgra.data)[1]);
//    printf("x2 = %f\n", ((double*)bgra.data)[2]);
//    printf("y2 = %f\n", ((double*)bgra.data)[3]);
//    printf("a  = %f\n", ((double*)bgra.data)[4]);
//    printf("b  = %f\n", ((double*)bgra.data)[5]);

    // gtk
    // gtk1 config
    width = 800;
    height = 448;
    bgra_alloc650(&bgra, width, height);
    bgra_origin650(&bgra, +width/2, +height/2);
    bgra_scale650(&bgra, 1, -1);


    // p1 = (58.739364, 200.953567) ; p2 = (404.474496, 200.449352)
    // p1 = (127.849826, 271.316449) ; p2 = (161.247973, 39.581984) ; c = -1427598263
    long l = -1427598263;
//    p1.y = +414549.710635175288;
//    p1.x = 240.071571635126;
//    p2.y = 814.599590942056;
//    p2.x = 36093.231835617305;
    p1.x = +127.849826;
    p1.y = 271.316449;
    p2.x = 161.247973;
    p2.y = 39.581984;

    dump650("p1 = ", &p1, ""); dump650(" ; p2 = ", &p2, "\n");
    l = draw_line2a650(&bgra, p1.x, p1.y, p2.x, p2.y, l);

    printf("%ld pixels\n", l);
    printf("x1 = %f\n", ((double*)bgra.data)[0]);
    printf("y1 = %f\n", ((double*)bgra.data)[1]);
    printf("x2 = %f\n", ((double*)bgra.data)[2]);
    printf("y2 = %f\n", ((double*)bgra.data)[3]);
    printf("a  = %f\n", ((double*)bgra.data)[4]);
    printf("b  = %f\n", ((double*)bgra.data)[5]);

    return 0;
}


int unit3() {
    int i, width = 910, height = 512;
    long l1, l2, l;
    vect650 p1, p2;
    bgra650 bgra1, bgra2;

    //
    bgra_alloc650(&bgra1, 1024, 600);
    bgra_clear650(&bgra1);
    bgra_alloc650(&bgra2, 1024, 600);
    bgra_clear650(&bgra2);

    // test 1
    p1.x = -300;
    p1.y = -50;
    p2.x = +300;
    p2.y = +50;

//    // test 2
//    p1.x = -300;
//    p1.y = -150;
//    p2.x = +300;
//    p2.y = +149;

//    // test 3
//    p1.x = -250;
//    p1.y = -30;
//    p2.x = +250;
//    p2.y = +70;

    for(i = 0 ; i < 5 ; i++) {
        //
        l1 = ReadTSC();
        l = draw_line2a650(&bgra1, p1.x, p1.y, p2.x, p2.y, WHITE650);
        l2 = ReadTSC();
        printf("line2 : %ld µops = %ld\n", l2 - l1, l);

        //
        l1 = ReadTSC();
        l = draw_line3a650(&bgra2, p1.x, p1.y, p2.x, p2.y, WHITE650);
        l2 = ReadTSC();
        printf("line3 : %ld µops = %ld\n", l2 - l1, l);
    }
    //
    for(i = 0 ; i < 10 ; i++) {
        printf("%d : %d (%d, %d)\n", i, bgra2.data[i], bgra2.data[i] % 1024 - 512, bgra2.data[i] / 1024 - 300);
    }

    //
    bgra_compare650(&bgra1, &bgra2);
}


int point1() {
    int i;
    long l1, l2;
    uint64_t l, c;
    struct timeval tv1, tv2;
    vect650 p;
    bgra650 img1;
    //
    bgra_alloc650(&img1, 1024, 600);
    bgra_clear650(&img1);
    //
    l = 0;
    int nb = 1000000;
    l1 = ReadTSC();
    for(i = 0 ; i < nb ; i++) {
        random650(&p);
        p.x *= 600.;
        p.y *= 350.;
        l += draw_point2a650(&img1, p.x, p.y, ORANGE650);
    }
    l2 = ReadTSC();
    //
    printf("%ld / %d pixels drawn in %ld µops / %ld µ/pix\n", l, nb, l2 - l1, (l2 - l1) / nb);

    return 0;

}

int font1() {
    unsigned char c = 80;
    int i = monospaced650.index[c];
    long l1, l2;
    uint64_t l;
    struct timeval tv1, tv2;
    vect650 p;
    bgra650 img;
    //
    bgra_alloc650(&img, 1024, 600);
    bgra_clear650(&img);

    printf("%c : ascii code %d, %d pixels : %2x %2x %2x %2x\n", c, monospaced650.glyphs[i], monospaced650.glyphs[i+1], monospaced650.glyphs[i+2], monospaced650.glyphs[i+3], monospaced650.glyphs[i+3], monospaced650.glyphs[i+4]);

    //
    l1 = ReadTSC();
    l = draw_char2a650(&img, 10, 10, &monospaced650, 80, WHITE650);
    l2 = ReadTSC();
    //
    printf("Read char %c wrotte %ld pixels for %ld µ\n", c, l, l2 - l1);
}

int main() {
//    ax2();
//    ax2f();

//    trig();

//    std1();

//    pi2();

//    test_geo1();

//    poly2();      // fb/poly/line

//    geo2();       // ~unit/perf

//    test3();      // fb/cam/line

//    gtk1();       // viewer.maj()

//    unit2();        // test unit line2

//    unit3();        // test unit line3

//    point1();

    font1();

    return 0;
}

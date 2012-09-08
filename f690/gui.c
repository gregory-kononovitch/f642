/*
 * file    : f650/gui.c
 *
 * Created on: Sep 2, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>

#include "f690.h"

//
static int width = 1024;    // 960;
static int height = 600;    // 540;
static bgra650 bgra0;
static bgra650 bgra1;
static GdkPixbuf *img = NULL;
static int timer_delay = 65;
static long upd = 1000 / 65;
//
static brodge650 *brodge;
extern void inserta650(bgra650 *img, bgra650 *into);

struct timing {
    long frame;
    struct timeval  tv_timer;
    struct timeval  tv_maj1;
    struct timeval  timing;
    long            nb_timed;
    uint64_t        pixels;
    struct timeval  tv_maj2;
    struct timeval  maj;
    long            nb_maj;
    struct timeval  tv_queued;
    long            nb_queued;
    struct timeval  tv_exposed;
    long            nb_exposed;
    struct timeval  delay;
    //
    int mousex;
    int mousey;
    int refresh;
    int selection;
};
static struct timing *timing = NULL;

static void tick_timer() {
    if (timing) {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        timersub(&tv, &timing->tv_timer, &tv);
        timing->nb_timed++;
        // last frame
        if (timing->frame % upd == 0) {
            double m = timing->maj.tv_sec + 0.000001 * timing->maj.tv_usec;
            double d = timing->delay.tv_sec + 0.000001 * timing->delay.tv_usec;
            printf("Frame %ld : Timer : %ld, Maj %ld : %.0fms (%lu), Queue %ld, Expose %ld : %.2fms\n"
                    , timing->frame
                    , timing->nb_timed
                    , timing->nb_maj, 1000. * m / upd, timing->pixels / 1000
                    , timing->nb_queued
                    , timing->nb_exposed, 1000. * d / upd);
            memset(&timing->timing, 0, sizeof(struct timeval));
            memset(&timing->maj, 0, sizeof(struct timeval));
            memset(&timing->delay, 0, sizeof(struct timeval));
            timing->pixels = 0;
            timing->nb_timed = 0;
            timing->nb_maj = 0;
            timing->nb_queued = 0;
            timing->nb_exposed = 0;
        }

        //
        timing->frame++;
        gettimeofday(&timing->tv_timer, NULL);
    }
}
static void tick_maj1() {
    if (timing) {
        struct timeval tv;
        gettimeofday(&timing->tv_maj1, NULL);
        timersub(&timing->tv_maj1, &timing->tv_timer, &tv);
        timeradd(&timing->timing, &tv, &timing->timing);
    }
}
static void tick_maj2() {
    if (timing) {
        struct timeval tv;
        gettimeofday(&timing->tv_maj2, NULL);
        timersub(&timing->tv_maj2, &timing->tv_maj1, &tv);
        timeradd(&timing->maj, &tv, &timing->maj);
        timing->nb_maj++;
    }
}
static void tick_queued() {
    if (timing) {
        gettimeofday(&timing->tv_queued, NULL);
        timing->nb_queued++;
    }
}
static void tick_expose() {
    if (timing) {
        struct timeval tv;
        gettimeofday(&timing->tv_exposed, NULL);
        timersub(&timing->tv_exposed, &timing->tv_queued, &tv);
        timeradd(&timing->delay, &tv, &timing->delay);
        timing->nb_exposed++;
    }
}

static void tst_bgra();

bgra650 *get_bgra() {
    return &bgra0; // before a straight struct
}


static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data) {
    // Change TRUE to FALSE and the main window will be destroyed with
    return FALSE;
}


static void destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

/*
 *
 */
static gboolean on_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
    tick_expose();
    return FALSE;
}

static void tst_bgra() {
    int i, j, debug = 0;
    vect650 p1, p2;
    //
    tick_maj1();
    //
    bgra_fill650(&bgra0, 0xff000000);
    long c;
    if (debug) printf("bgra clear ok\n");
    for (i = 0; i < 1; i++) {
        random650(&p1);
        p1.x = (1 + p1.x) * width / 2.;
        p1.y = (1 + p1.y) * height / 2.;
//        random650(&p2); p2.x = (1 + p2.x) * width/2. ; p2.y = (1 + p2.y) * height/2.;
//        random650(&p1); p1.x = -width/2. + (1. + p1.x) * width ; p1.y = -height/2. + (1. + p1.y) * height;
//        random650(&p2); p2.x = -width/2. + (1. + p2.x) * width ; p2.y = -height/2. + (1. + p2.y) * height;
//        p2.x = p1.x + 300 ; p2.y = p1.y + 50;
//        p1.x = -250;
//        p1.y = -50;
//        p2.x = +250;
//        p2.y = +50;
//
//        //
//        if (debug) printf("bgra0 random ok\n");
//        c = rand();
//        c = (c | 0xff000000) & 0xffffffff;
//        if (debug) dump650("p1 = ", &p1, "");
//        if (debug) dump650(" ; p2 = ", &p2, "");
//        if (debug) printf(" ; c = %ld\n", c);
//        c = draw_line2a650(&bgra0, p1.x, p1.y, p2.x, p2.y, WHITE650);
//        if (debug) printf("line2 ok\n", c);
//        c = draw_line3a650(&bgra0, p1.x, p1.y + 20, p2.x, p2.y + 20, ORANGE650);
////        c = draw_line2a650(&bgra0, p1.x, p1.y, p2.x, p2.y, c);
//        if (debug) {
//            printf("bgra0 draw line ok :\n");
//            printf("x1 = %f\n", ((double*)bgra0.data)[0]);
//            printf("y1 = %f\n", ((double*)bgra0.data)[1]);
//            printf("x2 = %f\n", ((double*)bgra0.data)[2]);
//            printf("y2 = %f\n", ((double*)bgra0.data)[3]);
//            printf("a  = %f\n", ((double*)bgra0.data)[4]);
//            printf("b  = %f\n", ((double*)bgra0.data)[5]);
//            printf("dist  = %f\n", ((float*)bgra0.data)[12]);
//            printf("ni    = %f\n", ((float*)bgra0.data)[13]);
//            printf("pas   = %f\n", ((float*)bgra0.data)[14]);
//            printf("return %ld\n", c);
//        }

        double a = -14;
        int k;
        while (j < 14) {
            turn2d650(&p1, 3.14159 * a / 180.);
            p2.x = -200. * p1.x;
            p2.y = -200. * p1.y;
            p1.x = +200. * p1.x;
            p1.y = +200. * p1.y;
//            dump650("p1 = ", &p1, "");
//            dump650(" ; p2 = ", &p2, "\n");
            c = draw_line3a650(&bgra0, p1.x, p1.y, p2.x, p2.y, WHITE650);
            if (bgra0.data[5] > bgra0.data[7] || bgra0.data[6] > bgra0.data[7]) {
                dump650("p1 = ", &p1, "");
                dump650(" ; p2 = ", &p2, "\n");
                for (k = 0; k < 10; k++) {
                    printf("%d : %d (%d, %d)\n", k, bgra0.data[k], bgra0.data[k] % 1024 - 512, bgra0.data[k] / 1024 - 300);
                }
            }
//            dump650("p1 = ", &p1, "");
//            dump650(" ; p2 = ", &p2, "\n");
//            printf("res = %ld\n", c);
            j = j + 1;
            a += 2;
        }
    }

    if (debug) printf("maj 150 ok\n");
    tick_maj2();
}

static void maj() {
    int i;
    vect650 p1, p2;
    //
    if (timing->frame % 2 == 0 && timing->refresh) {
        tick_maj1();
        //
        bgra_fill650(&bgra0, 0xff000000);
        long c;
        for (i = 0; i < 1500; i++) {
            random650(&p1);
            p1.x = p1.x * width *  .4 + (timing->mousex - width / 2);
            p1.y = p1.y * height * .4 + (height / 2. - timing->mousey);
            random650(&p2);
            p2.x = p2.x * width *  .4 + (timing->mousex - width / 2);
            p2.y = p2.y * height * .4 + (height / 2. - timing->mousey);
            ;
    //        random650(&p1); p1.x = -width/2. + (1. + p1.x) * width ; p1.y = -height/2. + (1. + p1.y) * height;
    //        random650(&p2); p2.x = -width/2. + (1. + p2.x) * width ; p2.y = -height/2. + (1. + p2.y) * height;

            c = rand();
            c = (c | 0xff000000) & 0xffffffff;
            if (i % 20 == 0) {
                timing->pixels += draw_line2a650(&bgra0, p1.x, p1.y, p2.x, p2.y, c % 2 == 0 ? ORANGE650 : YELLOW650);
            }
            timing->pixels += draw_char2a650(&bgra0, p1.x, p1.y, &monospaced650, 32 + (c % 90), c);
        }
        tick_maj2();
    }
    //
    unsigned char str[16];
    snprintf((char*)str, 16, "%d", timing->mousex);
    i = 0;
    int x = width/2 - 100;
    while(str[i]) {
        timing->pixels += draw_char2a650(&bgra0, x, -height/2 + 5, &monospaced650, str[i], BLUE650);
        x += monospaced650.width;
        i++;
    }
    snprintf((char*)str, 16, "%d", timing->mousey);
    i = 0;
    x += monospaced650.width;
    while(str[i]) {
        timing->pixels += draw_char2a650(&bgra0, x, -height/2 + 5, &monospaced650, str[i], BLUE650);
        x += monospaced650.width;
        i++;
    }
}

static int soon = 0;
static void maj_brodge() {
    if (timing->refresh && !soon) {
        soon = 1;
        tick_maj1();
        //bgra_clear650(&bgra0);
        brodge_anim(brodge);
        if (timing->selection > -1) {
            brodge->sources[timing->selection]->x = timing->mousex;
            brodge->sources[timing->selection]->y = timing->mousey;
        }
        brodge_exec(brodge, &bgra1);
        inserta650(&bgra1, &bgra0);
        tick_maj2();
        soon = 0;
    }
}

static gboolean time_handler(GtkWidget *widget) {
    tick_timer();
    //
    //tst_bgra();
    maj_brodge();
    //
    tick_queued();
    gtk_widget_queue_draw(widget);
    return TRUE;
}

// typedef void (* GdkPixbufDestroyNotify) (guchar *pixels, gpointer data);
static void pbd(guchar *pixels, gpointer data) {
    printf("PixBuf free\n");
    return;
}

static gboolean motion_notify_event(GtkWidget *widget, GdkEventMotion *event) {
    GdkModifierType state;

    if (event->is_hint) {
        gdk_window_get_pointer(event->window, &timing->mousex, &timing->mousey, &state);
    } else {
        timing->mousex = event->x;
        timing->mousey = event->y;
        state = event->state;
    }
//    if (state & GDK_BUTTON1_MASK && pixmap != NULL) draw_brush(widget, x, y);
    return TRUE;
}

static gboolean key_press_event(GtkWidget *widget, GdkEventKey *event) {
    printf("Keyval = %d\n", event->keyval);
    switch(event->keyval) {
        case 32:
            brodge_rebase(brodge);
            break;
        case 65307:
            gtk_main_quit();
            break;
    }
    return TRUE;
}

static gboolean button_press_event(GtkWidget *widget, GdkEventButton *event) {
    if (event->button == 1) {
        timing->selection++;
        if (timing->selection == brodge->nb_src) {
            timing->selection = -1;
        }
    } else if (event->button == 3) {
        timing->refresh = !timing->refresh;
    }
    return TRUE;
}

static gboolean button_release_event(GtkWidget *widget, GdkEventButton *event) {
    if (event->button == 1) {
    }
    return TRUE;
}

/*
 *
 */
int main(int argc, char *argv[]) {

    //
    timing = calloc(1, sizeof(struct timing));
    timing->refresh = 1;
    timing->selection = -1;
    //
    GtkWidget *window;
    GtkWidget *fixed;
    GtkWidget *frame;

    //
    gtk_init(&argc, &argv);

    // Window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "gui");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window), 0);
    gtk_window_set_default_size(GTK_WINDOW(window), width, height);
    gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);
    printf("window ok\n");

    // layout
    fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);
    printf("fixed ok\n");

    // Image
//    GList *visuals = gdk_list_visuals();
//    void tst(gpointer data, gpointer udata) {
//        if (((GdkVisual*) data)->depth == 32) printf(
//                "visual :\n\ttype = %d\n\ttype = %d\n\tdepth = %d\n\tbits/rgb = %d\n\torder = %d\n\tred = %08X\n\tgreen = %08X\n\tblue = %08X\n",
//                ((GdkVisual*) data)->type, ((GdkVisual*) data)->colormap_size, ((GdkVisual*) data)->depth,
//                ((GdkVisual*) data)->bits_per_rgb, ((GdkVisual*) data)->byte_order, ((GdkVisual*) data)->red_mask,
//                ((GdkVisual*) data)->green_mask, ((GdkVisual*) data)->blue_mask);
//    }
//    g_list_foreach(visuals, &tst, NULL);
//    GdkVisual *visu = gdk_visual_get_best_with_depth(32);
//    GdkImage *gdimg = gdk_image_new(GDK_IMAGE_SHARED, visu, width, height);
//    printf("GdkImage : bytes/pix = %d, linesize = %d, bits/pix = %d ; mem = %p\n", gdimg->bpp, gdimg->bpl,
//            gdimg->bits_per_pixel, gdimg->mem);

    //
    brodge = brodge_init(928, 522, 3);
    bgra_alloc650(&bgra1, brodge->width, brodge->height);

    //
    bgra_alloc650(&bgra0, width, height);
    bgra_fill650(&bgra0, 0xff000000);
    printf("bgra alloc ok ::\n");

    img = gdk_pixbuf_new_from_data((guchar*) bgra0.data, GDK_COLORSPACE_RGB, TRUE, 8, width, height, width << 2, &pbd, NULL);
    printf("PixBuf new ok\n");

    // Image
    frame = gtk_image_new_from_pixbuf(img);
    gtk_fixed_put(GTK_FIXED(fixed), frame, 0, 0);
    printf("fixed ok\n");

    // Events
//    g_signal_connect(darea, "expose-event", G_CALLBACK (on_expose_event), NULL);
    g_signal_connect(frame, "expose-event", G_CALLBACK(on_expose_event), NULL);

    g_signal_connect(window, "key_press_event", G_CALLBACK (key_press_event), NULL);
    g_signal_connect(window, "motion_notify_event", G_CALLBACK (motion_notify_event), NULL);
    g_signal_connect(window, "button_press_event", G_CALLBACK (button_press_event), NULL);
    g_signal_connect(window, "button_release_event", G_CALLBACK (button_release_event), NULL);

    gtk_widget_set_events(
            window,
            GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK
            | GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK
            | GDK_BUTTON_RELEASE_MASK
            | GDK_KEY_PRESS_MASK
            | GDK_POINTER_MOTION_HINT_MASK);

    g_signal_connect(window, "delete-event", G_CALLBACK(delete_event), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);

    // Show
    gtk_widget_show(fixed);
    gtk_widget_show_all(window);

    // Timer
    g_timeout_add(timer_delay, (GSourceFunc)time_handler, (gpointer)frame);

    gtk_main();

    return 0;
}


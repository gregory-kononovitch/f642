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
static int          width  = 910;
static int          height = 512;
static bgra650      bgra;
static GdkPixbuf    *img   = NULL;

static void tst_bgra();

bgra650 *get_bgra() {
    return &bgra;   // before a straight struct
}

//
static void hello(GtkWidget *widget, gpointer data) {
    g_print("Hello !\n");
}

static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data) {
    /* If you return FALSE in the "delete-event" signal handler,
     * GTK will emit the "destroy" signal. Returning TRUE means
     * you don't want the window to be destroyed.
     * This is useful for popping up 'are you sure you want to quit?'
     * type dialogs. */

    g_print("delete event occurred\n");

    /* Change TRUE to FALSE and the main window will be destroyed with
     * a "delete-event". */

    return FALSE;
}

/* Another callback */
static void destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

/*
 *
 */
static gboolean on_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
    return FALSE;
}

static void tst_bgra() {
    int i, j, debug = 0;
    vect650 p1, p2;
    //
    bgra_fill650(&bgra, 0xff000000);
    long c;
    if (debug) printf("bgra clear ok\n");
    for(i = 0 ; i < 1 ; i++) {
        random650(&p1); p1.x = (1 + p1.x) * width/2. ; p1.y = (1 + p1.y) * height/2.;
//        random650(&p2); p2.x = (1 + p2.x) * width/2. ; p2.y = (1 + p2.y) * height/2.;
//        random650(&p1); p1.x = -width/2. + (1. + p1.x) * width ; p1.y = -height/2. + (1. + p1.y) * height;
//        random650(&p2); p2.x = -width/2. + (1. + p2.x) * width ; p2.y = -height/2. + (1. + p2.y) * height;
//        p2.x = p1.x + 300 ; p2.y = p1.y + 50;
        p1.x = -250;
        p1.y = -50;
        p2.x = +250;
        p2.y = +50;

        //
        if (debug) printf("bgra random ok\n");
        c = rand();
        c = (c | 0xff000000) & 0xffffffff;
        if (debug) dump650("p1 = ", &p1, "");
        if (debug) dump650(" ; p2 = ", &p2, "");
        if (debug) printf(" ; c = %ld\n", c);
        if (debug) printf("%ld", c);
        c = draw_line2a650(&bgra, p1.x, p1.y, p2.x, p2.y, MAGENTA650);
        c = draw_line3a650(&bgra, p1.x, p1.y - 10, p2.x, p2.y + 20, ORANGE650);
//        c = draw_line2a650(&bgra, p1.x, p1.y, p2.x, p2.y, c);
        if (debug) {
            printf("bgra draw line ok :\n");
            printf("x1 = %f\n", ((double*)bgra.data)[0]);
            printf("y1 = %f\n", ((double*)bgra.data)[1]);
            printf("x2 = %f\n", ((double*)bgra.data)[2]);
            printf("y2 = %f\n", ((double*)bgra.data)[3]);
            printf("a  = %f\n", ((double*)bgra.data)[4]);
            printf("b  = %f\n", ((double*)bgra.data)[5]);
            printf("dist  = %f\n", ((float*)bgra.data)[12]);
            printf("ni    = %f\n", ((float*)bgra.data)[13]);
            printf("pas   = %f\n", ((float*)bgra.data)[14]);
            printf("return %ld\n", c);
        }

//        double a = 0;
//        while(j < 360) {
//            turn2d650(&p1, 3.14159 * a / 180.);
//            p2.x = -800. * p1.x;
//            p2.y = -256 - 800. * p1.y;
//            p1.x = +800. * p1.x;
//            p1.y = -256 + 800. * p1.y;
//            c = draw_line2a650(&bgra, p1.x, p1.y, p2.x, p2.y, WHITE650);
//            j = j + 1;
//            a += 0.5;
//        }
    }
    if (debug) printf("maj 150 ok\n");
}

static gboolean time_handler(GtkWidget *widget) {
    //
    tst_bgra();
    gtk_widget_queue_draw(widget);
    return TRUE;
}


// typedef void (* GdkPixbufDestroyNotify) (guchar *pixels, gpointer data);
static void pbd(guchar *pixels, gpointer data) {
    printf("PixBuf free\n");
    return;
}

/*
 *
 */
int main(int argc, char *argv[]) {
    //
    GtkWidget *window;
    GtkWidget *fixed;
    GtkWidget *darea;
    GtkWidget  *frame;

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

//    // drawing area
//    darea = gtk_drawing_area_new();
//    gtk_drawing_area_size(GTK_DRAWING_AREA(darea), width, height);
//    printf("area ok\n");
//
//    //gtk_container_add(GTK_CONTAINER(window), darea);
//    gtk_fixed_put(GTK_FIXED(fixed), darea, 0, 0);
//    printf("fixed ok\n");

    // Image
    GList *visuals = gdk_list_visuals();
    void tst(gpointer data, gpointer udata) {
        if (((GdkVisual*)data)->depth == 32)
        printf("visual :\n\ttype = %d\n\ttype = %d\n\tdepth = %d\n\tbits/rgb = %d\n\torder = %d\n\tred = %08X\n\tgreen = %08X\n\tblue = %08X\n"
                , ((GdkVisual*)data)->type
                , ((GdkVisual*)data)->colormap_size
                , ((GdkVisual*)data)->depth
                , ((GdkVisual*)data)->bits_per_rgb
                , ((GdkVisual*)data)->byte_order
                , ((GdkVisual*)data)->red_mask
                , ((GdkVisual*)data)->green_mask
                , ((GdkVisual*)data)->blue_mask
        );
    }
    g_list_foreach(visuals, &tst, NULL);
    GdkVisual *visu = gdk_visual_get_best_with_depth(32);
    GdkImage  *gdimg = gdk_image_new(GDK_IMAGE_SHARED, visu, width, height);
    printf("GdkImage : bytes/pix = %d, linesize = %d, bits/pix = %d ; mem = %p\n"
            , gdimg->bpp, gdimg->bpl, gdimg->bits_per_pixel
            , gdimg->mem
    );
    //
//    GdkBitmap *mask = NULL;
//    GtkImage *gtimg = gtk_image_new_from_image(gdimg, mask);

//    return 0;

    // GdkPixbufAnimation
    //gtk_image_set_from_pixbuf
    bgra_alloc650(&bgra, width, height);
//    bgra_origin650(&bgra, +width/2, +height/2);
//    bgra_scale650(&bgra, 1, -1);
    printf("bgra alloc ok ::\n");
    tst_bgra();
    printf("bgra maj ok\n");

    img = gdk_pixbuf_new_from_data(
              (guchar*)bgra.data
            , GDK_COLORSPACE_RGB
            , TRUE
            , 8
            , width
            , height
            , width << 2
            , &pbd, NULL
    );
    printf("PixBuf new ok\n");

    //
    // Image
    frame = gtk_image_new_from_pixbuf(img);
//    frame = gtimg;
    gtk_fixed_put(GTK_FIXED(fixed), frame, 0, 0);
    printf("fixed ok\n");



    // Events
//    g_signal_connect(darea, "expose-event", G_CALLBACK (on_expose_event), NULL);
    g_signal_connect(frame, "expose-event", G_CALLBACK(on_expose_event), NULL);
    g_signal_connect(window, "delete-event", G_CALLBACK(delete_event), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);


    // Show
//    gtk_widget_show(button);
//    gtk_widget_show(darea);
    gtk_widget_show(fixed);
    gtk_widget_show_all(window);

    // Timer
    g_timeout_add(40, (GSourceFunc)time_handler, (gpointer)frame);

    gtk_main();

    return 0;
}


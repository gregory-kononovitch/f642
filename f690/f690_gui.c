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
static int          width  = 800;
static int          height = 448;
static bgra650      bgra;
static GdkImage     *ximg;
static GdkPixbuf    *img   = NULL;
static GdkPixmap    *pixmap= NULL;
//


static void maj();


static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data) {
    // FALSE to exit appli
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
    printf("Ev : draw start\n");
//    gdk_draw_image(widget->window, widget->style->black_gc
//            , ximg
//            , event->area.x, event->area.y
//            , event->area.x, event->area.y
//            , event->area.width, event->area.y
//    );
    //
    gdk_draw_drawable(widget->window,
                widget->style->black_gc,
                pixmap,
                event->area.x, event->area.y,
                event->area.x, event->area.y,
                event->area.width, event->area.height);

    printf("Ev : draw done\n");
    // ok
    return FALSE;
}

static void maj() {
    int i;
    vect650 p1, p2;
    //
    bgra_fill650(&bgra, 0xff000000);
    long c;
    for(i = 0 ; i < 150 ; i++) {
        random650(&p1); p1.x = (1 + p1.x) * width/2 ; p1.y = (1 + p1.y) * height/2;
        random650(&p2); p2.x = (1 + p2.x) * width/2 ; p2.y = (1 + p2.y) * height/2;
        c = 0L + RAND_MAX + 0L + rand();
        c = ((c << 32) | 0xff000000) >> 32;
        draw_linea650(&bgra, p1.x, p1.y, p2.x, p2.y, c);
    }
    //abgr
    draw_line650(&bgra, 0, 0, width, 0, 0xffff00ff);
    draw_line650(&bgra, 0, height - 1, width, height - 1, 0xff0000ff);
    draw_line650(&bgra, 0, 0, 0, height, 0xffff00ff);
    draw_line650(&bgra, width - 1, 0, width - 1, height, 0xff0000ff);
}

static gboolean time_handler(GtkWidget *widget) {
    maj();
//    printf("TH : maj   done (%p <- %p)\n", ximg->mem, bgra.data);
//    memcpy(ximg->mem, bgra.data, bgra.size << 2);
//    printf("TH : mcpy  done\n");
    gtk_widget_queue_draw_area(widget, 0, 0, width, height);
    printf("TH : queue done\n");
    return TRUE;
}


/*
 *
 */
int main(int argc, char *argv[]) {
    //
    GtkWidget *window;
    GtkWidget *fixed;
    GtkWidget *area;

    //
    gtk_init(&argc, &argv);

    // Window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "gui");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window), 0);
    gtk_window_set_default_size(GTK_WINDOW(window), width, height);
    printf("window ok\n");

    // layout
    fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);
    printf("fixed ok\n");

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
    ximg = gdk_image_new(GDK_IMAGE_SHARED, visu, width, height);
    printf("GdkImage : bytes/pix = %d, linesize = %d, bits/pix = %d ; type %d (mem = %p)\n"
            , ximg->bpp, ximg->bpl, ximg->bits_per_pixel
            , ximg->type, ximg->mem
    );
    // GdkPixbufAnimation
    //gtk_image_set_from_pixbuf

    //
    GdkColormap *dcm = gdk_colormap_new(visu, FALSE);
    // drawing area
    area = gtk_drawing_area_new();
    gdk_drawable_set_colormap(window, dcm);
    gdk_drawable_set_colormap(area, dcm);
    gtk_drawing_area_size(GTK_DRAWING_AREA(area), width, height);
    printf("area ok\n");

    //
    gtk_fixed_put(GTK_FIXED(fixed), area, 0, 0);
    printf("fixed ok\n");


    //
    bgra_alloc650(&bgra, width, height);
    bgra_origin650(&bgra, +width/2, +height/2);
    bgra_scale650(&bgra, 1, -1);
    printf("bgra alloc ok\n");
    maj();
    printf("bgra maj done, still (%p <- %p)\n", ximg->mem, bgra.data);
//    // Ximg
//    memcpy(ximg->mem, bgra.data, bgra.size);
//    printf("mcpy done\n");
    // Pixmap
    GdkColor bg;
    GdkColor fg;
    fg.pixel = 0xff000000;
    fg.red = 0;
    fg.green = 0;
    fg.blue = 0;
    bg.pixel = 0xff000000;
    bg.red = 0;
    bg.green = 0;
    bg.blue = 0;
    pixmap = gdk_pixmap_create_from_data(
            GTK_WINDOW(window)
            , bgra.data
            , width
            , height
            , 32
            , &fg
            , &bg
    );


//    img = gdk_pixbuf_new_from_data(
//              (guchar*)bgra.data
//            , GDK_COLORSPACE_RGB
//            , TRUE
//            , 8
//            , width
//            , height
//            , width << 2
//            , &pbd, NULL
//    );
//    printf("PixBuf new ok\n");

    //
    // Image
//    frame = gtk_image_new_from_pixbuf(img);
////    frame = gtimg;
//    gtk_fixed_put(GTK_FIXED(fixed), frame, 0, 0);
//    printf("fixed ok\n");


    // Events
    g_signal_connect(area, "expose-event", G_CALLBACK (on_expose_event), NULL);
//    g_signal_connect(frame, "expose-event", G_CALLBACK (on_expose_event), NULL);
    g_signal_connect(window, "delete-event", G_CALLBACK (delete_event), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK (destroy), NULL);
    printf("signals ok\n");

    // Show
//    gtk_widget_show(area);
    gtk_widget_show(fixed);
    gtk_widget_show_all(window);
    printf("show ok\n");

    // Timer
    g_timeout_add(500, (GSourceFunc)time_handler, (gpointer)area);
    printf("timer ok\n");

    gtk_main();

    return 0;
}


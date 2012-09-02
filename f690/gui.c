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


static bgra650      *bgra;
static GdkPixbuf    *img;

/*
 * 		Prototypes
 */
static void hello(GtkWidget *widget, gpointer data);
static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);
static void destroy(GtkWidget *widget, gpointer data);

/*
 *
 */
/* This is a callback function. The data arguments are ignored
 * in this example. More on callbacks below. */
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
    gint x = widget->allocation.x;
    gint y = widget->allocation.y;
    gint width = widget->allocation.width;
    gint height = widget->allocation.height;

    cairo_t *cr;

    cr = gdk_cairo_create(widget->window);

    cairo_set_source_rgba(cr, 0, 0, 0, 1);

    static const double dashed1[] = { 4.0, 1.0 };
    static int len1 = sizeof(dashed1) / sizeof(dashed1[0]);

    static const double dashed2[] = { 4.0, 1.0, 4.0 };
    static int len2 = sizeof(dashed2) / sizeof(dashed2[0]);

    static const double dashed3[] = { 1.0 };

    /*
     *       Geometry
     */
    cairo_select_font_face(cr, "Purisa", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 13);

    char s[16];
    snprintf(s, 15, "( %i , %i )", x, y);
    cairo_move_to(cr, 10, height - 1);
    cairo_show_text(cr, s);

    snprintf(s, 15, "( %i , %i )", width, height);
    cairo_move_to(cr, width / 2, height - 1);
    cairo_show_text(cr, s);

    /*
     *         DASH
     */
    cairo_set_line_width(cr, 1.5);

    cairo_set_dash(cr, dashed1, len1, 0);
    cairo_move_to(cr, 40, 30);
    cairo_line_to(cr, 200, 30);
    cairo_stroke(cr);

    cairo_set_dash(cr, dashed2, len2, 1);
    cairo_move_to(cr, 40, 50);
    cairo_line_to(cr, 200, 50);
    cairo_stroke(cr);

    cairo_set_dash(cr, dashed3, 1, 0);
    cairo_move_to(cr, 40, 70);
    cairo_line_to(cr, 200, 70);
    cairo_stroke(cr);

    //cairo_set_dash(cr, nodash, 0, 0);
    cairo_set_dash(cr, (double[]) {}, 0, 0);

    /*
     *        Rectangles
     */
    cairo_set_source_rgb(cr, 0.69, 0.19, 0);
    cairo_rectangle(cr, 30, 30, 100, 100);
    cairo_set_line_width(cr, 14);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_MITER);
    cairo_stroke(cr);

    cairo_set_source_rgb(cr, 0.3, 0.4, 0.6);
    cairo_rectangle(cr, 160, 30, 100, 100);
    cairo_set_line_width(cr, 14);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_BEVEL);
    cairo_stroke(cr);

    cairo_set_source_rgb(cr, 0.3, 0.8, 0.2);
    cairo_rectangle(cr, 100, 160, 100, 100);
    cairo_set_line_width(cr, 14);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
    cairo_stroke(cr);

    cairo_destroy(cr);

    return FALSE;
}

static gboolean time_handler(GtkWidget *widget) {
    gtk_widget_queue_draw(widget);
    return TRUE;
}


// typedef void (* GdkPixbufDestroyNotify) (guchar *pixels, gpointer data);
static void pbd(guchar *pixels, gpointer data) {
    return;
}

/*
 *
 */
int main(int argc, char *argv[]) {
    //
    int width  = 800;
    int height = 448;
    //
    GtkWidget *window;
    GtkWidget *fixed;
    GtkWidget *button;
    GtkWidget *darea;

    //
    gtk_init(&argc, &argv);

    // Window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "gui");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER (window), 0);
    gtk_window_set_default_size(GTK_WINDOW(window), 800 + 4, height + 20 + 4);

    // layout
    fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);

    // widget
    button = gtk_button_new_with_label("Hello !");
    gtk_fixed_put(GTK_FIXED(fixed), button, 580, 10);

    // drawing area
    darea = gtk_drawing_area_new();
    gtk_drawing_area_size(GTK_DRAWING_AREA(darea), width, height);

    //gtk_container_add(GTK_CONTAINER(window), darea);
    gtk_fixed_put(GTK_FIXED(fixed), darea, 0, 0);

    // Image
    // GdkPixbufAnimation
    bgra = bgra_alloc650(&bgra, width, height);

    img = gdk_pixbuf_new_from_data(
              bgra->data
            , GDK_COLORSPACE_RGB
            , TRUE
            , 8
            , width
            , height
            , 0
            , &pbd, NULL
    );



    // Events
    g_signal_connect(button, "clicked", G_CALLBACK (hello), NULL);
    g_signal_connect_swapped(button, "clicked", G_CALLBACK (gtk_widget_destroy), window);
    g_signal_connect(darea, "expose-event", G_CALLBACK (on_expose_event), NULL);
    g_signal_connect(window, "delete-event", G_CALLBACK (delete_event), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK (destroy), NULL);


    // Show
//    gtk_widget_show(button);
//    gtk_widget_show(darea);
    gtk_widget_show(fixed);
    gtk_widget_show_all(window);

    // Timer
    g_timeout_add(40, (GSourceFunc)time_handler, (gpointer)darea);

    gtk_main();

    return 0;
}


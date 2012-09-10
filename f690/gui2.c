/*
 * file    : f650/gui2.c
 *
 * Created on: Sep 19, 2012 (fork of gui.c)
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>

#include "f690.h"


struct timing {
    long frame;
    struct timeval  tv_timer;
    struct timeval  timing;
    long            nb_timed;
    // brodge
    struct timeval  tv_maj1;
    uint64_t        pixels;
    struct timeval  tv_maj2;
    struct timeval  maj;
    long            nb_maj;
    // layout
    struct timeval  tv_layout1;
    struct timeval  tv_layout2;
    struct timeval  layout;
    long            nb_layout;
    long            ops_layout;
    // Queue
    struct timeval  tv_queued;
    long            nb_queued;
    // Expose
    struct timeval  tv_exposed1;
    struct timeval  delay;
    struct timeval  tv_exposed2;
    struct timeval  expose;
    long            nb_exposed;
    //
    int mousex;
    int mousey;
    int refresh;
    int selection;
};

//
struct gui690tmp {
    int width;
    int height;
    //
    desk654 *desk;
    //
    GtkWidget *window;
    GtkWidget *frame;
    GdkPixbuf *img;

    int timer_delay;
    long updlog;
    //
    brodge650 *brodge;
    //
    struct timing *timing;
};

static void tick_timer(struct gui690tmp *gui) {
    if (gui->timing) {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        timersub(&tv, &gui->timing->tv_timer, &tv);
        gui->timing->nb_timed++;
        // last frame
        if (gui->timing->frame % gui->updlog == 0) {
            double m = gui->timing->maj.tv_sec + 0.000001 * gui->timing->maj.tv_usec;
            double l = gui->timing->layout.tv_sec + 0.000001 * gui->timing->layout.tv_usec;
            double d = gui->timing->delay.tv_sec + 0.000001 * gui->timing->delay.tv_usec;
            double e = gui->timing->expose.tv_sec + 0.000001 * gui->timing->expose.tv_usec;
            printf("Frame %ld : Timer : %ld | %ld Maj %.1fms (%lu) | %ld Layout %.3fms (%ld) | Queue %ld | %ld Expose %.2fms - %.2fms\n"
                    , gui->timing->frame
                    , gui->timing->nb_timed
                    , gui->timing->nb_maj, 1000. * m / gui->updlog, gui->timing->pixels / 1000
                    , gui->timing->nb_layout, 1000. * l / gui->updlog, gui->timing->ops_layout / gui->updlog
                    , gui->timing->nb_queued
                    , gui->timing->nb_exposed, 1000. * d / gui->updlog, 1000. * e / gui->updlog);
            memset(&gui->timing->timing, 0, sizeof(struct timeval));
            memset(&gui->timing->maj, 0, sizeof(struct timeval));
            memset(&gui->timing->layout, 0, sizeof(struct timeval));
            memset(&gui->timing->delay, 0, sizeof(struct timeval));
            memset(&gui->timing->expose, 0, sizeof(struct timeval));
            gui->timing->pixels = 0;
            gui->timing->nb_timed = 0;
            gui->timing->nb_maj = 0;
            gui->timing->nb_layout = 0;
            gui->timing->ops_layout = 0;
            gui->timing->nb_queued = 0;
            gui->timing->nb_exposed = 0;
        }

        //
        gui->timing->frame++;
        gettimeofday(&gui->timing->tv_timer, NULL);
    }
}
static void tick_maj1(struct gui690tmp *gui) {
    if (gui->timing) {
        struct timeval tv;
        gettimeofday(&gui->timing->tv_maj1, NULL);
        timersub(&gui->timing->tv_maj1, &gui->timing->tv_timer, &tv);
        timeradd(&gui->timing->timing, &tv, &gui->timing->timing);
    }
}
static void tick_maj2(struct gui690tmp *gui) {
    if (gui->timing) {
        struct timeval tv;
        gettimeofday(&gui->timing->tv_maj2, NULL);
        timersub(&gui->timing->tv_maj2, &gui->timing->tv_maj1, &tv);
        timeradd(&gui->timing->maj, &tv, &gui->timing->maj);
        gui->timing->nb_maj++;
    }
}
static void tick_layout1(struct gui690tmp *gui) {
    if (gui->timing) {
        struct timeval tv;
        gettimeofday(&gui->timing->tv_layout1, NULL);
        timersub(&gui->timing->tv_layout1, &gui->timing->tv_maj2, &tv);
        timeradd(&gui->timing->timing, &tv, &gui->timing->timing);
    }
}
static void tick_layout2(struct gui690tmp *gui, long ops) {
    if (gui->timing) {
        struct timeval tv;
        gettimeofday(&gui->timing->tv_layout2, NULL);
        timersub(&gui->timing->tv_layout2, &gui->timing->tv_layout1, &tv);
        timeradd(&gui->timing->layout, &tv, &gui->timing->layout);
        gui->timing->nb_layout++;
        gui->timing->ops_layout += ops;
    }
}
static void tick_queued(struct gui690tmp *gui) {
    if (gui->timing) {
        gettimeofday(&gui->timing->tv_queued, NULL);
        gui->timing->nb_queued++;
    }
}
static void tick_expose1(struct gui690tmp *gui) {
    if (gui->timing) {
        struct timeval tv;
        gettimeofday(&gui->timing->tv_exposed1, NULL);
        timersub(&gui->timing->tv_exposed1, &gui->timing->tv_queued, &tv);
        timeradd(&gui->timing->delay, &tv, &gui->timing->delay);
    }
}
static void tick_expose2(struct gui690tmp *gui) {
    if (gui->timing) {
        struct timeval tv;
        gettimeofday(&gui->timing->tv_exposed2, NULL);
        timersub(&gui->timing->tv_exposed2, &gui->timing->tv_exposed1, &tv);
        timeradd(&gui->timing->expose, &tv, &gui->timing->expose);
        gui->timing->nb_exposed++;
    }
}


static void exit_gui(struct gui690tmp *gui) {
    gtk_main_quit();
}

static gboolean delete_event(GtkWidget *widget, GdkEvent *event, struct gui690tmp *gui) {
    // false to destroy
    return FALSE;
}


static void destroy(GtkWidget *widget, struct gui690tmp *gui) {
    exit_gui(gui);
}

/*
 *
 */
static gboolean on_expose_event(GtkWidget *widget, GdkEventExpose *event, struct gui690tmp *gui) {
    tick_expose1(gui);
    return FALSE;
}
static gboolean after_expose_event(GtkWidget *widget, GdkEventExpose *event, struct gui690tmp *gui) {
    tick_expose2(gui);
    return FALSE;
}


static void maj_layout(struct gui690tmp *gui) {
    long l1, l2;
    //
    tick_layout1(gui);
    //
    int color = rand();
    color |= 0xff000000;
    l1 = ReadTSC();
//    imgfill1a650(&gui->desk->bgra, color, &zsta->pties);
    bgra_fill2650(&gui->desk->bgra, color);
    l2 = ReadTSC();
    //

    //
    tick_layout2(gui, l2 - l1);
}

static gboolean time_handler(struct gui690tmp *gui) {
//    tick_timer(gui);
//
//    maj_layout(gui);
//
//    tick_queued(gui);
//
//    gtk_widget_queue_draw(gui->window);
    return TRUE;
}


static gboolean motion_notify_event(GtkWidget *widget, GdkEventMotion *event, struct gui690tmp *gui) {
    GdkModifierType state;

    if (event->is_hint) {
        gdk_window_get_pointer(event->window, &gui->timing->mousex, &gui->timing->mousey, &state);
    } else {
        gui->timing->mousex = event->x;
        gui->timing->mousey = event->y;
        state = event->state;
    }
    return TRUE;
}

static gboolean key_press_event(GtkWidget *widget, GdkEventKey *event, struct gui690tmp *gui) {
    printf("Keyval = %d\n", event->keyval);
    switch(event->keyval) {
        case 32:                // space
            if (gui->brodge) brodge_rebase(gui->brodge);
            break;
        case 65307:             // ESC
            exit_gui(gui);
            break;
    }
    return TRUE;
}

static gboolean button_press_event(GtkWidget *widget, GdkEventButton *event, struct gui690tmp *gui) {
    if (gui->brodge && event->button == 1) {
        gui->timing->selection++;
        if (gui->timing->selection == gui->brodge->nb_src) {
            gui->timing->selection = -1;
        }
    } else if (event->button == 3) {
        gui->timing->refresh = !gui->timing->refresh;
    }
    return TRUE;
}

static gboolean button_release_event(GtkWidget *widget, GdkEventButton *event, struct gui690tmp *gui) {
    if (event->button == 1) {
    }
    return TRUE;
}


static struct gui690tmp *create_gui690(int width, int height, int rms) {
    struct gui690tmp *new = calloc(1, sizeof(struct gui690tmp));

    // Desk
    new->desk = desk_create654(width, height);
    bgra_fill650(&new->desk->bgra, 0xff0000ff);
    FOG("desk create ok :");

    // Window
    new->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(new->window), "gui");
    gtk_window_set_position(GTK_WINDOW(new->window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(new->window), 0);
    gtk_window_set_default_size(GTK_WINDOW(new->window), width, height);
    gtk_window_set_decorated(GTK_WINDOW(new->window), FALSE);
    gtk_window_set_keep_above(GTK_WINDOW(new->window), TRUE);
    FOG("window ok");

    // Image
    void pbd(guchar *pixels, gpointer data) { printf("PixBuf free\n"); return; }
    new->img = gdk_pixbuf_new_from_data((guchar*) new->desk->bgra.data, GDK_COLORSPACE_RGB, TRUE, 8, width, height, width << 2, &pbd, NULL);
    printf("PixBuf new ok\n");
    new->frame = gtk_image_new_from_pixbuf(new->img);
    gtk_container_add(GTK_CONTAINER(new->window), new->frame);
    FOG("image ok");

    // Events
    gtk_widget_set_events(
            new->window
            , GDK_EXPOSURE_MASK
            | GDK_LEAVE_NOTIFY_MASK
            | GDK_KEY_PRESS_MASK
            | GDK_BUTTON_PRESS_MASK   | GDK_BUTTON_RELEASE_MASK
            | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK
    );

    // Signals
    g_signal_connect(new->frame, "expose-event", G_CALLBACK(on_expose_event), new);
    g_signal_connect_after(new->frame, "expose-event", G_CALLBACK(after_expose_event), new);

    g_signal_connect(new->window, "key_press_event", G_CALLBACK (key_press_event), new);
    g_signal_connect(new->window, "motion_notify_event", G_CALLBACK (motion_notify_event), new);
    g_signal_connect(new->window, "button_press_event", G_CALLBACK (button_press_event), new);
    g_signal_connect(new->window, "button_release_event", G_CALLBACK (button_release_event), new);

    g_signal_connect(new->window, "delete-event", G_CALLBACK(delete_event), new);
    g_signal_connect(new->window, "destroy", G_CALLBACK(destroy), new);
    FOG("Signals connected");

    //
    new->timing = calloc(1, sizeof(struct timing));
    new->timing->refresh = 1;
    new->timing->selection = -1;

    //
    new->timer_delay = rms;

    FOG("Gui created");
    return new;
}

/*
 *
 */
int main(int argc, char *argv[]) {
    //
    gtk_init(&argc, &argv);

    //
    struct gui690tmp *tmp = create_gui690(800, 448, 65);


    // Show
    gtk_widget_show(tmp->frame);
    gtk_widget_show_all(tmp->window);

    // Timer
    //g_timeout_add(tmp->timer_delay, (GSourceFunc)time_handler, (gpointer)tmp);

    //
    FOG("Going to gtk_main");
    gtk_main();
    FOG("Exit gtk_main");

    return 0;
}

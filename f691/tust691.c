/*
 * file    : tust691.c
 * project : f640 (f642 / t508.f640)
 *
 * Created on: Sep 10, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#include <xcb/xcb.h>
#include "f691.h"

#include "../f650/f650.h"
#include "../f650/brodge/brodge650.h"
#include "../f650/layout/f650_layout.h"


typedef struct {
    int         width;
    int         height;

    // Xcb
    xcb_connection_t    *cn;
    xcb_screen_t        *screen;
    xcb_window_t        window;

    //
    int             shm;
    int             shm_flag;
    XShmSegmentInfo shm_info;

    //
    XImage      *ximg;
    bgra650     bgra;


    //
} x11gui691;

static int xerror = 0;
static int handle_xerror(Display* display, XErrorEvent* err) {
    FOG("Error %d", err->type)
    xerror = 1;
    return 0;
}

int create_gui(x11gui691 *gui, int width, int height, int shm) {
    int r;

    // Connection
    int screen_nbr;
    gui->cn = xcb_connect(":0", &screen_nbr);
    FOG("xcb_connect return %p, for %d screen_nbr", gui->cn, screen_nbr);

    // Screen
    xcb_screen_iterator_t iter;

    // Get the first screen
//    gui->screen = xcb_setup_roots_iterator (xcb_get_setup (c)).data;

    // Get the screen #screen_nbr
    iter = xcb_setup_roots_iterator(xcb_get_setup (gui->cn));
    for (; iter.rem; --screen_nbr, xcb_screen_next(&iter)) {
        if (screen_nbr == 0) {
          gui->screen = iter.data;
          break;
        }
        printf ("\n");
        printf ("Informations of screen %ld:\n", iter.data->root);
        printf ("  width.........: %d\n", iter.data->width_in_pixels);
        printf ("  height........: %d\n", iter.data->height_in_pixels);
        printf ("  white pixel...: %ld\n", iter.data->white_pixel);
        printf ("  black pixel...: %ld\n", iter.data->black_pixel);
        printf ("\n");
    }

    printf ("--------------------------------------------------------\n");
    printf ("Informations of screen %ld:\n", gui->screen->root);
    printf ("  width.........: %d\n", gui->screen->width_in_pixels);
    printf ("  height........: %d\n", gui->screen->height_in_pixels);
    printf ("  white pixel...: %ld\n", gui->screen->white_pixel);
    printf ("  black pixel...: %ld\n", gui->screen->black_pixel);
    printf ("  Dim in mm....:  %ux%u\n", gui->screen->width_in_millimeters, gui->screen->height_in_millimeters);
    printf ("\n");

    //
    gui->window = xcb_generate_id(gui->cn);
    FOG("xcb_generate_id return %d", gui->window);

    //
    xcb_void_cookie_t coo = xcb_create_window(gui->cn,
         XCB_COPY_FROM_PARENT, gui->window, gui->screen->root
         , 0, 0, width, height, 0
         , XCB_WINDOW_CLASS_INPUT_OUTPUT
         , gui->screen->root_visual
         , 0, NULL);
    FOG("xcb_create_window return %d", coo.sequence);

    //
    coo = xcb_map_window(gui->cn, gui->window);
    FOG("xcb_map_window return %d", coo.sequence);

    // Make sure commands are sent before we pause, so window is shown
    r = xcb_flush(gui->cn);
    FOG("xcb_flush return %d", r);

    // hold client until Ctrl-C
    pause();

    //
    FOG("Exit from pause");
    return 0;
}

int main1() {
    int r;
    x11gui691 *gui = calloc(1, sizeof(x11gui691));

    r = create_gui(gui, 800, 448, 1);
    FOG("Create window return %d", r);

//    //
//    XEvent evt0;
//    XExposeEvent evt;
//    XGraphicsExposeEvent egt;
////    r = XWindowEvent(gui->display, gui->window, 0, &evt);
////    LOG("XWindowEvent return %d", r);
//
//    //
//    long l = 0;
//    struct timeval tv1, tv2;
//    brodge650 *brodge = brodge_init(gui->width, gui->height, 2);
//    gettimeofday(&tv2, NULL);
//    while(1) {
//        brodge_anim(brodge);
//        brodge_exec(brodge, &gui->bgra);
//        //
//        gettimeofday(&tv1, NULL);
//        //
//        if (gui->ximg && !gui->shm) {
//            r = XPutImage(gui->display, gui->window, gui->gc, gui->ximg
//                    , 0, 0, 0, 0, gui->ximg->width, gui->ximg->height);
//        } else if (gui->ximg && gui->shm) {
//            r = XShmPutImage(gui->display, gui->window, gui->gc, gui->ximg
//                    , 0, 0, 0, 0, gui->ximg->width, gui->ximg->height, 1);
//            //
//            r = XFlush(gui->display);
//        }
//        //
//        XSync(gui->display, 0);
//
//        usleep(10000);
//    }
//
//
//    usleep(5000000);

}

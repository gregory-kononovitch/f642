/*
 * file    : xgui650_test2.c
 * project : f640 (| f642 | t508.f640)
 *
 * Created on: Sep 12, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */



#include "../f650.h"
#include "../f650_fonts.h"
#include "../brodge/brodge650.h"
#include "../layout/f650_layout.h"
#include "../../f691/f691.h"



static int notify_key_pressed0(void *ext, unsigned long key, int x, int y, int mask, uint64_t time) {
    desk654 *desk = (desk654*)ext;

    return 0;
}

static int notify_key_released0(void *ext, unsigned long key, int x, int y, int mask, uint64_t time) {
    desk654 *desk = (desk654*)ext;


    return 0;
}

// Mouse
static int notify_mouse_motion0(void *ext, int x, int y, int mask, uint64_t time) {
    desk654 *desk = (desk654*)ext;

    return 0;
}

static int notify_button_pressed0(void *ext, int button, int x, int y, int mask, uint64_t time) {
    desk654 *desk = (desk654*)ext;


    return 0;
}

static int notify_button_released0(void *ext, int button, int x, int y, int mask, uint64_t time) {
    desk654 *desk = (desk654*)ext;


    return 0;
}

static int notify_scroll0(void *ext, int button, int x, int y, int mask, uint64_t time) {
    desk654 *desk = (desk654*)ext;


    return 0;
}


/*
 *
 */
static int test() {
    int r;

    //
    Status st = XInitThreads();
    FOG("XInitThreads return %d",st);
    //
//    XrmInitialize();
//    FOG("XrmInitialize done");

    // Gui
    xgui691 *gui = xgui_create691(752, 416, 1);
    if (!gui) return -1;
    long period = 57143;
    //
    r = xgui_open_window691(gui, "Test");
    if (r) { LOG("Can't open window, returning"); return -1; }
    LOG("Window opened %p :%dx%d", gui->pix1, gui->width, gui->height);

    // Bgra
    bgra650 bgra;
    bgra_link650(&bgra, gui->pix1, gui->width, gui->height);
    FOG("Bgra linked");

    // Desk
    desk654 *desk = desk_create654(gui->width, gui->height);
    FOG("desk created");
    // Center
    zone654 *zimg = zone_add_item(desk, desk->root, 2);
    zone_set_dimension(zimg, 8 * gui->width / 10, 8 * gui->height / 10);
    zone_set_location(zimg, (gui->width - zimg->pties.dim.width) / 2, (gui->height - zimg->pties.dim.height) / 2);
    // Widget1
    zone654 *zsta = zone_add_item(desk, desk->root, 1);
    zone_set_location(zsta, 22, 5);
    zone_set_dimension(zsta, 24, 24);
    //
    desk_layout(desk);
    FOG("desk layout");
    desk_dump(desk);


    // Events
    events691 events;
    memset(&events, 0, sizeof(events691));
    events.notify_key_pressed       = notify_key_pressed0;
    events.notify_key_pressed       = notify_key_released0;
    events.notify_mouse_motion      = notify_mouse_motion0;
    events.notify_button_pressed    = notify_button_pressed0;
    events.notify_button_released   = notify_button_released0;
    events.notify_scroll            = notify_scroll0;
    events.notify_key_pressed       = notify_key_released0;
    events.notify_key_pressed       = notify_key_pressed0;
    events.notify_key_pressed       = notify_key_released0;
    xgui_listen691(gui, &events, NULL);

    // Loop
    int i;
    long l;
    struct timeval tv0, tv1, tv2, tv3, tv4;
    struct timeval tvb1, tvb2;
    long broad;
    long frame = 0;
    gettimeofday(&tv0, NULL);
    gettimeofday(&tv1, NULL);
    gettimeofday(&tv3, NULL);
    while(1) {
        //
        gettimeofday(&tvb1, NULL);
        show691(gui, 0, 0, 0, 0, 0, gui->width, gui->height);
        gettimeofday(&tvb2, NULL);
        timersub(&tvb2, &tvb1, &tvb2);
        broad += tvb2.tv_usec;
        //

        //
        frame++;
        //
        tv0.tv_usec += period;
        while(tv0.tv_usec > 999999) {
            tv0.tv_sec++;
            tv0.tv_usec -= 1000000;
        }
        //
        gettimeofday(&tv2, NULL);
        timersub(&tv0, &tv2, &tv1);
        if (tv1.tv_sec > -1) {     // @@@ manage ~=
            usleep(tv1.tv_usec + 1000000L * tv1.tv_sec);
        }
        //
        if (frame % 70 == 0) {
            gettimeofday(&tv4, NULL);
            timersub(&tv4, &tv3, &tv4);
            LOG("Frame %ld for %.3f Hz for %ld µs", frame, 70. / (1. * tv4.tv_sec + 0.000001 * tv4.tv_usec), broad / 70);
            broad = 0;
            gettimeofday(&tv3, NULL);
        }
        //
    }
    //
    xgui_stop691(gui);
    xgui_close_window691(gui);
    xgui_free691(&gui);
    return 0;
}



int main() {
    test();

    return 0;
}

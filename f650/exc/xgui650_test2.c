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


typedef struct _xgui_test2_ {

    // L&F
    color650    c_bg;
    color650    c_fg;
    color650    c_wbg;
    color650    c_wfg;
    color650    c_wbp;
    color650    c_wbs;

    //
    char        run;
} t2st654;

static int notify_key_pressed0(void *ext, unsigned long key, int x, int y, int mask, uint64_t time) {
    desk654 *desk = (desk654*)ext;

    switch(key) {
        case XK_Escape:
            ((t2st654*)desk->appli)->run = 0;
            break;
    }
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


static void make_desk(desk654 *desk) {
    int i, x, y;
    int b = 1;
    zone654 *z;

    // Center
    zone654 *center = zone_add_item(desk, desk->root, 2);
    zone_set_dimension(center, 8 * desk->width / 10, 8 * desk->height / 10);
    zone_set_location(center, (desk->width - center->pties.dim.width) / 2, (desk->height - center->pties.dim.height) / 2);
    zone_set_borders(center, b, b, b, b);

    // North
    x = center->pties.posr.x;
    y = 10;
    while(x + 20 < center->pties.posr.x + center->pties.dim.width) {
        z = zone_add_item(desk, desk->root, 1);
        zone_set_location(z, x, y);
        zone_set_dimension(z, 20, 20);
        zone_set_borders(z, b, b, b, b);
        x += 20 + 2;
    }
    // East
    x = desk->root->pties.hborder.left;
    y = center->pties.posr.y;
    z = zone_add_item(desk, desk->root, 1);
    zone_set_location(z, x, y);
    zone_set_dimension(z, 16, center->pties.dim.height/2 - 2);
    zone_set_borders(z, b, b, b, b);
    //
    y += center->pties.dim.height/2 + 2;
    z = zone_add_item(desk, desk->root, 1);
    zone_set_location(z, x, y);
    zone_set_dimension(z, 16, center->pties.dim.height/2 - 2);
    zone_set_borders(z, b, b, b, b);

    // West
    x = desk->root->pties.dim.width - desk->root->pties.hborder.right - 16;
    y = center->pties.posr.y;
    z = zone_add_item(desk, desk->root, 1);
    zone_set_location(z, x, y);
    zone_set_dimension(z, 16, center->pties.dim.height/2 - 2);
    zone_set_borders(z, b, b, b, b);
    //
    y += center->pties.dim.height/2 + 2;
    z = zone_add_item(desk, desk->root, 1);
    zone_set_location(z, x, y);
    zone_set_dimension(z, 16, center->pties.dim.height/2 - 2);
    zone_set_borders(z, b, b, b, b);

    // South
    z = zone_add_item(desk, desk->root, 1);
    zone_set_location(z, desk->root->pties.hborder.left, desk->root->pties.dim.height - desk->root->pties.vborder.below - 20);
    zone_set_dimension(z, desk->root->pties.dim.width - desk->root->pties.hborder.left - desk->root->pties.hborder.right, 20);
    zone_set_borders(z, b, b, b, b);

    //
    desk_layout(desk);
    FOG("desk layed out");
    desk_dump(desk);
}

static long draw_layout_iter(desk654 *desk, zone654 *head, t2st654 *appli) {
    long l = 0;
    zone654 *z;
    if (!head) return 0;
    // Head
    imgfill1a650(&desk->bgra, appli->c_wbp.value, &head->pties.out);
    imgfill1a650(&desk->bgra, appli->c_wbg.value, &head->pties.in);
    //
    z = head->links.items;
    while(z) {
        l += draw_layout_iter(desk, z, appli);
        z = z->links.next;
    }
    return 0;
}

static long draw_layout(desk654 *desk, t2st654 *appli) {
    long l = 0;
    zone654 *z = desk->root->links.items;

    while(z) {
        l += draw_layout_iter(desk, z, appli);
        z = z->links.next;
    }
    return l;
}

/*
 *
 */
static int test() {
    int r;

    //
    t2st654 appli;
    appli.c_bg.value  = BLACK650;
    appli.c_fg.value  = 0xff60c010;
    appli.c_wbg.value = 0xff203030;
    appli.c_wfg.value = 0xff60c010;
    appli.c_wbp.value = 0xff60c010;
    appli.c_wbs.value = 0xffc01010;

    //
    Status st = XInitThreads();
    FOG("XInitThreads return %d",st);
    //
//    XrmInitialize();
//    FOG("XrmInitialize done");

    // Gui
    xgui691 *gui = xgui_create691(800, 448, 1);
    if (!gui) return -1;
    long period = F691_20HZ;
    //
    r = xgui_open_window691(gui, "Test");
    if (r) { LOG("Can't open window, returning"); return -1; }
    LOG("Window opened %p :%dx%d", gui->pix1, gui->width, gui->height);

    // Desk
    desk654 *desk = desk_create654(gui->width, gui->height);
    desk->appli = &appli;
    FOG("desk created");
    bgra_link650(&desk->bgra, gui->pix1, gui->width, gui->height);
    FOG("Bgra linked");
    make_desk(desk);

    // Events
    events691 events;
    memset(&events, 0, sizeof(events691));
    events.notify_key_pressed       = notify_key_pressed0;
    events.notify_key_released      = notify_key_released0;
    events.notify_mouse_motion      = notify_mouse_motion0;
    events.notify_button_pressed    = notify_button_pressed0;
    events.notify_button_released   = notify_button_released0;
    events.notify_scroll            = notify_scroll0;
    xgui_listen691(gui, &events, desk);

    // Loop
    int i;
    long l;
    struct timeval tv0, tv1, tv2, tv3, tv4;
    struct timeval tvb1, tvb2;
    long draw, broad;
    long frame = 0;
    gettimeofday(&tv0, NULL);
    gettimeofday(&tv1, NULL);
    gettimeofday(&tv3, NULL);
    appli.run = 1;
    while(appli.run) {
        //
        bgra_fill650(&desk->bgra, appli.c_bg.value);
        gettimeofday(&tvb1, NULL);
        draw_layout(desk, &appli);
        gettimeofday(&tvb2, NULL);
        timersub(&tvb2, &tvb1, &tvb2);
        draw += tvb2.tv_usec;
        frame++;

        //
        broad += show691(gui, 0, 0, 0, 0, 0, gui->width, gui->height);

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
            LOG("Frame %ld for %.3f Hz for draw = %ld µs ; broad = %ld µs", frame, 70. / (1. * tv4.tv_sec + 0.000001 * tv4.tv_usec), draw / 70, broad / 70);
            draw  = 0;
            broad = 0;
            gettimeofday(&tv3, NULL);
        }
        //
    }


end:
    //
    FOG("gui = %p", gui);
    xgui_stop691(gui);
    xgui_close_window691(gui);
    xgui_free691(&gui);
    return 0;
}



int main() {
    test();

    return 0;
}

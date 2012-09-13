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


static void make_desk(desk654 *desk) {
    int i, x, y;
    zone654 *z;

    // Center
    zone654 *center = zone_add_item(desk, desk->root, 2);
    zone_set_dimension(center, 8 * desk->width / 10, 8 * desk->height / 10);
    zone_set_location(center, (desk->width - center->pties.dim.width) / 2, (desk->height - center->pties.dim.height) / 2);

    // North
    x = center->pties.posr.x;
    y = 10;
    while(x + 20 < center->pties.posr.x + center->pties.dim.width) {
        z = zone_add_item(desk, desk->root, 1);
        zone_set_location(z, x, y);
        zone_set_dimension(z, 20, 20);
        zone_set_borders(z, 2, 2, 2, 2);
        x += 20 + 2;
    }
    // East
    x = 10;
    y = center->pties.posr.y;
    z = zone_add_item(desk, desk->root, 1);
    zone_set_location(z, x, y);
    zone_set_dimension(z, 16, center->pties.dim.height/2 - 2);
    zone_set_borders(z, 2, 2, 2, 2);
    //
    y += center->pties.dim.height/2 + 2;
    z = zone_add_item(desk, desk->root, 1);
    zone_set_location(z, x, y);
    zone_set_dimension(z, 16, center->pties.dim.height/2 - 2);
    zone_set_borders(z, 2, 2, 2, 2);

    // West
    x = desk->root->pties.dim.width - 10;
    y = center->pties.posr.y;
    z = zone_add_item(desk, desk->root, 1);
    zone_set_location(z, x, y);
    zone_set_dimension(z, 16, center->pties.dim.height/2 - 2);
    zone_set_borders(z, 2, 2, 2, 2);
    //
    y += center->pties.dim.height/2 + 2;
    z = zone_add_item(desk, desk->root, 1);
    zone_set_location(z, x, y);
    zone_set_dimension(z, 16, center->pties.dim.height/2 - 2);
    zone_set_borders(z, 2, 2, 2, 2);

    // South
    z = zone_add_item(desk, desk->root, 1);
    zone_set_location(z, center->pties.posr.x, desk->root->pties.dim.height - 28);
    zone_set_dimension(z, center->pties.dim.width, 20);
    zone_set_borders(z, 2, 2, 2, 2);

    //
    desk_layout(desk);
    FOG("desk layed out");
    desk_dump(desk);
}

static long draw_layout_iter(desk654 *desk, zone654 *head, long cin, long cbor) {
    long l = 0;
    zone654 *z;
    if (!head) return 0;
    // Head
    imgfill1a650(&desk->bgra, cin, &head->pties.out);
    //
    z = head->links.items;
    while(z) {
        l += draw_layout_iter(desk, z, cin, cbor);
        z = z->links.next;
    }
    return 0;
}

static long draw_layout(desk654 *desk, long cin, long cbor) {
    long l = 0;
    zone654 *z = desk->root->links.items;

    while(z) {
        l += draw_layout_iter(desk, z, cin, cbor);
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

    // Desk
    desk654 *desk = desk_create654(gui->width, gui->height);
    FOG("desk created");
    bgra_link650(&desk->bgra, gui->pix1, gui->width, gui->height);
    FOG("Bgra linked");
    make_desk(desk);

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
        int cins = 0xff505070;
        int cbor = 0xff80ff10;
        bgra_fill650(&desk->bgra, 0xff000000);
        draw_layout(desk, cins, cbor);
        frame++;

        //
        gettimeofday(&tvb1, NULL);
        show691(gui, 0, 0, 0, 0, 0, gui->width, gui->height);
        gettimeofday(&tvb2, NULL);
        timersub(&tvb2, &tvb1, &tvb2);
        broad += tvb2.tv_usec;

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


end:
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

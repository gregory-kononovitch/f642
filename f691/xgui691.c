/*
 * file    : xgui691.c
 * project : f640 (f642 | t508.f640)
 *
 * Created on: Sep 10, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */



#include "f691.h"


static int xerror = 0;
static int handle_xerror(Display* display, XErrorEvent* err) {
    FOG("Error %u (%u), request %u, serial %lu, type %d"
            , err->error_code, err->minor_code, err->request_code
            , err->serial, err->type);
    xerror = 1;
    return 0;
}


xgui691 *xgui_create691(int width, int height, int shm) {
    int r;

    xgui691 *gui = calloc(1, sizeof(xgui691));
    if (!gui) {
        LOG("ENOMEM creating gui, returning");
        return NULL;
    }

    //
    gui->width  = width;
    gui->height = height;
    gui->shm    = shm;
    gui->ximg1  = NULL;
    gui->ximg2  = NULL;

    //
    XSetErrorHandler(&handle_xerror);

    // Open display
    gui->display = XOpenDisplay(0);
    if (!gui->display || xerror) {
        LOG("Cannot open display, returning");
        free(gui);
        return NULL;
    }

    // Trace
    XSynchronize(gui->display, True);

    // Screen
    gui->screen = DefaultScreen(gui->display);
    if (xerror) {
        LOG("Cannot get screen, returning");
        XCloseDisplay(gui->display);
        free(gui);
        return NULL;
    }

    //
    if (XMatchVisualInfo(gui->display, gui->screen, 24, TrueColor, &gui->vinfo)) {
        FOG("Found Match TrueColor depth 24 : %d bits per rgb", gui->vinfo.bits_per_rgb);
    } else {
        LOG("No visual found, returning");
        XCloseDisplay(gui->display);
        free(gui);
        return NULL;
    }


    // GC
    gui->gc = DefaultGC(gui->display, gui->screen);
    FOG("Get default GC");


    // Shm
    if (shm && XShmQueryExtension(gui->display)) {
        FOG("XShmQueryExtension : ok");
        gui->shm = 1;
    } else {
        FOG("XShmQueryExtension : no");
        shm = 0;
        gui->shm = 0;
    }

    while (shm) {
        //
        gui->shm_info.shmid = shmget(IPC_PRIVATE, 2 * 4 * width * height, IPC_CREAT | 0777);
        FOG("shmget return %d - %p", gui->shm_info.shmid, gui->shm_info.shmaddr);
        //
        gui->shm_info.shmaddr = (char*)shmat(gui->shm_info.shmid, 0, 0);
        FOG("shmat return %p", gui->shm_info.shmaddr);
        //
        gui->shm_info.readOnly = False;
        Status st = XShmAttach(gui->display, &gui->shm_info);
        FOG("XShmAttach return %d - %p", st, gui->shm_info.shmaddr);
        if (st == False || xerror) {
            LOG("Failed to attach shm memory");
            shm = 0;
            gui->shm = 0;
            shmdt(gui->shm_info.shmaddr);
            break;
        }
        r = XSync(gui->display, False);
        FOG("XSync return %d", r);
        //
        r = shmctl(gui->shm_info.shmid, IPC_RMID, 0);
        FOG("shmctl return %d", r);

        //
        gui->ximg1 = XShmCreateImage(gui->display, gui->vinfo.visual
                , gui->vinfo.depth, ZPixmap, gui->shm_info.shmaddr
                , &gui->shm_info
                , gui->width, gui->height
        );
        memcpy(&gui->shm_info2, sizeof(XShmSegmentInfo), &gui.shm_info);
        gui->ximg2 = XShmCreateImage(gui->display, gui->vinfo.visual
                , gui->vinfo.depth, ZPixmap, gui->shm_info.shmaddr + (4 * width * height)
                , &gui->shm_info2
                , gui->width, gui->height
        );
        FOG("XShmCreateImage2 return %p, err = %d", gui->ximg2, xerror);
        if (gui->ximg1 && gui->ximg2 && !xerror) {
            LOG("XShmImages created");
            break;
        } else {
            shm = 0;
            gui->shm = 0;
            if (!gui->ximg1) LOG("Failed to create shm-img1");
            if (!gui->ximg2) LOG("Failed to create shm-img2");
            if (xerror) LOG("Error creating shm-img's");
            shmdt(gui->shm_info.shmaddr);
            break;
        }
    }
    if (!gui->shm) {
        char *tmp = calloc(2, 4 * width * height);
        gui->ximg1 = XCreateImage(gui->display, gui->vinfo.visual
                , gui->vinfo.depth, ZPixmap, 0
                , tmp
                , gui->width, gui->height
                , gui->vinfo.visual->bits_per_rgb
                , 4 * gui->width
                );
        FOG("XCreateImage 1 return %p", gui->ximg1);
        if (!gui->ximg1 || xerror) {
            LOG("Failed to create XImage img1");
            free(tmp);
            XCloseDisplay(gui->display);
            free(gui);
        }
        //
        tmp += 4 * width * height;
        gui->ximg2 = XCreateImage(gui->display, gui->vinfo.visual
                , gui->vinfo.depth, ZPixmap, 0
                , tmp
                , gui->width, gui->height
                , gui->vinfo.visual->bits_per_rgb
                , 4 * gui->width
                );
        FOG("XCreateImage 2 return %p", gui->ximg1);
        if (!gui->ximg2 || xerror) {
            LOG("Failed to create XImage img1");
            free(tmp);
            XCloseDisplay(gui->display);
            free(gui);
        }
    }

    //
    r = XSync(gui->display, False);
    FOG("XSync return %d", r);

    //
    return gui;
}


int xgui_open_window691(xgui691 *gui, char *title) {
    // Window
    int x, y;
    unsigned int w, h, b, d;
    Window root_window;
    XGetGeometry(gui->display, DefaultRootWindow(gui->display), &root_window, &x, &y, &w, &h, &b, &d);
    gui->hint.x      = x + ( (w - gui->width) < 0 ? 0 : (w - gui->width) / 2);
    gui->hint.x      = y + ( (h - gui->height) < 0 ? 0 : (h - gui->height) / 2);
    gui->hint.width  = gui->width;
    gui->hint.height = gui->height;
    gui->hint.flags  = PPosition | PSize;

    //
    gui->color_map = XCreateColormap(gui->display, DefaultRootWindow(gui->display), gui->vinfo.visual, AllocNone);
    FOG("CMap created");
    XSetWindowAttributes xswa;
    xswa.colormap     = gui->color_map;
    xswa.event_mask   = StructureNotifyMask;
    xswa.border_pixel = BlackPixel(gui->display, gui->screen);

    gui->window = XCreateWindow(gui->display, DefaultRootWindow(gui->display)
            , gui->hint.x, gui->hint.y, gui->hint.width, gui->hint.height, 0
            , gui->vinfo.depth, InputOutput
            , gui->vinfo.visual
            , CWBorderPixel | CWColormap | CWEventMask, &xswa);
    if (xerror) {
        LOG("Cannot create window, exiting");
        XCloseDisplay(gui->display);
        free(gui);
        return NULL;
    }
    FOG("Window created");

    // Event
    XSetStandardProperties(gui->display, gui->window, title, title, None, NULL, 0, &gui->hint);
    FOG("Standrd properties set");

    // Map gui->window
    XMapWindow(gui->display, gui->window);
    FOG("Map window wait");

    // Wait for map.
    XEvent event;
    do {
        XNextEvent(gui->display, &event);
    } while (event.type != MapNotify || event.xmap.event != gui->window);
    FOG("Map window done");

    XSelectInput(gui->display, gui->window, NoEventMask);

}




int main() {

}

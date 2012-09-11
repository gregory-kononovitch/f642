/*
 * file    : xgui691.c
 * project : f640 (f642 | t508.f640)
 *
 * Created on: Sep 10, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>

#include <X11/Xproto.h>

#include "f691.h"

static long mask =
        KeyPressMask
      | KeyReleaseMask
      | ButtonPressMask
      | ButtonReleaseMask
      | EnterWindowMask
      | LeaveWindowMask
      | PointerMotionMask
//      | PointerMotionHintMask
      | Button1MotionMask
      | ButtonMotionMask
      | ExposureMask
      | VisibilityChangeMask
      | StructureNotifyMask
      | ResizeRedirectMask
      | FocusChangeMask
      | PropertyChangeMask
      | ColormapChangeMask
      | OwnerGrabButtonMask
      | (1L << 32)
;

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
        XSetErrorHandler(NULL);
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
        XSetErrorHandler(NULL);
        return NULL;
    }

    //
    if (XMatchVisualInfo(gui->display, gui->screen, 24, TrueColor, &gui->vinfo)) {
        FOG("Found Match TrueColor depth 24 : %d bits per rgb", gui->vinfo.bits_per_rgb);
    } else {
        LOG("No visual found, returning");
        XCloseDisplay(gui->display);
        free(gui);
        XSetErrorHandler(NULL);
        return NULL;
    }


    // GC
    gui->gc = DefaultGC(gui->display, gui->screen);
    FOG("Get default GC");


    // Shm
    if (shm && XShmQueryExtension(gui->display)) {
        FOG("XShmQueryExtension : ok");
        XExtCodes *shm_codes = XInitExtension(gui->display, SHMNAME);
        FOG("XInitExtension(" SHMNAME ") return ext = %d, opcode = %d, event1 = %d, error1 = %d", shm_codes->extension, shm_codes->major_opcode, shm_codes->first_event, shm_codes->first_error);
        r = XShmGetEventBase(gui->display);
        FOG("XShmGetEventBase return %d, XShmCompletionEvent ?", r);
        gui->shm = 1;
    } else {
        FOG("XShmQueryExtension : no");
        shm = 0;
        gui->shm = 0;
    }

    while (shm) {
        // ###
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
        FOG("XShmCreateImage 1 return img %p, pix %p,  err = %d", gui->ximg1, gui->ximg1->data, xerror);        memcpy(&gui->shm_info2, &gui->shm_info, sizeof(XShmSegmentInfo));
        gui->ximg2 = XShmCreateImage(gui->display, gui->vinfo.visual
                , gui->vinfo.depth, ZPixmap, gui->shm_info.shmaddr + (4 * width * height)
                , &gui->shm_info2
                , gui->width, gui->height
        );
        FOG("XShmCreateImage 2 return img %p, pix %p,  err = %d", gui->ximg2, gui->ximg2->data, xerror);
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
        FOG("XCreateImage 1 return img %p, pix %p", gui->ximg1, gui->ximg1->data);
        if (!gui->ximg1 || xerror) {
            LOG("Failed to create XImage img1");
            free(tmp);
            XCloseDisplay(gui->display);
            free(gui);
            XSetErrorHandler(NULL);
            return NULL;
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
        FOG("XCreateImage 2 return img %p, pix %p", gui->ximg2, gui->ximg2->data);
        if (!gui->ximg2 || xerror) {
            LOG("Failed to create XImage img1");
            free(tmp);
            XCloseDisplay(gui->display);
            free(gui);
            XSetErrorHandler(NULL);
            return NULL;
        }
    }

    //
    r = XSync(gui->display, False);
    FOG("XSync return %d", r);

    //
    XSetErrorHandler(NULL);

    //
    return gui;
}


void xgui_free691(xgui691 **gui) {
    if ((*gui)->window) xgui_close_window691(*gui);
    //
    if ((*gui)->shm) {
        XShmDetach((*gui)->display, &(*gui)->shm_info);
        XSync((*gui)->display, True);
        //
        shmdt((*gui)->shm_info.shmaddr);
        free((*gui)->ximg1);
        free((*gui)->ximg2);
    } else {
        free((*gui)->ximg1->data);
        free((*gui)->ximg2->data);
        free((*gui)->ximg1);
        free((*gui)->ximg2);
    }
    //
    XCloseDisplay((*gui)->display);
    free(*gui);
    *gui = NULL;
    return;
}

int xgui_open_window691(xgui691 *gui, const char *title) {
    // Window
    int x, y;
    unsigned int w, h, b, d;
    Window root_window;
    //
    xerror = 0;
    XSetErrorHandler(&handle_xerror);
    //
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
    xswa.event_mask   = mask;
    xswa.border_pixel = BlackPixel(gui->display, gui->screen);

    gui->window = XCreateWindow(gui->display, DefaultRootWindow(gui->display)
            , gui->hint.x, gui->hint.y, gui->hint.width, gui->hint.height, 0
            , gui->vinfo.depth, InputOutput
            , gui->vinfo.visual
            , CWBorderPixel | CWColormap | CWEventMask, &xswa);
    if (xerror) {
        LOG("Cannot create window, exiting");
        XSetErrorHandler(NULL);
        return -1;
    }
    FOG("Window created");

    // Event
    XSelectInput(gui->display, gui->window, mask);
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

//    XSelectInput(gui->display, gui->window, NoEventMask);

    XSetErrorHandler(NULL);
    return 0;
}


int xgui_close_window691(xgui691 *gui) {
    XUnmapWindow(gui->display, gui->window);
    XDestroyWindow(gui->display, gui->window);
    gui->window = 0;
    return 0;
}


/* --------------------------------------------------------
 *
 *                      EVENTS
 *
   -------------------------------------------------------- */
static void *event_loop691(void *prm);
struct _event_thread691_;
typedef int (*event691)(struct _event_thread691_ *thread, XEvent *evt);
struct _event_thread691_ {
    xgui691         *gui;
    int             run;
    long            types_mask;

    //
    event691        procs[LASTEvent];

    //
    pthread_t       thread;
    pthread_mutex_t mutex;
};
static struct _event_thread691_ event_thread691;

//
static int repeat_test[LASTEvent + 10000];
static void clear_test691() {
    memset(repeat_test, 0, sizeof(repeat_test));
}
static int event_test_timing691(struct _event_thread691_ *thread, XEvent *evt) {
    if (evt->type > (sizeof(repeat_test) >> 2)) return -1;
    repeat_test[evt->type]++;
    return 0;
}

//
int xgui_listen691(xgui691 *gui) {

    //
    event_thread691.gui = gui;
    event_thread691.types_mask = 0xFFFFFFFFFFFFFFL;
    event_thread691.run = 0;
    int i;
    for(i = 0 ; i < LASTEvent ; i++) {
        event_thread691.procs[i] = &event_test_timing691;
    }
    pthread_mutex_init(&event_thread691.mutex, NULL);
    clear_test691();

//    event_thread691.dpy = XOpenDisplay(":0");
////    XMapWindow(event_thread691.dpy, gui->window);
////    sleep(500000);

    //
    event_thread691.run = 1;
    pthread_create(&event_thread691.thread, NULL, &event_loop691, &event_thread691);

    //
    XSelectInput(gui->display, gui->window, mask);

    FOG("Listen done");
    return 0;
}



static void *event_loop691(void *prm) {
    int r, i;
    struct timeval tv0, tv1, tv2, tv3;
    XEvent event;
    struct _event_thread691_ *info = (struct _event_thread691_*)prm;
    xgui691 *gui = info->gui;

    FOG("Thread launch");

    long emask;
    gettimeofday(&tv0, NULL);
    gettimeofday(&tv1, NULL);
    while(info->run) {
        pthread_mutex_lock(&event_thread691.mutex);
        while(XCheckMaskEvent(gui->display, mask, &event) && info->run) {
            repeat_test[2]++;
            if (event.xany.window != gui->window) continue;     // @@@

            emask = 1L << event.type;
            if (info->types_mask & emask == 0) continue;

            repeat_test[3]++;
            //
            //info->procs[event.type](info, &event);
            event_test_timing691(info, &event);

            //
            gettimeofday(&tv2, NULL);
            timersub(&tv2, &tv1, &tv3);
            if (tv3.tv_sec > 0) {
                LOG("Events loop : mo %d | kp %d | kr %d | ex %d | ge %d  |  in %ld.%06lu s / %d - %d"
                        , repeat_test[MotionNotify]
                        , repeat_test[KeyPress]
                        , repeat_test[KeyRelease]
                        , repeat_test[Expose]
                        , repeat_test[GraphicsExpose], tv3.tv_sec, tv3.tv_usec, repeat_test[2], repeat_test[3]
                );
                for(i = 0 ; i < LASTEvent ; i++) {
                    printf("%d|", repeat_test[i]);
                }
                printf("\n");
                clear_test691();
                tv1.tv_sec  = tv2.tv_sec;
                tv1.tv_usec = tv2.tv_usec;
            }
        }
        pthread_mutex_unlock(&event_thread691.mutex);
        usleep(1000);
    }
    FOG("Thread ended");
    return NULL;
}


/*
 *
 */
static void test() {
    int r;

    //
    Status st = XInitThreads();
    FOG("XInitThreads return %d",st);
    //
//    XrmInitialize();
//    FOG("XrmInitialize done");

    //
    xgui691 *gui = xgui_create691(864, 480, 1);
    if (!gui) return;
    gui->period = 57140;
    //
    r = xgui_open_window691(gui, "Test");
    if (r) {
        LOG("Can't open window, returning");
        return;
    }

    //
    int i;
    long l;
    struct timeval tv0, tv1, tv2, tv3, tv4;
    brodge650 *brodge = brodge_init(gui->width, gui->height, 2);
    bgra650   bgra;
    bgra_link650(&bgra, gui->ximg1->data, gui->width, gui->height);
    brodge_anim(brodge);
    brodge_exec(brodge, &bgra);
    //
    xgui_listen691(gui);

    //
    XGenericEvent evt;
    evt.type = GenericEvent;
    evt.serial = 0;
    evt.send_event = True;
    evt.display = gui->display;
    evt.extension = 234;
    evt.evtype = 0;
    //
    long frame = 0;
    i = 1;  // first in ximg1
    gettimeofday(&tv0, NULL);
    gettimeofday(&tv1, NULL);
    gettimeofday(&tv3, NULL);
    while(1) {
        //
        if (!gui->shm) {
            if (i % 2 == 0) {
                pthread_mutex_lock(&event_thread691.mutex);
                r = XPutImage(gui->display, gui->window, gui->gc, gui->ximg2
                        , 0, 0, 0, 0, gui->ximg2->width, gui->ximg2->height);
                pthread_mutex_unlock(&event_thread691.mutex);
                i = 1;
            } else {
                pthread_mutex_lock(&event_thread691.mutex);
                r = XPutImage(gui->display, gui->window, gui->gc, gui->ximg1
                        , 0, 0, 0, 0, gui->ximg1->width, gui->ximg1->height);
                pthread_mutex_unlock(&event_thread691.mutex);
                i = 0;
            }
        } else if (gui->shm) {
            if (i % 2 == 0) {
                r = XShmPutImage(gui->display, gui->window, gui->gc, gui->ximg2
                        , 0, 0, 0, 0, gui->ximg2->width, gui->ximg2->height, 1);
                r = XFlush(gui->display);
                i = 1;
            } else {
                r = XShmPutImage(gui->display, gui->window, gui->gc, gui->ximg1
                        , 0, 0, 0, 0, gui->ximg1->width, gui->ximg1->height, 1);
                r = XFlush(gui->display);
                i = 0;
            }
        }
        //
        if (i % 2 == 0) {
            bgra.data = (uint32_t*)gui->ximg2->data;
        } else {
            bgra.data = (uint32_t*)gui->ximg1->data;
        }
        //
        brodge_anim(brodge);
        brodge_exec(brodge, &bgra);
        frame++;
        //
        tv0.tv_usec += gui->period;
        while(tv0.tv_usec > 999999) {
            tv0.tv_sec++;
            tv0.tv_usec -= 1000000;
        }
        //
        gettimeofday(&tv2, NULL);
        timersub(&tv2, &tv0, &tv1);
        if (tv1.tv_usec < gui->period && !tv1.tv_sec) {     // @@@ manage ~=
            usleep(gui->period - tv1.tv_usec);
        }
        //
        if (frame % 30 == 0) {
            gettimeofday(&tv4, NULL);
            timersub(&tv4, &tv3, &tv4);
            LOG("Frame %ld for %.3f Hz", frame, 30. / (1. * tv4.tv_sec + 0.000001 * tv4.tv_usec));
            gettimeofday(&tv3, NULL);
        }
        //
    }
}


int main() {
    test();
}

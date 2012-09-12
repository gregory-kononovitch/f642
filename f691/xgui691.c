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

typedef struct _event_thread691_ ethread691;

typedef int (*event691)(struct _event_thread691_ *thread, XEvent *evt);

struct _event_thread691_ {
    //
    xgui691         *gui;
    int             run;
    long            types_mask;

    //
    events691       *events;
    void            *ext;

    //
    event691        procs[LASTEvent];

    //
    pthread_t       thread;
    pthread_mutex_t mutex;

    //
    int             repeat_test[LASTEvent];
    long            num_event0;
    long            num_event1;
    long            num_event2;
    long            num_event3;


    // KeyEvents
    char    escape;
    char    space;
    char    enter;
    char    tab;

    // Mouse
    int     mousex;
    int     mousey;

    // uitls
    int     i0;
    int     i1;
    int     i2;
    int     i3;
    int     i4;
};

static ethread691 event_thread691;

//
static void clear_test691(ethread691 *ethread) {
    memset(ethread->repeat_test, 0, sizeof(ethread->repeat_test));
}
static int event_test_timing691(ethread691 *ethread, XEvent *evt) {
    if (evt->type > (sizeof(ethread->repeat_test) >> 2)) return -1;
    ethread->repeat_test[evt->type]++;
    return 0;
}

static int motion_event691(ethread691 *ethread, XEvent *xevt) {
    event_test_timing691(ethread, xevt);
    if (ethread->events->notify_mouse_motion) {
        ethread->mousex = xevt->xmotion.x;
        ethread->mousey = xevt->xmotion.y;
        ethread->events->notify_mouse_motion(ethread->ext, xevt->xmotion.x, xevt->xmotion.y, xevt->xmotion.state, xevt->xmotion.time);
    }
    return 0;
}

static int button_pressed_event691(ethread691 *ethread, XEvent *xevt) {
    event_test_timing691(ethread, (XEvent*)xevt);

    if (xevt->xbutton.button > Button3) {
        if (ethread->events->notify_scroll) {
            ethread->events->notify_scroll(ethread->ext, xevt->xbutton.button, xevt->xbutton.x, xevt->xbutton.y, xevt->xbutton.state, xevt->xbutton.time);
            printf("SCROLL1: button %u ; state = %u ; x = %d ; y = %d ; t = %lu ms\n"
                    , xevt->xbutton.button
                    , xevt->xbutton.state
                    , xevt->xbutton.x
                    , xevt->xbutton.y
                    , xevt->xbutton.time
            );
            return 0;
        }
    } else {
        if (ethread->events->notify_button_pressed) {
            ethread->events->notify_button_pressed(ethread->ext, xevt->xbutton.button, xevt->xbutton.x, xevt->xbutton.y, xevt->xbutton.state, xevt->xbutton.time);
            return 0;
        }
    }
    return 1;
}

static int button_released_event691(ethread691 *ethread, XEvent *xevt) {
    event_test_timing691(ethread, (XEvent*)xevt);

    if (xevt->xbutton.button > Button3) {
        if (ethread->events->notify_scroll) {
            ethread->events->notify_scroll(ethread->ext, xevt->xbutton.button, xevt->xbutton.x, xevt->xbutton.y, xevt->xbutton.state, xevt->xbutton.time);
            printf("SCROLL2: button %u ; state = %u ; x = %d ; y = %d ; t = %lu ms\n"
                    , xevt->xbutton.button
                    , xevt->xbutton.state
                    , xevt->xbutton.x
                    , xevt->xbutton.y
                    , xevt->xbutton.time
            );
            return 0;
        }
    } else {
        if (ethread->events->notify_button_released) {
            ethread->events->notify_button_released(ethread->ext, xevt->xbutton.button, xevt->xbutton.x, xevt->xbutton.y, xevt->xbutton.state, xevt->xbutton.time);
            return 0;
        }
    }
    return 1;
}

static int key_pressed_event691(ethread691 *ethread, XEvent *xevt) {
    event_test_timing691(ethread, xevt);
    if (ethread->events->notify_key_pressed) {
        KeySym ky = XKeycodeToKeysym(ethread->gui->display, xevt->xkey.keycode, 0);
        switch(ky) {
            case XK_Escape:
                ethread->escape = 1;
                ethread->run = 0;
                break;
            case XK_space:
                ethread->space = 1;
                break;
            case XK_Return:
                ethread->enter = 1;
                break;
            case XK_Tab:
                ethread->tab = 1;
                break;
        }

        ethread->events->notify_key_pressed(ethread->ext, ky, xevt->xkey.x, xevt->xkey.y, xevt->xkey.state, xevt->xkey.time);
    }
    return 0;
}

static int key_released_event691(ethread691 *ethread, XEvent *xevt) {
    event_test_timing691(ethread, xevt);
    if (ethread->events->notify_key_released) {
        KeySym ky = XKeycodeToKeysym(ethread->gui->display, xevt->xkey.keycode, 0);
        ethread->events->notify_key_released(ethread->ext, ky, xevt->xkey.x, xevt->xkey.y, xevt->xkey.state, xevt->xkey.time);
    }
    return 0;
}


//
int xgui_listen691(xgui691 *gui, events691 *events) {

    //
    event_thread691.gui = gui;
    event_thread691.types_mask = 0xFFFFFFFFFFFFFFL;
    event_thread691.run = 0;
    event_thread691.i0 = -1;

    //
    if (events) {
        event_thread691.events = events;
    } else {
        event_thread691.events = calloc(1, sizeof(events691));
    }

    //
    event_thread691.num_event0 = 0;
    event_thread691.num_event1 = 0;
    event_thread691.num_event2 = 0;
    event_thread691.num_event3 = 0;
    int i;
    for(i = 0 ; i < LASTEvent ; i++) {
        event_thread691.procs[i] = event_test_timing691;
    }
    //
    event_thread691.procs[KeyPress]         = key_pressed_event691;
    event_thread691.procs[KeyRelease]       = key_released_event691;
    //
    event_thread691.procs[MotionNotify]     = motion_event691;
    event_thread691.procs[ButtonPress]      = button_pressed_event691;
    event_thread691.procs[ButtonRelease]    = button_released_event691;
    event_thread691.procs[KeyPress]         = key_pressed_event691;
    event_thread691.procs[KeyRelease]       = key_released_event691;

    //
    clear_test691(&event_thread691);

    //
    pthread_mutex_init(&event_thread691.mutex, NULL);


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
    ethread691 *ethread = (ethread691*)prm;
    xgui691 *gui = ethread->gui;

    FOG("Thread launch");

    long emask;
    gettimeofday(&tv0, NULL);
    gettimeofday(&tv1, NULL);
    while(ethread->run) {
        pthread_mutex_lock(&ethread->mutex);
        while(XCheckMaskEvent(gui->display, mask, &event) && ethread->run) {
            //
            ethread->num_event0++;
            if (event.xany.window != gui->window) continue;     // @@@
            ethread->num_event1++;
            //
            emask = 1L << event.type;
            if (ethread->types_mask & emask == 0) continue;
            ethread->num_event2++;

            //
            if (ethread->procs[event.type]) {
                ethread->procs[event.type](ethread, &event);
            }
        }
        pthread_mutex_unlock(&ethread->mutex);

        //
        gettimeofday(&tv2, NULL);
        timersub(&tv2, &tv1, &tv3);
        if (tv3.tv_sec > 0) {
            LOG("Events loop : mo %d | kp %d | kr %d  | bp %d | br %d | ex %d | ge %d  |  in %ld.%06lu s / %d - %d"
                    , ethread->repeat_test[MotionNotify]
                    , ethread->repeat_test[KeyPress]
                    , ethread->repeat_test[KeyRelease]
                    , ethread->repeat_test[ButtonPress]
                    , ethread->repeat_test[ButtonRelease]
                    , ethread->repeat_test[Expose]
                    , ethread->repeat_test[GraphicsExpose], tv3.tv_sec, tv3.tv_usec, ethread->repeat_test[50], ethread->repeat_test[51]
            );
            for(i = 0 ; i < LASTEvent ; i++) {
                printf("%d|", ethread->repeat_test[i]);
                if (i % 5 == 4) printf("| |");
            }
            printf("\n");
            clear_test691(ethread);
            tv1.tv_sec  = tv2.tv_sec;
            tv1.tv_usec = tv2.tv_usec;
        }

        usleep(1000);
    }
    FOG("Thread ended");
    return NULL;
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

    //
    xgui691 *gui = xgui_create691(800, 448, 1);
    if (!gui) return -1;
    gui->period = 57140;
    //
    r = xgui_open_window691(gui, "Test");
    if (r) {
        LOG("Can't open window, returning");
        return -1;
    }

    //
    int i;
    long l;
    struct timeval tv0, tv1, tv2, tv3, tv4;
    struct timeval tvb1, tvb2;
    long broad;
    brodge650 *brodge = brodge_init(gui->width, gui->height, 2);
    void *srcs = brodge->sources;
    int nb_srcs = brodge->nb_src;
    bgra650   bgra;
    bgra_link650(&bgra, gui->ximg1->data, gui->width, gui->height);
    brodge_anim(brodge);
    brodge_exec(brodge, &bgra);
    //
    xgui_listen691(gui, NULL);

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
    while(event_thread691.run) {
        //
        gettimeofday(&tvb1, NULL);
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
        gettimeofday(&tvb2, NULL);
        timersub(&tvb2, &tvb1, &tvb2);
        broad += tvb2.tv_usec;
        //
        if (i % 2 == 0) {
            bgra.data = (uint32_t*)gui->ximg2->data;
        } else {
            bgra.data = (uint32_t*)gui->ximg1->data;
        }
        //
        if (event_thread691.space) {
            brodge_rebase(brodge);
            event_thread691.space = 0;
        }
        if (event_thread691.tab) {
            event_thread691.i0++;
            if (event_thread691.i0 >= brodge->nb_src) {
                event_thread691.i0 = -1;
            }
            event_thread691.tab = 0;
        }
        if (event_thread691.enter) {
            if (event_thread691.i1) event_thread691.i1 = 0;
            else event_thread691.i1 = 1;
            event_thread691.enter = 0;
        }
        brodge_anim(brodge);
        if (event_thread691.i0 > -1) {
            brodge->sources[event_thread691.i0]->x = event_thread691.mousex;
            brodge->sources[event_thread691.i0]->y = event_thread691.mousey;
//            if (event_thread691.i1) {
//                brodge->sources = &srcs[event_thread691.i0];
//                brodge->nb_src = 1;
//            } else {
//                brodge->sources = srcs;
//                brodge->nb_src = nb_srcs;
//            }
        } else {
            brodge->sources = srcs;
            brodge->nb_src = nb_srcs;
        }
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
            LOG("Frame %ld for %.3f Hz for %ld µs", frame, 30. / (1. * tv4.tv_sec + 0.000001 * tv4.tv_usec), broad / 30);
            broad = 0;
            gettimeofday(&tv3, NULL);
        }
        //
    }
    //
    xgui_close_window691(gui);
    xgui_free691(&gui);
    return 0;
}


int main() {
    test();
}

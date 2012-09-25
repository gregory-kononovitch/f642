/*
 * file    : xgui691p.c
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

typedef struct _event_thread691_ ethread691;

typedef struct {
    // public
    int             width;
    int             height;
    int xoffset;            /* number of pixels offset in X direction */
    int byte_order;         /* data byte order, LSBFirst, MSBFirst */
    int depth;              /* depth of image */
    int bytes_per_line;     /* accelarator to next line */
    int bits_per_pixel;     /* bits per pixel (ZPixmap) */
    unsigned long red_mask; /* bits in z arrangment */
    unsigned long green_mask;
    unsigned long blue_mask;

    void    *pix1;
    void    *pix2;

    // private @@@
    // display
    Display         *display;
    XVisualInfo     vinfo;
    int             screen;
    GC              gc;

    // window
    Window          window;
    Colormap        color_map;
    XSizeHints      hint;
    XWindowAttributes xwa;

    //
    int             shm;
    XShmSegmentInfo shm_info;
    XShmSegmentInfo shm_info2;

    //
    XImage          *ximg1;
    XImage          *ximg2;     // @@@

    //
    ethread691      *event_thread;
} xgui691p;

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

    xgui691p *gui = calloc(1, sizeof(xgui691p));
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

    // Find visual info
    gui->vinfo.depth = gui->screen;
    gui->vinfo.depth = 24;
    gui->vinfo.red_mask = 0x00ff0000;
    gui->vinfo.green_mask = 0x0000ff00;
    gui->vinfo.blue_mask = 0x000000ff;
    gui->vinfo.bits_per_rgb = 8;
    gui->vinfo.class = TrueColor;
    gui->vinfo.colormap_size = 256;
    int nbvi = 0;
    XVisualInfo *vinfos = XGetVisualInfo(gui->display
            , VisualScreenMask | VisualClassMask | VisualDepthMask  | VisualBitsPerRGBMask
            | VisualRedMaskMask | VisualGreenMaskMask | VisualBlueMaskMask
            | VisualColormapSizeMask
            , &gui->vinfo, &nbvi
            );
    //
    if (nbvi > 0) {
        memcpy(&gui->vinfo, vinfos, sizeof(XVisualInfo));
        FOG("Found %d Match TrueColor depth 24 : %d bits per rgb and good mask (cmap %d)"
                , nbvi, gui->vinfo.bits_per_rgb, gui->vinfo.colormap_size);
    }
    if (XMatchVisualInfo(gui->display, gui->screen, 24, TrueColor, &gui->vinfo)) {
        FOG("Get Match TrueColor depth 24 : %d bits per rgb, cmap %d, mask red %08x ; green %08x ; blue %08x"
                , gui->vinfo.bits_per_rgb, gui->vinfo.colormap_size
                , gui->vinfo.red_mask, gui->vinfo.green_mask, gui->vinfo.blue_mask);
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
        gui->shm_info.shmid = shmget(IPC_PRIVATE, 4 * width * height, IPC_CREAT | 0777);
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
        // ###
        gui->shm_info2.shmid = shmget(IPC_PRIVATE, 4 * width * height, IPC_CREAT | 0777);
        FOG("shmget2 return %d - %p", gui->shm_info2.shmid, gui->shm_info2.shmaddr);
        //
        gui->shm_info2.shmaddr = (char*)shmat(gui->shm_info2.shmid, 0, 0);
        FOG("shmat2 return %p", gui->shm_info2.shmaddr);
        //
        gui->shm_info2.readOnly = False;
        st = XShmAttach(gui->display, &gui->shm_info2);
        FOG("XShmAttach return %d - %p", st, gui->shm_info2.shmaddr);
        if (st == False || xerror) {
            LOG("Failed to attach shm memory");
            shm = 0;
            gui->shm = 0;
            shmdt(gui->shm_info2.shmaddr);
            break;
        }
        r = XSync(gui->display, False);
        FOG("XSync return %d", r);
        //
        r = shmctl(gui->shm_info2.shmid, IPC_RMID, 0);
        FOG("shmctl2 return %d", r);

        //
        gui->ximg1 = XShmCreateImage(gui->display, gui->vinfo.visual
                , gui->vinfo.depth, ZPixmap, gui->shm_info.shmaddr
                , &gui->shm_info
                , gui->width, gui->height
        );
        FOG("XShmCreateImage 1 return img %p, pix %p,  err = %d", gui->ximg1, gui->ximg1->data, xerror);        memcpy(&gui->shm_info2, &gui->shm_info, sizeof(XShmSegmentInfo));
//        gui->ximg2 = XShmCreateImage(gui->display, gui->vinfo.visual
//                , gui->vinfo.depth, ZPixmap, gui->shm_info.shmaddr + (4 * width * height)
//                , &gui->shm_info2
//                , gui->width, gui->height
//        );
        gui->ximg2 = XShmCreateImage(gui->display, gui->vinfo.visual
                , gui->vinfo.depth, ZPixmap, gui->shm_info2.shmaddr
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
    gui->xoffset        = gui->ximg1->xoffset;
    gui->byte_order     = gui->ximg1->byte_order;
    gui->depth          = gui->ximg1->depth;
    gui->bytes_per_line = gui->ximg1->bytes_per_line;
    gui->bits_per_pixel = gui->ximg1->bits_per_pixel;
    gui->red_mask       = gui->ximg1->red_mask;
    gui->green_mask     = gui->ximg1->green_mask;
    gui->blue_mask      = gui->ximg1->blue_mask;

    gui->pix1           = gui->ximg1->data;
    gui->pix2           = gui->ximg2->data;

    //
    return (xgui691*)gui;
}


void xgui_free691(xgui691 **xgui) {
    xgui691p *gui = (xgui691p*)(*xgui);
    if (gui->window) xgui_close_window691(*xgui);
    //
    if (gui->shm) {
        XShmDetach(gui->display, &gui->shm_info);
        XSync(gui->display, True);
        //
        shmdt(gui->shm_info.shmaddr);
        free(gui->ximg1);
        free(gui->ximg2);
    } else {
        free(gui->ximg1->data);
        free(gui->ximg2->data);
        free(gui->ximg1);
        free(gui->ximg2);
    }
    //
    XCloseDisplay(gui->display);
    free(gui);
    *xgui = NULL;
    return;
}

int xgui_open_window691(xgui691 *xgui, const char *title) {
    xgui691p *gui = (xgui691p*)xgui;
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


int xgui_close_window691(xgui691 *xgui) {
    xgui691p *gui = (xgui691p*)xgui;

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

typedef int (*event691)(struct _event_thread691_ *thread, XEvent *evt);

struct _event_thread691_ {
    //
    xgui691p         *gui;
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
    pthread_attr_t  attr;

    // Monitoring
    struct timeval  tvm0, tvm1, tvm2, tvm3;
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

//
static void clear_test691(ethread691 *ethread) {
    memset(ethread->repeat_test, 0, sizeof(ethread->repeat_test));
}
static int event_test_timing691(ethread691 *ethread, XEvent *evt) {
    if (evt->type < LASTEvent) {
        ethread->repeat_test[evt->type]++;
    }
    return 0;
}
static int event_monitor_log(ethread691 *ethread) {
    int i;
    struct timeval tv;
    //
    if (ethread->num_event0) {
        gettimeofday(&ethread->tvm2, NULL);
        timersub(&ethread->tvm2, &ethread->tvm1, &tv);
        if (tv.tv_sec > 1) {
            LOG("Events loop : mo %d | kp %d | kr %d  | bp %d | br %d | ex %d | ge %d  |  in %ld.%06lu s / %d - %d - %d"
                    , ethread->repeat_test[MotionNotify]
                    , ethread->repeat_test[KeyPress]
                    , ethread->repeat_test[KeyRelease]
                    , ethread->repeat_test[ButtonPress]
                    , ethread->repeat_test[ButtonRelease]
                    , ethread->repeat_test[Expose]
                    , ethread->repeat_test[GraphicsExpose]
                    , tv.tv_sec, tv.tv_usec, ethread->num_event0, ethread->num_event1, ethread->num_event2
            );
            for(i = 0 ; i < LASTEvent ; i++) {
                printf("%d|", ethread->repeat_test[i]);
                if (i % 5 == 4) printf("| |");
            }
            printf("\n");
            clear_test691(ethread);
            ethread->num_event3 += ethread->num_event0;
            ethread->num_event0 = 0;
            ethread->num_event1 = 0;
            ethread->num_event2 = 0;
            ethread->tvm1.tv_sec  = ethread->tvm2.tv_sec;
            ethread->tvm1.tv_usec = ethread->tvm2.tv_usec;
        }
    }
    return 0;
}

static int event_monitor(ethread691 *ethread) {
    event_monitor_log(ethread);
    usleep(1000);
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

    if (xevt->xbutton.button == Button4 || xevt->xbutton.button == Button5) {
        if (ethread->events->notify_scroll) {
            ethread->events->notify_scroll(ethread->ext, xevt->xbutton.button, xevt->xbutton.x, xevt->xbutton.y, xevt->xbutton.state, xevt->xbutton.time);
            return 0;
        }
        LOG("SCROLL1: button %u ; state = %u ; x = %d ; y = %d ; t = %lu ms"
                , xevt->xbutton.button
                , xevt->xbutton.state
                , xevt->xbutton.x
                , xevt->xbutton.y
                , xevt->xbutton.time
        );
    } else  if (xevt->xbutton.button && xevt->xbutton.button < Button4) {
        if (ethread->events->notify_button_pressed) {
            ethread->events->notify_button_pressed(ethread->ext, xevt->xbutton.button, xevt->xbutton.x, xevt->xbutton.y, xevt->xbutton.state, xevt->xbutton.time);
            return 0;
        }
        LOG("PRESSED: button %u ; state = %u ; x = %d ; y = %d ; t = %lu ms"
                , xevt->xbutton.button
                , xevt->xbutton.state
                , xevt->xbutton.x
                , xevt->xbutton.y
                , xevt->xbutton.time
        );
    } else {
        FOG("Unknown button released");
        return -1;
    }
    return 1;
}

static int button_released_event691(ethread691 *ethread, XEvent *xevt) {
    event_test_timing691(ethread, (XEvent*)xevt);

    if (xevt->xbutton.button == Button4 || xevt->xbutton.button == Button5) {
        if (ethread->events->notify_scroll) {
            ethread->events->notify_scroll(ethread->ext, xevt->xbutton.button, xevt->xbutton.x, xevt->xbutton.y, xevt->xbutton.state, xevt->xbutton.time);
            return 0;
        }
        LOG("SCROLL2: button %u ; state = %u ; x = %d ; y = %d ; t = %lu ms"
                , xevt->xbutton.button
                , xevt->xbutton.state
                , xevt->xbutton.x
                , xevt->xbutton.y
                , xevt->xbutton.time
        );
    } else if (xevt->xbutton.button && xevt->xbutton.button < Button4) {
        if (ethread->events->notify_button_released) {
            ethread->events->notify_button_released(ethread->ext, xevt->xbutton.button, xevt->xbutton.x, xevt->xbutton.y, xevt->xbutton.state, xevt->xbutton.time);
            return 0;
        }
        LOG("RELEASE: button %u ; state = %u ; x = %d ; y = %d ; t = %lu ms"
                , xevt->xbutton.button
                , xevt->xbutton.state
                , xevt->xbutton.x
                , xevt->xbutton.y
                , xevt->xbutton.time
        );
    } else {
        FOG("Unknown button released");
        return -1;
    }
    return 1;
}

static int key_pressed_event691(ethread691 *ethread, XEvent *xevt) {
    event_test_timing691(ethread, xevt);
    if (ethread->events->notify_key_pressed) {
        KeySym ky = XKeycodeToKeysym(ethread->gui->display, xevt->xkey.keycode, 0);
        ethread->events->notify_key_pressed(ethread->ext, ky, xevt->xkey.x, xevt->xkey.y, xevt->xkey.state, xevt->xkey.time);
        //
        LOG("KEYPRES: keycode = %u ; keysym = %lu O->", xevt->xkey.keycode, ky);
    } else {
        KeySym ky = XKeycodeToKeysym(ethread->gui->display, xevt->xkey.keycode, 0);
        LOG("KEYPRES: keycode = %u ; keysym = %lu X->", xevt->xkey.keycode, ky);
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

static int enter_event691(ethread691 *ethread, XEvent *xevt) {
    event_test_timing691(ethread, xevt);
    if (ethread->events->notify_enter) {
        ethread->events->notify_enter(ethread->ext, 1);
    }
    return 0;
}
static int leave_event691(ethread691 *ethread, XEvent *xevt) {
    event_test_timing691(ethread, xevt);
    if (ethread->events->notify_enter) {
        ethread->events->notify_enter(ethread->ext, -1);
    }
    return 0;
}

// Focus
static int focusin_event691(ethread691 *ethread, XEvent *xevt) {
    event_test_timing691(ethread, xevt);
    if (ethread->events->notify_focus) {
        ethread->events->notify_focus(ethread->ext, +1);
    }
    return 0;
}
static int focusout_event691(ethread691 *ethread, XEvent *xevt) {
    event_test_timing691(ethread, xevt);
    if (ethread->events->notify_focus) {
        ethread->events->notify_focus(ethread->ext, -1);    //xevt->xfocus.mode
    }
    return 0;
}

//
static int expose_event691(ethread691 *ethread, XEvent *xevt) {
    event_test_timing691(ethread, xevt);
    if (ethread->events->notify_expose) {
        ethread->events->notify_expose(ethread->ext, xevt->xexpose.x, xevt->xexpose.y, xevt->xexpose.width, xevt->xexpose.height);
    }
    return 0;
}

//
static int window_event691(ethread691 *ethread, XEvent *xevt) {
    event_test_timing691(ethread, xevt);
    if (ethread->events->notify_window) {
        ethread->events->notify_window(ethread->ext, xevt->xexpose.x, xevt->xexpose.y, xevt->xexpose.width, xevt->xexpose.height);
    }
    return 0;
}

//
int xgui_listen691(xgui691 *xgui, events691 *events, void *ext) {
    //
    xgui691p *gui = (xgui691p*)xgui;
    if (gui->event_thread) return -1;
    gui->event_thread = calloc(1, sizeof(ethread691));

    //
    gui->event_thread->gui = gui;
    gui->event_thread->types_mask = 0xFFFFFFFFFFFFFFL;
    gui->event_thread->run = 0;
    gui->event_thread->i0 = -1;

    //
    if (events) {
        gui->event_thread->events = events;
        gui->event_thread->ext    = ext;
    } else {                        // @@@
        gui->event_thread->events = calloc(1, sizeof(events691));
        gui->event_thread->ext    = ext;
    }

    //
    gui->event_thread->num_event0 = 0;
    gui->event_thread->num_event1 = 0;
    gui->event_thread->num_event2 = 0;
    gui->event_thread->num_event3 = 0;
    int i;
    for(i = 0 ; i < LASTEvent ; i++) {
        gui->event_thread->procs[i] = event_test_timing691;
    }

    //
    gui->event_thread->procs[Expose]           = expose_event691;
    gui->event_thread->procs[ConfigureNotify]  = window_event691;

    //
    gui->event_thread->procs[EnterNotify]      = enter_event691;
    gui->event_thread->procs[LeaveNotify]      = leave_event691;

    //
    gui->event_thread->procs[FocusIn]          = focusin_event691;
    gui->event_thread->procs[FocusOut]         = focusout_event691;

    //
    gui->event_thread->procs[KeyPress]         = key_pressed_event691;
    gui->event_thread->procs[KeyRelease]       = key_released_event691;
    //
    gui->event_thread->procs[MotionNotify]     = motion_event691;
    gui->event_thread->procs[ButtonPress]      = button_pressed_event691;
    gui->event_thread->procs[ButtonRelease]    = button_released_event691;
    gui->event_thread->procs[KeyPress]         = key_pressed_event691;
    gui->event_thread->procs[KeyRelease]       = key_released_event691;

    //
    clear_test691(gui->event_thread);

    //
    gui->event_thread->run = 1;
    pthread_attr_setdetachstate(&gui->event_thread->attr, PTHREAD_CREATE_JOINABLE);
    pthread_mutex_init(&gui->event_thread->mutex, NULL);
    pthread_create(&gui->event_thread->thread, &gui->event_thread->attr, &event_loop691, gui);

    //
    XSelectInput(gui->display, gui->window, mask);

    FOG("Listen initialized");
    return 0;
}

int xgui_stop691(xgui691 *xgui) {
    xgui691p *gui = (xgui691p*)xgui;

    // join
    if (gui->event_thread) {
        void *res;
        gui->event_thread->run = 0;
        pthread_join(gui->event_thread->thread, &res);
        free(gui->event_thread);
        gui->event_thread = NULL;
        return 0;
    }
    return -1;
}


static void *event_loop691(void *prm) {
    int r, i;
    XEvent event;
    xgui691p *gui = (xgui691p*)prm;
    ethread691 *ethread = gui->event_thread;


    FOG("Thread launch");

    long emask;
    gettimeofday(&ethread->tvm0, NULL);
    gettimeofday(&ethread->tvm1, NULL);
    FOG("Listen started run = %d", ethread->run);
    while(ethread->run) {
        pthread_mutex_lock(&ethread->mutex);
        while(XCheckMaskEvent(gui->display, mask, &event) && ethread->run) {
            //
            ethread->num_event0++;
            if (event.xany.window != gui->window) continue;     // @@@
            ethread->num_event1++;
            //
            emask = 1L << event.type;
            if (ethread->types_mask & emask == 0) continue;     // @@@
            ethread->num_event2++;

            //
            if (ethread->procs[event.type]) {
                ethread->procs[event.type](ethread, &event);
            }
        }
        pthread_mutex_unlock(&ethread->mutex);
        //
        event_monitor(ethread);
    }
    FOG("Thread ended");
    return NULL;
}


/*
 *
 */
int xgui_show691(xgui691 *xgui, int i, int srcx, int srcy, int destx, int desty, int width, int height) {
    int r = 0;
    struct timeval tvb1, tvb2;
    xgui691p *gui = (xgui691p*)xgui;

    if (!gui) return -1;
    ethread691 *ethread = (ethread691*)gui->event_thread;

    if(ethread->run) {      // @@@ sync
        //
        gettimeofday(&tvb1, NULL);
        if (ethread->gui->shm) {
            if (i) {
                pthread_mutex_lock(&ethread->mutex);
                r = XShmPutImage(gui->display, gui->window, gui->gc
                        , gui->ximg2
                        , srcx, srcy, destx, desty, width, height, False);
                r = XFlush(ethread->gui->display);    // @@@ r
//                XSync(gui->display, True);
                pthread_mutex_unlock(&ethread->mutex);
            } else {
                pthread_mutex_lock(&ethread->mutex);
                r = XShmPutImage(gui->display, gui->window, gui->gc
                        , gui->ximg1
                        , srcx, srcy, destx, desty, width, height, False);
                r = XFlush(ethread->gui->display);
//                XSync(gui->display, True);
                pthread_mutex_unlock(&ethread->mutex);
            }
        } else {
            if (i) {
                pthread_mutex_lock(&ethread->mutex);
                r = XPutImage(gui->display, gui->window, gui->gc
                        , gui->ximg2
                        , srcx, srcy, destx, desty, width, height);
                pthread_mutex_unlock(&ethread->mutex);
            } else {
                pthread_mutex_lock(&ethread->mutex);
                r = XPutImage(gui->display, gui->window, gui->gc
                        , gui->ximg1
                        , srcx, srcy, destx, desty, width, height);
                pthread_mutex_unlock(&ethread->mutex);
            }
        }
        gettimeofday(&tvb2, NULL);
        timersub(&tvb2, &tvb1, &tvb2);
        r = tvb2.tv_usec;
    } else {
        r = -1;
    }
    return r;
}

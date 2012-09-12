/*
 * file    : test691.c
 * project : f640 (f642 / t508.f640)
 *
 * Created on: Sep 10, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "f691.h"

#include "../f650/f650.h"
#include "../f650/brodge/brodge650.h"
#include "../f650/layout/f650_layout.h"


typedef struct {
    int         width;
    int         height;

    // X
    Display     *display;
    int         screen;
    Window      window;
    GC          gc;

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


static void dum_ximage(XImage *img) {
    FOG("XImage : pad = %d unit = %d bpp = %d", img->bitmap_pad, img->bitmap_unit, img->bits_per_pixel);
    FOG("\t bperl = %d data = %p depth = %d", img->bytes_per_line, img->data, img->depth);
    FOG("\t format = %d height = %d obdata = %d", img->format, img->height, img->obdata);
    FOG("\t width = %d offset = %d", img->width, img->xoffset);
    FOG("\t f = %d", img->f);
}

int create_window(x11gui691 *gui, int width, int height, int shm) {
    int r;
    // X11 related variables
    XVisualInfo vinfo;
    XEvent event;
  #ifdef SH_MEM
    int shmem_flag;
    XShmSegmentInfo shminfo1, shminfo2;
    int CompletionType;
    int xwidth;
  #endif // SH_MEM
    //
    char *title = "Test Xlib";
    Colormap cmap;
    XColor xcolor;
    XSizeHints hint;
    XWindowAttributes xwa;

    //
    gui->width  = width;
    gui->height = height;
    gui->shm    = shm;

    //
    gui->ximg = 0;


#ifdef SH_MEM
    CompletionType = -1;
#endif

    if (!(gui->display = XOpenDisplay(0))) {
        FOG("Can not open display, returning");
        return -1;
    }
    //
    //XSetErrorHandler(&handle_xerror);

    //
    if (shm && XShmQueryExtension(gui->display)) {
        FOG("XShmQueryExtension : ok");
        gui->shm = 1;
    } else {
        FOG("XShmQueryExtension : ko");
        shm = 0;
        gui->shm = 0;
    }

    // TRACE
    XSynchronize(gui->display, True);

    gui->screen = DefaultScreen(gui->display);

    // find best gui->display
//    if (XMatchVisualInfo(gui->display, gui->screen, 32, TrueColor, &vinfo)) {
//        FOG("Math TrueColor 32 : %d bits per rgb", vinfo.bits_per_rgb);
//    } else
    if (XMatchVisualInfo(gui->display, gui->screen, 24, TrueColor, &vinfo)) {
        FOG("Math TrueColor 24 : %d bits per rgb", vinfo.bits_per_rgb);
    } else if (XMatchVisualInfo(gui->display, gui->screen, 16, TrueColor, &vinfo)) {
    } else if (XMatchVisualInfo(gui->display, gui->screen, 8, PseudoColor, &vinfo)) {
    } else if (XMatchVisualInfo(gui->display, gui->screen, 8, GrayScale, &vinfo)) {
    } else if (XMatchVisualInfo(gui->display, gui->screen, 8, StaticGray, &vinfo)) {
    } else if (XMatchVisualInfo(gui->display, gui->screen, 1, StaticGray, &vinfo)) {
    } else error("requires 16 bit gui->display\n");


    // Make the gui->window
    hint.x = 200;
    hint.y = 200;
    hint.width = width;
    hint.height = height;
    hint.flags = PPosition | PSize;

    if (vinfo.class == TrueColor && vinfo.depth == 24) {
        FOG("XCreateWindow");
        cmap = XCreateColormap(gui->display, DefaultRootWindow(gui->display), vinfo.visual, AllocNone);
        FOG("CMap created");
        XSetWindowAttributes xswa;
        xswa.colormap = cmap;
        xswa.event_mask = StructureNotifyMask;
        xswa.border_pixel = BlackPixel(gui->display, gui->screen);

        gui->window = XCreateWindow(gui->display, DefaultRootWindow(gui->display)
                , hint.x, hint.y, hint.width, hint.height, 2
                , vinfo.depth, InputOutput
                , vinfo.visual
                , CWBorderPixel | CWColormap | CWEventMask, &xswa);
    } else {
        FOG("XCreateSimpleWindow");

        gui->window = XCreateSimpleWindow(gui->display
                , DefaultRootWindow(gui->display)
                , hint.x, hint.y, hint.width, hint.height, 4
                , WhitePixel(gui->display, gui->screen), BlackPixel(gui->display, gui->screen));
    }
    FOG("Window created");

    // Tell other applications about this gui->window
    XSetStandardProperties(gui->display, gui->window, title, title, None, NULL, 0, &hint);
    FOG("Standrd properties set");

    XSelectInput(gui->display, gui->window, StructureNotifyMask);
    FOG("Select StructureNotifyMask input");

    // Map gui->window
    XMapWindow(gui->display, gui->window);
    FOG("Map window wait");

    // Wait for map.
    do {
        XNextEvent(gui->display, &event);
    } while (event.type != MapNotify || event.xmap.event != gui->window);
    FOG("Map window done");

    XSelectInput(gui->display, gui->window, NoEventMask);
    FOG("Select NoEventMask input");

    gui->gc = DefaultGC(gui->display, gui->screen);
    FOG("Get default GC");

    //
    if (0) {
        gui->ximg = XCreateImage(gui->display
                , vinfo.visual
                , vinfo.depth, ZPixmap, 0
                , (char*)gui->bgra.data, gui->width, gui->height
                , vinfo.depth, vinfo.depth / vinfo.bits_per_rgb * gui->width
        );
        FOG("XCreateImage return %p", gui->ximg);
    } else if (0) {
        gui->ximg->width = width;
        gui->ximg->height = height;
        gui->ximg->format = ZPixmap;
        gui->ximg->data = (char*)gui->bgra.data;
//        gui->ximg->obdata = (void *)shm;
        gui->ximg->byte_order = LSBFirst;
        gui->ximg->bitmap_unit = 32;
        gui->ximg->bitmap_bit_order = LSBFirst;
        gui->ximg->bitmap_pad = 32;
        gui->ximg->depth = vinfo.depth;
        gui->ximg->bytes_per_line = 4 * width;
        gui->ximg->bits_per_pixel = 32;
        gui->ximg->red_mask = 0xff << 16;
        gui->ximg->green_mask = 0xff << 8;
        gui->ximg->blue_mask = 0xff << 0;
        gui->ximg->xoffset = 0;
    } else if (0) {
        Pixmap tmp;
        char *real, *ref;
        uint32_t mask;
        int i, j;
        XGCValues gcv;
        GC gc;

        gcv.graphics_exposures = 0;

        tmp = XCreatePixmap(gui->display, gui->window
                , gui->width, gui->height, vinfo.depth);
        gc = XCreateGC(gui->display, tmp, GCGraphicsExposures, &gcv);
        XCopyArea(gui->display, gui->window, tmp, gc
                , 0, 0, gui->width, gui->height, 0, 0);
        XShmGetImage(gui->display, tmp, gui->ximg, 0, 0, AllPlanes);
        XFreeGC(gui->display, gc);
        XFreePixmap(gui->display, tmp);
    } else if (gui->shm) {
        gui->ximg = XShmCreateImage(gui->display, vinfo.visual
                , vinfo.depth, ZPixmap, NULL
                , &gui->shm_info
                , gui->width, gui->height
                );
        FOG("XShmCreateImage return %p", gui->ximg);
        if (gui->ximg) {
            gui->shm_info.shmid = shmget(IPC_PRIVATE
                    , gui->ximg->bytes_per_line * gui->ximg->height
                    , IPC_CREAT | 0777
            );
            FOG("shmget return %d - %p", gui->shm_info.shmid, gui->shm_info.shmaddr);
            //
            gui->shm_info.shmaddr = (char*)shmat(gui->shm_info.shmid, 0, 0);
            FOG("shmat return %p", gui->shm_info.shmaddr);
            gui->shm_info.readOnly = False;
            gui->ximg->data = gui->shm_info.shmaddr;
            bgra_link650(&gui->bgra, gui->ximg->data, gui->width, gui->height);
            //
            Status st = XShmAttach(gui->display, &gui->shm_info);
            FOG("XShmAttach return %d - %p", st, gui->shm_info.shmaddr);
            r = XSync(gui->display, False);
            FOG("XSync return %d", r);
            //
            r = shmctl(gui->shm_info.shmid, IPC_RMID, 0);
            FOG("shmctl return %d", r);
        } else {
            FOG("XShmCreateImage failed");
        }
    } else {
        gui->shm  = 0;
//        gui->ximg = XGetImage(gui->display, gui->window
//                , 0, 0, gui->width, gui->height
//                , AllPlanes, ZPixmap);
//        FOG("XGetImage return %p", gui->ximg);
        bgra_alloc650(&gui->bgra, gui->width, gui->height);
        gui->ximg = XCreateImage(gui->display, vinfo.visual
                , vinfo.depth, ZPixmap, 0
                , (char*)gui->bgra.data
                , gui->width, gui->height
                , vinfo.visual->bits_per_rgb
                , 4 * gui->width
                );
        FOG("XCreateImage return %p", gui->ximg);
        //
//        if (gui->ximg) {
//            Status st = XInitImage(gui->ximg);
//            FOG("XInitImage return %d", st);
//        }
    }

    r = XSync(gui->display, False);
    FOG("XSync return %d", r);

    dum_ximage(gui->ximg);

    bgra_fill2650(&gui->bgra, 0xff507010);
    FOG("bgra fill ok");

    //
    if (gui->ximg && !gui->shm) {
        r = XPutImage(gui->display, gui->window, gui->gc, gui->ximg
                , 0, 0, 0, 0, gui->ximg->width, gui->ximg->height);
        FOG("XPutImage image return %d", r);
    } else if (gui->ximg && gui->shm) {
        r = XShmPutImage(gui->display, gui->window, gui->gc, gui->ximg
                , 0, 0, 0, 0, gui->ximg->width, gui->ximg->height, True);
        FOG("XShmPutImage return %d", r);
        //
        r = XFlush(gui->display);
        FOG("XFlush return %d", r);
    }

    return 0;
}



int resize(x11gui691 *gui) {
  FOG("int DisplayX11::resize()");
  Window root;
  int x, y;
  unsigned int w, h, b, d;
  XGetGeometry(gui->display, gui->window, &root, &x, &y, &w, &h, &b, &d);

  // to avoid new events for the time being
  XSelectInput(gui->display, gui->window, NoEventMask);

  // Create new image with new geometry

  XSelectInput(gui->display, gui->window, StructureNotifyMask);
  return 1;
}


int show_properties() {
    int i, r;

    // Display
    Display *display = XOpenDisplay(":0");
    FOG("Open display : %p", display);

    // Fonts
    int nbf;
    char **fonts = XListFonts(display, "sp", 50, &nbf);
    FOG("List %d fonts like %s :", nbf, "sp");

    // Extensions
    int nbe;
    char **extension = XListExtensions(display, &nbe);
    FOG("List %d extensions :", nbe);
//    for(i = 0 ; i < nbe ; i++) {
//        FOG("\t%s", extension[i]);
//    }

    // Hosts
    int nbh;
    Bool sth;
    XHostAddress *add = XListHosts(display, &nbh, &sth);
    FOG("List %d host address return %d", nbh, sth);
    for (i = 0; i < nbh; i++) {
        FOG("\t%s", add[i].address);
    }

    // Window
    Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 112, 76, 800, 448, 2, 0xffff00ff,
            0xff00ff00);
    FOG("Create window return %lu", window);

    // Window properties
    int nba;
    Atom *atoms = XListProperties(display, window, &nba);
    FOG("List %d properties", nba);
    for (i = 0; i < nba; i++) {
        FOG("\t%s", atoms[i]);
    }

    //
    r = XCloseDisplay(display);
    FOG("Close display return %d", r);

    return r;
}


int main0() {
    int r;
    x11gui691 *gui = calloc(1, sizeof(x11gui691));

    r = create_window(gui, 800, 448, 1);
    FOG("Create window return %d", r);

    //
    XEvent evt0;
    XExposeEvent evt;
    XGraphicsExposeEvent egt;
//    r = XWindowEvent(gui->display, gui->window, 0, &evt);
//    LOG("XWindowEvent return %d", r);

    //
    long l = 0;
    struct timeval tv1, tv2;
    brodge650 *brodge = brodge_init(gui->width, gui->height, 2);
    gettimeofday(&tv2, NULL);
    while(1) {
        brodge_anim(brodge);
        brodge_exec(brodge, &gui->bgra);
        //
        gettimeofday(&tv1, NULL);
        //
        if (gui->ximg && !gui->shm) {
            r = XPutImage(gui->display, gui->window, gui->gc, gui->ximg
                    , 0, 0, 0, 0, gui->ximg->width, gui->ximg->height);
        } else if (gui->ximg && gui->shm) {
            r = XShmPutImage(gui->display, gui->window, gui->gc, gui->ximg
                    , 0, 0, 0, 0, gui->ximg->width, gui->ximg->height, 1);
            //
            r = XFlush(gui->display);
        }
        //
        XSync(gui->display, 0);

        usleep(10000);
    }


    usleep(5000000);

}

/*
 * file    : test691.c
 * project : f640
 *
 * Created on: Sep 10, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "f691.h"



typedef struct {
    int         width;
    int         height;

    // X
    Display     *display;
    int         screen;
    Window      window;
    GC          gc;

    //
    XImage      *ximg;
    bgra650     bgra;


    //
} x11gui691;


int create_window(x11gui691 *gui, int width, int height) {
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

    //
    gui->ximg = 0;


#ifdef SH_MEM
    CompletionType = -1;
#endif

    if (!(gui->display = XOpenDisplay(0))) {
        FOG("Can not open display, returning");
        return -1;
    }

#ifdef TRACE
    XSynchronize(gui->display, 1);
#endif

    gui->screen = DefaultScreen(gui->display);

    // find best gui->display
    if (XMatchVisualInfo(gui->display, gui->screen, 32, TrueColor, &vinfo)) {
        FOG("Math TrueColor 32 : %d bits per rgb", vinfo.bits_per_rgb);
    } else if (XMatchVisualInfo(gui->display, gui->screen, 24, TrueColor, &vinfo)) {
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
    hint.flags = PPosition | PSize; // | NotUseful;

    if (vinfo.class == TrueColor && vinfo.depth == 24) {
        FOG("XCreateWindow");
        cmap = XCreateColormap(gui->display, DefaultRootWindow(gui->display), vinfo.visual, AllocNone);

        XSetWindowAttributes xswa;
        xswa.colormap = cmap;
        xswa.event_mask = StructureNotifyMask;
        xswa.border_pixel = BlackPixel(gui->display, gui->screen);

        gui->window = XCreateWindow(gui->display, DefaultRootWindow(gui->display)
                , hint.x, hint.y, hint.width, hint.height, 2
                , vinfo.depth, vinfo.class //InputOutput
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
    FOG("Map window start");

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
//    unsigned int    /* depth */,
//    int         /* format */,
//    int         /* offset */,
//    char*       /* data */,
//    unsigned int    /* width */,
//    unsigned int    /* height */,
//    int         /* bitmap_pad */,
//    int         /* bytes_per_line */

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
    } else {
        gui->ximg = XGetImage(gui->display, gui->window
                , 0, 0, gui->width, gui->height
                , AllPlanes, ZPixmap);
    }

    //
    if (gui->ximg) {
        Status st = XInitImage(gui->ximg);
        FOG("XInitImage return %d", st);
    }

    //
    bgra_link650(&gui->bgra, gui->ximg->data, gui->width, gui->height);
    bgra_fill2650(&gui->bgra, 0xff507010);

    //
    if (gui->ximg) {
        r = XPutImage(gui->display, gui->window, gui->gc, gui->ximg
                , 0, 0, 0, 0, gui->ximg->width, gui->ximg->height);
        FOG("XPutImage image return %d", r);
    }

    r = XFlush(gui->display);
    FOG("XFlush return %d", r);

    return 0;
}

void show_image(x11gui691 *gui) {
  FOG("void DisplayX11::gui->display_image(ximage, unsigned char *dithered_image)");

#ifdef SH_MEM                            // gui->display dithered image
  if (shmem_flag==2) return;
#endif


  // check for geometry changes
//  if (XCheckWindowEvent(gui->display, gui->window, StructureNotifyMask, &event))  resize();

#ifdef SH_MEM
  if (shmem_flag==1){
    XShmPutImage(gui->display, gui->window, gui->gc, ximg, 0,0,0,0,ximg->width,ximg->height,True);
    XFlush(gui->display);
  }
  else
#endif // SH_MEM
  {
    gui->ximg->data=(char*) gui->bgra.data;
    XPutImage(gui->display, gui->window, gui->gc, gui->ximg, 0, 0, 0, 0, gui->ximg->width, gui->ximg->height);
  }
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


int main() {
    int r;
    x11gui691 *gui = calloc(1, sizeof(x11gui691));

    r = create_window(gui, 640, 360);
    FOG("Create window return %d", r);

    //
    brodge650 *brodge = brodge_init(gui->width, gui->height, 2);
    while(1) {
        brodge_anim(brodge);
        brodge_exec(brodge, &gui->bgra);
        //
        r = XPutImage(gui->display, gui->window, gui->gc, gui->ximg
                , 0, 0, 0, 0, gui->ximg->width, gui->ximg->height);

        usleep(30);
    }


    usleep(5000000);

}

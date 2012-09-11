/*
 * file    : f691.h
 * project : f640 (f642 / t508.f640)
 *
 * Created on: Sep 10, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#ifndef F691_H_
#define F691_H_


#include <X11/Xutil.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>


#include "../f650/f650.h"
#include "../f650/brodge/brodge650.h"
#include "../f650/layout/f650_layout.h"


typedef struct {
    int             width;
    int             height;

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
    XImage          *ximg2;

} xgui691;



/*
 *
 */
xgui691 *xgui_create691(int width, int height, int shm);
void xgui_free691(xgui691 **gui);

int xgui_open_window691(xgui691 *gui, const char *title);
int xgui_close_window691(xgui691 *gui);

//
int xgui_listen691(xgui691 *gui);


#endif /* F691_H_ */

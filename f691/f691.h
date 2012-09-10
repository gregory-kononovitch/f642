/*
 * file    : f691.h
 * project : f640
 *
 * Created on: Sep 10, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#ifndef F691_H_
#define F691_H_


#include <X11/Xlib-xcb.h>
#include <X11/Xutil.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>


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
    XShmSegmentInfo shm_info;

    //
    XImage      *ximg;

} x11gui691;



#endif /* F691_H_ */

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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>

#include <X11/XKBlib.h>
#include <X11/Xutil.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>



//
#define FOG(s,...) fprintf(stderr, "%s: " s "\n", __func__, ##__VA_ARGS__);
#define LOG(s,...) fprintf(stderr, s "\n", ##__VA_ARGS__);


/*
 *
 */
typedef struct {
    // Key
    int     (*notify_key_pressed)(void *ext, unsigned long key, int x, int y, int mask, uint64_t time);
    int     (*notify_key_released)(void *ext, unsigned long key, int x, int y, int mask, uint64_t time);

    // Mouse
    int     (*notify_mouse_motion)(void *ext, int x, int y, int mask, uint64_t time);
    int     (*notify_button_pressed)(void *ext, int button, int x, int y, int mask, uint64_t time);
    int     (*notify_button_released)(void *ext, int button, int x, int y, int mask, uint64_t time);
    int     (*notify_scroll)(void *ext, int button, int x, int y, int mask, uint64_t time);

    // Drag
    int     (*notify_drag)(void *ext, int button, int x, int y, int mask, uint64_t time);

    // Enter/Leave
    int     (*notify_enter)(void *ext, int inout, int x, int y, uint64_t time);

    // Focus
    int     (*notify_focus)(void *ext, int inout);

    // Window
    int     (*notify_window)(void *ext, int x, int y, int width, int height);
    int     (*notify_visibility)(void *ext, int state, uint64_t time);

    // Expose
    int     (*notify_expose)(void *ext, int x, int y, int width, int height);


} events691;


typedef struct {
    int width, height;      /* size of image */
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
} xgui691;

/*
 *
 */
xgui691 *xgui_create691(int width, int height, int shm);
void xgui_free691(xgui691 **gui);

int xgui_open_window691(xgui691 *gui, const char *title);
int xgui_close_window691(xgui691 *gui);

//
int xgui_listen691(xgui691 *gui, events691 *events, void* ext);

//
int xgui_show691(xgui691 *gui, int i, int srcx, int srcy, int destx, int desty, int width, int height);


#endif /* F691_H_ */

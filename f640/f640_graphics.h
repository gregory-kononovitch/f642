/*
 * file    : f640_graphics.h
 * project : f640
 *
 * Created on: Dec 3, 2011
 * Author and copyright (C) 2011 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#ifndef F640_GRAPHICS_H_
#define F640_GRAPHICS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

//
struct f640_image;

//
#define F640_LEFT_TO_RIGHT  1
#define F640_RIGHT_TO_LEFT  -1
#define F640_TOP_TO_BOTTOM  1
#define F640_BOTTOM_TO_TOP  -1

struct f640_graphics_operations {
    int (*f640_draw_hline)(struct f640_image *image, int index, int w, int dir);
    int (*f640_draw_vline)(struct f640_image *image, int index, int h, int dir);
};

//
struct f640_glyphs {
    int width;
    int height;
};

// ----------------
#define F640_IMAGE()                            \
    int         width;                          \
    int         height;                         \
    int         size;                           \
                                                \
    uint8_t     *data;                          \
    int         data_size;                      \
                                                \
    struct f640_graphics_operations gops;       \
                                                \
    struct f640_glyphs glyphs;                  \
                                                \
    uint8_t     *ptr;                           \
                                                \
                                                \
// ----------------
struct f640_graphics_operations;
struct f640_glyphs;
struct f640_image {
    F640_IMAGE()
};


/*
 *
 */
#define f640_get_index(x,y,width) (x + width * y)

extern int f640_draw_rect(struct f640_image *image, int index, int distx, int disty);

extern int f640_draw_digit(struct f640_image *image, int x, int y, int value);
extern int f640_draw_number(struct f640_image *image, int x, int y, uint64_t value);


/*
 *
 */
extern struct f640_image *f640_create_gry_image(int width, int height);
extern struct f640_image *f640_create_yuv_image(int width, int height);
extern struct f640_image *f640_create_rgb_image(int width, int height);
extern struct f640_image *f640_create_rgba_image(int width, int height);

/*
 *
 */
extern void f640_yuv_to_rgb(struct f640_image *yuv, int index, struct f640_image *rgb);
extern void f640_full_yuv_to_rgb(struct f640_image *yuv, struct f640_image *rgb);

#endif /* F640_GRAPHICS_H_ */

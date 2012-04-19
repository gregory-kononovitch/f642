/*
 * file    : f641_v4L2.h
 * project : f640
 *
 * Created on: Apr 19, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#ifndef F641_V4L2_H_
#define F641_V4L2_H_

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>

#include <linux/videodev2.h>

#define F640_RESET       "\033[0m"//(0)
#define F640_UNDER       "\033[4m"//(0)
#define F640_ERASE_LINE  "\033[2J"
#define F640_BOLD        "\033[1m"//(1)
#define F640_FG_BLACK    "\033[30m"//(30)
#define F640_FG_RED      "\033[31m"//(31)
#define F640_FG_GREEN    "\033[32m"//(32)
#define F640_FG_BROWN    "\033[33m"//(33)
#define F640_FG_BLUE     "\033[34m"//(34)
#define F640_FG_PURPLE   "\033[35m"//(35)
#define F640_FG_CYAN     "\033[36m"//(36)
#define F640_FG_GREY     "\033[37m"//(37)
#define SCALE(df, dr, sf, sr, sv) (((sv - sf) * (dr - df) / (sr - sf)) + df)

typedef struct {
    void *start;
    size_t length;
} v4l2_buffer_t;

struct f641_v4l2_parameters {
    int DEBUG;

    char device[32];
    char dev[32];
    int fd;
    struct v4l2_capability cap;
    char source[32];
    char input[32];
    int width;
    int height;
    int frames_pers;
    uint32_t palette;// = 0x47504A4D;   // 0x56595559
    struct v4l2_format format;
    struct v4l2_fmtdesc format_desc;

    enum v4l2_memory memory;
    struct v4l2_requestbuffers req;
    struct v4l2_buffer buf;

    v4l2_buffer_t *buffers;

    int capture;
    int verbose;
    int quiet;
    int debug;
    int show_all;
    int show_caps;
    int show_inputs;
    int show_controls;
    int show_formats;
    int show_framsize;
    int show_framints;
    int show_vidstds;

    int nb_controls;
    struct v4l2_queryctrl controls[128];
    int32_t controls_value[128];
};


#endif /* F641_V4L2_H_ */

/*
 * file    : f641.h
 * project : f640
 *
 * Created on: Dec 20, 2011
 * Author and copyright (C) 2011 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#ifndef F641_H_
#define F641_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include <sys/ioctl.h>
#include <linux/videodev2.h>

#include "../f640.h"
#include "../f640_graphics.h"
#include "../f640_queues.h"


struct f641_raw_data {
    void*   mem;
    size_t  size;
};

struct f641_process_data {

    //
    int proc_len;
    struct f640_stone *stones;

    //
    struct timeval tv0;     // start

    // Decoding
    int decoded_format;                 // YUV : PIX_FMT_YUYV422, MJPEG : PIX_FMT_YUVJ422P, MPEG : PIX_FMT_YUV420P
    int broadcast_format;               // PIX_FMT_BGR24, PIX_FMT_GRAY8

    // Grid
    struct f640_grid        *grid;
    struct f640_grid2       *grid2;

    // Recording
    enum CodecID recording_codec;       // CODEC_ID_MJPEG, CODEC_ID_RAWVIDEO, CODEC_ID_MPEG1VIDEO, CODEC_ID_MPEG4
    int record_format;                  // MJPEG -> PIX_FMT_YUVJ422P, CODEC_ID_MPEG1VIDEO -> PIX_FMT_YUV420P
    long recorded_frames;
    struct timeval tvr;

};


struct f641_appli {

    // Dimension
    int width;
    int height;
    int size;

    // Parameters
    int frames_pers;
    long max_frame;

    // Functions
    long functions;

    // V4L2
    char device[256];
    int fd;

    // Tagging
    int threshold;

    // Recording
    int  recording;
    char record_path[256];

    // Processing
    struct f641_process_data *process;

    // Broadcasting
    char stream_path[256];
    int fd_stream;
    char grid_path[256];
    int fd_grid;
    char edge_path[256];
    int fd_edge;
    char get_path[256];
    int fd_get;
    char grid10_path[256];
    int fd_grid10;
    char grid20_path[256];
    int fd_grid20;
    char grid30_path[256];
    int fd_grid30;
};


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
};

/************************************
 *      VIDEO FUNCTIONS
 ************************************/
// V4L2
extern void f641_attrib_v4l2_snaping(struct f641_thread_operations *ops);
extern void f641_attrib_v4l2_desnaping(struct f641_thread_operations *ops);

// FFMpeg
extern void f641_attrib_recording(struct f641_thread_operations *ops);
extern void f641_attrib_edging(struct f641_thread_operations *ops);
extern void f641_attrib_converting_torgb(struct f641_thread_operations *ops);
extern void f641_attrib_decoding_mjpeg(struct f641_thread_operations *ops);

// Processing
extern void f641_attrib_logging(struct f641_thread_operations *ops);
extern void f641_attrib_griding(struct f641_thread_operations *ops);
extern void f641_attrib_broadcasting(struct f641_thread_operations *ops);
extern void f641_attrib_tagging(struct f641_thread_operations *ops);
extern void f641_attrib_watching_422p(struct f641_thread_operations *ops);



#endif /* F641_H_ */

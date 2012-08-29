/*
 * file    : f643.h
 * project : f640
 *
 * Created on: Aug 24, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#ifndef F643_H_
#define F643_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <lame/lame.h>
#include <x264.h>
#include <mp4v2/mp4v2.h>
#include <libavutil/pixdesc.h>
#include <libswscale/swscale.h>


/*
 * From flv, concerning x264 payloads :
 * 1) First frame contains a ~header :
 *      - The nal[2].p_payload from x264_encoder_parameters (+ x264_encoder_headers)
 *      - The frame nal[0].p_payload
 * 2) Frames nal[0].p_payload
 *
 * => The 0th nal[2].p_payload contains X264 parameters {(name, value)}
 * ? sps & pps : before [2], seems codec tags
 */


#define FOG(s,...) fprintf(stderr, "%s: " s "\n", __func__, ##__VA_ARGS__);


//
struct f643 {
    // Video
    int             width;
    int             height;
    float           fps;
    enum PixelFormat pix_in;

    // MP4
    MP4FileHandle   handle;
    uint32_t        timescale;
    MP4TrackId      x264_track;

    // MP3

    // X264
    x264_t          *x264;
    x264_param_t    param;
    int             preset;
    int             tune;
    x264_picture_t  rgb;
    x264_picture_t  yuv;
    int64_t         frame;
    int64_t         pts0;
    int64_t         pts;
    int64_t         dts0;
    int64_t         dts;
    uint8_t         *sei;
    int             lsei;

    // SwScale
    struct SwsContext *swsCtxt;

};

/* 1) alloc : malloc
 * 2) init_xxx : setXxx
 * 3) open_file : MP4FileHandle
 * 4) setup_x264_codec : x264_t + 264_param_t + track_id
 * 5) set other x264 params
 * 6) f643_open_x264_track
 *
 *
 *
 *
 * +) close_x264_codec
 * +) close_file
 */

//
struct f643 *f643_alloc(int width, int height, float fps);
void f643_free(struct f643 **f643);


// MP4
void f643_init_mp4(struct f643 *f643, uint32_t timescale);
int f643_open_file(const char *path, struct f643 *f643);
int f643_open_x264_track(struct f643 *f643);
int f643_close_file(struct f643 *f643);

int f643_add_x264_frame(struct f643 *f643);


// X264
void f643_init_x264(struct f643 *f643, int preset, int tune);
int f643_setup_x264_codec(struct f643 *f643);
int f643_close_x264_codec(struct f643 *f643);

#endif /* F643_H_ */

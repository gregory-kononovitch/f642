/*
 * file    : f642_x264.h
 * project : f640
 *
 * Created on: Sep 25, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#ifndef F642_X264_H_
#define F642_X264_H_

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

#include <x264.h>
#include "muxers/output.h"


// Muxers
#define RAW_MUXER = 0
#define MKV_MUXER = 1
#define FLV_MUXER = 2

typedef struct {
    // Video
    int width;
    int height;
    float fps;

    // SwScale
    struct SwsContext *swsCtxt;
    x264_picture_t rgb;
    x264_picture_t yuv;

    // Encoding
    x264_param_t param;
    x264_t *x264;

    // File
    const cli_output_t *output;
    hnd_t outh;

    //
    long frame;
    int64_t pts, pts0;
    int64_t dts, dts0;

    //
    int loglevel;

} f642x264;

typedef struct {
    x264_picture_t  **yuv;
    int             nb_yuv;
    //
    int             num;
    pthread_mutex_t mutex;
    pthread_cond_t  cond;

} queue642;


extern f642x264 *init642_x264(int width, int height, float fps, int preset, int tune);
extern int f642_open(f642x264 *x264, const char *path, int muxer);
extern int f642_close(f642x264 *x264);
extern int f642_setQP(f642x264 *x264, int qp);
extern int f642_setQP(f642x264 *x264, int qpmin, int qpmax, int qpstep);
extern int f642_setParam(f642x264 *x264, const char *name, const char *value);
extern int f642_setNbThreads(f642x264 *x264, int nb);
extern int f642_addFrame(f642x264 *x264, x264_picture_t *yuv, struct timeval tv);

#endif /* F642_X264_H_ */

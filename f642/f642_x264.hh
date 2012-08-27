/*
 * file    : f642_x264.hh
 * project : f640
 *
 * Created on: Aug 22, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


/*
 * This "wrapper" is fully based on libx264 library @@@TODO
 */

#ifndef F642_X264_HH_
#define F642_X264_HH_

extern "C" {
#include <stdio.h>
#include <stdint.h>

#include <libavutil/pixdesc.h>
#include <libswscale/swscale.h>
#include <x264.h>
#include "muxers/output.h"
}

// Muxers
#define RAW_MUXER = 0
#define MKV_MUXER = 1
#define FLV_MUXER = 2
/*
 *
 */
namespace t508 {
namespace f642 {
    class X264 {
    public:
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
        int logLevel;

        // Encoding
        X264(int width, int height, enum PixelFormat pixin, float fps, int preset, int tune);
        ~X264();
        //
        int open(const char *path, int muxer);
        int setQP(int qp);
        int setQP(int qpmin, int qpmax, int qpstep);
        int setParam(const char *name, const char *value);
        int setNbThreads(int nb);
        int addFrame(uint8_t *rgb);
        int close();

        // Utils
        void setLogLevel(int logLevel);
        void dumpConfig();
        void dumpConfig(x264_param_t *prm);
    };
}
}

#endif /* F642_X264_HH_ */

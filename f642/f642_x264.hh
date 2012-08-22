/*
 * file    : f642_x264.hh
 * project : f640
 *
 * Created on: Aug 22, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
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
//        uint8_t *iPlan[8];
//        int     iStride[8];
//        uint8_t *oPlan[8];
//        int     oStride[8];
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

        // Encoding
        X264(int width, int height, float fps, int preset, int tune);
        ~X264();
        //
        int open(char *path);
        int setQP(int qp);
        int setQP(int qpmin, int qpmax, int qpstep);
        int setParam(const char *name, const char *value);
        int addFrame(uint8_t *rgb);
        int close();
    };
}
}

#endif /* F642_X264_HH_ */

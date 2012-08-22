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
        int     iStride[1];
        uint8_t *oPlan[4];
        int     oStride[4];
        // Encoding
        x264_t *x264;
        x264_nal_t *pp_nal;
        int pi_nal;
        x264_picture_t *rgb;
        x264_picture_t *ibp;

        //
        long              frame;

        // Encoding
        X264(int width, int height, float fps, const char *path);
        ~X264();
        //
        int setPresets(int preset, int tune);
        int setQP(int qp);
        int setQP(int qpmin, int qpmax, int qpstep);
        int setParam(const char *name, const char *value);
        int addFrame(uint8_t *rgb);
        int closeFile();
    };
}
}

#endif /* F642_X264_HH_ */

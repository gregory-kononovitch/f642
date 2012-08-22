/*
 * file    : f642_test.cc
 * project : f640
 *
 * Created on: Aug 22, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#include <time.h>
#include <sys/time.h>

#include "f642_x264.hh"

using namespace t508::f642;


//x264_preset_names[] = { "ultrafast", "superfast", "veryfast", "faster", "fast", "medium", "slow", "slower", "veryslow", "placebo", 0 };
//x264_tune_names[] = { "film", "animation", "grain", "stillimage", "psnr", "ssim", "fastdecode", "zerolatency", 0 };

int main(int argc, char *argv[]) {
    int r;
    int width = 600;
    int height = 400;
    uint8_t *rgb = (uint8_t*)calloc(3, width * height);
    memset(rgb, 0xFF, 3 * width * height);
    X264 *x264 = new X264(width, height, 20, 4, 1);
    //X264 *x264 = new X264(width, height, 20, 9, 0);
    fprintf(stderr, "X264 created\n");
    x264->setQP(0, 12, 2);
    x264->setLogLevel(0);
    x264->open("t.flv");
    x264->dumpConfig();
    fprintf(stderr, "X264 opened\n");
    //
    for(int i = 0 ; i < 100 ; i++) {
        uint8_t *t = rgb + 3 * i * width;
        memset(t, 0x20, 30 * width);
        r = x264->addFrame(rgb);
        //fprintf(stderr, "Add frame return %d\n", r);
        //fwrite(rgb, 1, 3*width*height, stdout);
    }
    //
    x264->close();
    fprintf(stderr, "X264 closed\n");

    delete x264;

    return 0;
}

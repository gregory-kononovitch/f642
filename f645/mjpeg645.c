/*
 * file    : mjpeg645.c
 * project : f640
 *
 * Created on: Sep 18, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */



#include "mjpeg645.h"



/**
 *
 */


/**
 * Stop at FF of next marker
 */
int next_marker645(mjpeg645_img *img) {
    uint16_t dep;

    if (img->offset + 2 >= img->size) {
        LOG("End of file");
        return EOF;
    }
    if (*img->ptr != 0xFF) {
        LOG("Bad alignment, no marker here");
        return -ENODATA;
    }
    dep = *((uint16_t*)(img->ptr));     // @@@

    img->ptr    += 2;
    img->offset += 2;
    //
    dep = *((uint16_t*)(img->ptr));
    if (img->offset + dep >= img->size) {
        LOG("Payload reached end of file");
        return EOF;
    }
    //
    img->ptr    += dep;
    img->offset += dep;
    if (*img->ptr != 0xFF) {
        LOG("Bad alignment, no marker");
        return -ENOKEY;
    }

    return 0;
}








//
//enum _marker645 {
//      SOI = 0
//    , EOI
//    , SOF0
//    , SOS
//    , DQT
//    , DRI
//    , APP0
//    , RST0
//    , RST1
//    , RST2
//    , RST3
//    , RST4
//    , RST5
//    , RST6
//    , RST7
//    , UNKN
//    , LAST
//};

/**
 *
 */
marker645 markers645[] = {
    {.key = 0xFFD8, .flags = 0 , .code = "SOI",  .desc = "Start of image"}
  , {.key = 0xFFD9, .flags = 0 , .code = "EOI",  .desc = "End of image"}
  , {.key = 0xFFC0, .flags = 1 , .code = "SOF0", .desc = "Baseline DCT"}
  , {.key = 0xFFDA, .flags = 1 , .code = "SOS",  .desc = "Start of scan"}
  , {.key = 0xFFDB, .flags = 1 , .code = "DQT",  .desc = "Quantization table(s)"}
  , {.key = 0xFFDD, .flags = 1 , .code = "DRI",  .desc = "Restart interval"}
  , {.key = 0xFFE0, .flags = 1 , .code = "APP0", .desc = "Application segment"}
  , {.key = 0xFFD0, .flags = 0 , .code = "RST0", .desc = "Restart 0"}
  , {.key = 0xFFD1, .flags = 0 , .code = "RST1", .desc = "Restart 1"}
  , {.key = 0xFFD2, .flags = 0 , .code = "RST2", .desc = "Restart 2"}
  , {.key = 0xFFD3, .flags = 0 , .code = "RST3", .desc = "Restart 3"}
  , {.key = 0xFFD4, .flags = 0 , .code = "RST4", .desc = "Restart 4"}
  , {.key = 0xFFD5, .flags = 0 , .code = "RST5", .desc = "Restart 5"}
  , {.key = 0xFFD6, .flags = 0 , .code = "RST6", .desc = "Restart 6"}
  , {.key = 0xFFD7, .flags = 0 , .code = "RST7", .desc = "Restart 7"}
  , {.key = 0xFF00, .flags = 0 , .code = "UNKN", .desc = "UNKNOWN MARKER"}
};

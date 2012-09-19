/*
 * file    : mjpeg645.h
 * project : f640
 *
 * Created on: Sep 18, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#ifndef MJPEG645_H_
#define MJPEG645_H_

//#define __USE_BSD

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>

#include <endian.h>
#include <byteswap.h>
#include <math.h>


#define FOG(s,...) fprintf(stderr, "%s: " s "\n", __func__, ##__VA_ARGS__);
#define LOG(s,...) fprintf(stderr, s "\n", ##__VA_ARGS__);


#define phtobe16p(p) (  htobe16(  *((uint16_t*)(p))  )  )

//
enum _marker645 {
#define     M645_SOI    0
      SOI = 0
#define     M645_EOI    1
    , EOI
#define     M645_SOF0   2
    , SOF0
#define     M645_SOS    3
    , SOS
#define     M645_DQT    4
    , DQT
#define     M645_DRI    5
    , DRI
#define     M645_APP0   6
    , APP0
#define     M645_RST0   7
    , RST0
#define     M645_RST1   8
    , RST1
#define     M645_RST2   9
    , RST2
#define     M645_RST3   10
    , RST3
#define     M645_RST4   11
    , RST4
#define     M645_RST5   12
    , RST5
#define     M645_RST6   13
    , RST6
#define     M645_RST7   14
    , RST7
#define     M645_UNKN   15
    , UNKN
#define     M645_LAST   16
    , LAST
};

typedef struct {
    uint16_t        key;
    uint8_t         index;
    char            flags;
    char            code[5];
    char            desc[33];
    //
    uint16_t        length;
} marker645;


//
typedef struct {
    // in
    uint8_t     *data;
    int         size;

    // out
    int         width;
    int         height;

    // work
    int         flags;
    uint8_t     *ptr;
    uint8_t     *eof;
    int         offset;

    // in
    int         log;

    //
    marker645   *markers;
    int         lm;
} mjpeg645_img;

/**
 * ASM
 */
extern long huffman645(void *dseg, void *dest);
extern long huf2man645(void *dseg, void *dest);
extern long huf3man645(void *dseg, void *dest);


/**
 *
 */
extern mjpeg645_img *alloc_mjpeg645_image(void *data, int size);
extern void free_mjpeg645_image(mjpeg645_img **img);

extern int load_next_marker645(mjpeg645_img *img);



#endif /* MJPEG645_H_ */

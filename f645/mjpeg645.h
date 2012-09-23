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

typedef struct _mjpeg645_img mjpeg645_img;

typedef struct {
    uint16_t        key;
    uint8_t         index;
    char            flags;
    char            code[5];
    char            desc[33];
    //
    uint16_t        length;

    //
    int             (*load_data)(mjpeg645_img *img, uint8_t *data);

} marker645;


//
struct _mjpeg645_img {
    // in
    uint8_t     *data;          // 0
    int         size;           // 8

    // out
    int         width;          // 12
    int         height;         // 16
    int         wxh;            // 20

    // decoder work
    int         flags;          // 24
    int         offset;         // 28
    uint8_t     *ptr;           // 32
    uint8_t     *eof;           // 40

    // Qanti
    int         *quantizY;      // 48           // @@@ *[4]
    int         *quantizUV;     // 56

    // Decode
    uint8_t     *pixels;        // 64
    int         ri;             // 72

    // in
    int         log;            // 76

    // out
    marker645   *markers;       //
    int         lm;

    //
    void        *ext1;          //

};

/**
 * ASM
 */
extern long huffman645(void *dseg, void *dest);
extern long huf2man645(void *dseg, void *dest);
extern long huf3man645(void *dseg, void *dest);
extern long huf4man645(void *dseg, void *dest);

extern long decode645(mjpeg645_img *img, void *dest, int size);

// test
extern uint8_t *scan645(uint8_t *ptr, int size, int *res);
extern uint8_t *scan645o(uint8_t *ptr, int size, int *res);


/**
 *
 */
extern mjpeg645_img *alloc_mjpeg645_image(void *data, int size);
extern void free_mjpeg645_image(mjpeg645_img **img);

extern int load_next_marker645(mjpeg645_img *img);



#endif /* MJPEG645_H_ */

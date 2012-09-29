/*
 * file    : ffmpeg.c
 * project : f640
 *
 * Created on: Sep 26, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#include "mjpeg645.h"

static int debug = 0;

//
static int zigzag645[64] =
{      0,  1,  5,  6, 14, 15, 27, 28
    ,  2,  4,  7, 13, 16, 26, 29, 42
    ,  3,  8, 12, 17, 25, 30, 41, 43
    ,  9, 11, 18, 24, 31, 40, 44, 53
    , 10, 19, 23, 32, 39, 45, 52, 54
    , 20, 22, 33, 38, 46, 51, 55, 60
    , 21, 34, 37, 47, 50, 56, 59, 61
    , 35, 36, 48, 49, 57, 58, 62, 63  };

static float    zuvcoscos645[64 * 64];      // {i x (r,c)}
// Huffman
static  int     hdcl645[2 * 256] = {0,2,3,4,5,6,14,30,62,126,254,510,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2,3,3,3,3,3,4,5,6,7,8,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static  int     hacl645[2 * 256] = {10,0,1,4,11,26,120,248,1014,65410,65411,-1,-1,-1,-1,-1,-1,12,27,121,502,2038,65412,65413,65414,65415,65416,-1,-1,-1,-1,-1,-1,28,249,1015,4084,65417,65418,65419,65420,65421,65422,-1,-1,-1,-1,-1,-1,58,503,4085,65423,65424,65425,65426,65427,65428,65429,-1,-1,-1,-1,-1,-1,59,1016,65430,65431,65432,65433,65434,65435,65436,65437,-1,-1,-1,-1,-1,-1,122,2039,65438,65439,65440,65441,65442,65443,65444,65445,-1,-1,-1,-1,-1,-1,123,4086,65446,65447,65448,65449,65450,65451,65452,65453,-1,-1,-1,-1,-1,-1,250,4087,65454,65455,65456,65457,65458,65459,65460,65461,-1,-1,-1,-1,-1,-1,504,32704,65462,65463,65464,65465,65466,65467,65468,65469,-1,-1,-1,-1,-1,-1,505,65470,65471,65472,65473,65474,65475,65476,65477,65478,-1,-1,-1,-1,-1,-1,506,65479,65480,65481,65482,65483,65484,65485,65486,65487,-1,-1,-1,-1,-1,-1,1017,65488,65489,65490,65491,65492,65493,65494,65495,65496,-1,-1,-1,-1,-1,-1,1018,65497,65498,65499,65500,65501,65502,65503,65504,65505,-1,-1,-1,-1,-1,-1,2040,65506,65507,65508,65509,65510,65511,65512,65513,65514,-1,-1,-1,-1,-1,-1,65515,65516,65517,65518,65519,65520,65521,65522,65523,65524,-1,-1,-1,-1,-1,2041,65525,65526,65527,65528,65529,65530,65531,65532,65533,65534,-1,-1,-1,-1,-1,4,2,2,3,4,5,7,8,10,16,16,0,0,0,0,0,0,4,5,7,9,11,16,16,16,16,16,0,0,0,0,0,0,5,8,10,12,16,16,16,16,16,16,0,0,0,0,0,0,6,9,12,16,16,16,16,16,16,16,0,0,0,0,0,0,6,10,16,16,16,16,16,16,16,16,0,0,0,0,0,0,7,11,16,16,16,16,16,16,16,16,0,0,0,0,0,0,7,12,16,16,16,16,16,16,16,16,0,0,0,0,0,0,8,12,16,16,16,16,16,16,16,16,0,0,0,0,0,0,9,15,16,16,16,16,16,16,16,16,0,0,0,0,0,0,9,16,16,16,16,16,16,16,16,16,0,0,0,0,0,0,9,16,16,16,16,16,16,16,16,16,0,0,0,0,0,0,10,16,16,16,16,16,16,16,16,16,0,0,0,0,0,0,10,16,16,16,16,16,16,16,16,16,0,0,0,0,0,0,11,16,16,16,16,16,16,16,16,16,0,0,0,0,0,0,16,16,16,16,16,16,16,16,16,16,0,0,0,0,0,11,16,16,16,16,16,16,16,16,16,16,0,0,0,0,0};
static  int     hdcc645[2 * 256] = {0,1,2,6,14,30,62,126,254,510,1022,2046,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2,2,2,3,4,5,6,7,8,9,10,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static  int     hacc645[2 * 256] = {0,1,4,10,24,25,56,120,500,1014,4084,-1,-1,-1,-1,-1,-1,11,57,246,501,2038,4085,65416,65417,65418,65419,-1,-1,-1,-1,-1,-1,26,247,1015,4086,32706,65420,65421,65422,65423,65424,-1,-1,-1,-1,-1,-1,27,248,1016,4087,65425,65426,65427,65428,65429,65430,-1,-1,-1,-1,-1,-1,58,502,65431,65432,65433,65434,65435,65436,65437,65438,-1,-1,-1,-1,-1,-1,59,1017,65439,65440,65441,65442,65443,65444,65445,65446,-1,-1,-1,-1,-1,-1,121,2039,65447,65448,65449,65450,65451,65452,65453,65454,-1,-1,-1,-1,-1,-1,122,2040,65455,65456,65457,65458,65459,65460,65461,65462,-1,-1,-1,-1,-1,-1,249,65463,65464,65465,65466,65467,65468,65469,65470,65471,-1,-1,-1,-1,-1,-1,503,65472,65473,65474,65475,65476,65477,65478,65479,65480,-1,-1,-1,-1,-1,-1,504,65481,65482,65483,65484,65485,65486,65487,65488,65489,-1,-1,-1,-1,-1,-1,505,65490,65491,65492,65493,65494,65495,65496,65497,65498,-1,-1,-1,-1,-1,-1,506,65499,65500,65501,65502,65503,65504,65505,65506,65507,-1,-1,-1,-1,-1,-1,2041,65508,65509,65510,65511,65512,65513,65514,65515,65516,-1,-1,-1,-1,-1,-1,16352,65517,65518,65519,65520,65521,65522,65523,65524,65525,-1,-1,-1,-1,-1,1018,32707,65526,65527,65528,65529,65530,65531,65532,65533,65534,-1,-1,-1,-1,-1,2,2,3,4,5,5,6,7,9,10,12,0,0,0,0,0,0,4,6,8,9,11,12,16,16,16,16,0,0,0,0,0,0,5,8,10,12,15,16,16,16,16,16,0,0,0,0,0,0,5,8,10,12,16,16,16,16,16,16,0,0,0,0,0,0,6,9,16,16,16,16,16,16,16,16,0,0,0,0,0,0,6,10,16,16,16,16,16,16,16,16,0,0,0,0,0,0,7,11,16,16,16,16,16,16,16,16,0,0,0,0,0,0,7,11,16,16,16,16,16,16,16,16,0,0,0,0,0,0,8,16,16,16,16,16,16,16,16,16,0,0,0,0,0,0,9,16,16,16,16,16,16,16,16,16,0,0,0,0,0,0,9,16,16,16,16,16,16,16,16,16,0,0,0,0,0,0,9,16,16,16,16,16,16,16,16,16,0,0,0,0,0,0,9,16,16,16,16,16,16,16,16,16,0,0,0,0,0,0,11,16,16,16,16,16,16,16,16,16,0,0,0,0,0,0,14,16,16,16,16,16,16,16,16,16,0,0,0,0,0,10,15,16,16,16,16,16,16,16,16,16,0,0,0,0,0};

// Quantization
static  float   quantization645[4 * 64] = {
         4,2,3,3,3,2,4,3,3,3,4,4,4,4,6,10,6,6,5,5,6,12,8,9,7,10,14,12,15,15,14,12,14,15,16,18,23,19,16,17,21,17,13,14,20,26,20,21,23,24,25,26,25,15,19,28,30,28,25,30,23,25,25,24
        ,4,4,4,6,5,6,11,6,6,11,24,16,14,16,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24
        ,4,2,3,3,3,2,4,3,3,3,4,4,4,4,6,10,6,6,5,5,6,12,8,9,7,10,14,12,15,15,14,12,14,15,16,18,23,19,16,17,21,17,13,14,20,26,20,21,23,24,25,26,25,15,19,28,30,28,25,30,23,25,25,24
        ,4,4,4,6,5,6,11,6,6,11,24,16,14,16,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24
};
//
static  int     ff[17] = {0x00,0x01,0x03,0x07,0x0F,0x1F,0x03F,0x7F,0xFF,0x1FF,0x3FF,0x7FF,0xFFF,0x1FFF,0x3FFF,0x7FFF,0xFFFF};

static char     header[220]   = {-1,-40,-1,-32,0,33,65,86,73,49,0,1,1,1,0,120,0,120,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-37,0,67,0,4,2,3,3,3,2,4,3,3,3,4,4,4,4,6,10,6,6,5,5,6,12,8,9,7,10,14,12,15,15,14,12,14,15,16,18,23,19,16,17,21,17,13,14,20,26,20,21,23,24,25,26,25,15,19,28,30,28,25,30,23,25,25,24,-1,-37,0,67,1,4,4,4,6,5,6,11,6,6,11,24,16,14,16,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,-1,-35,0,4,0,10,-1,-32,0,4,0,0,-1,-64,0,17,8,1,-64,3,32,3,1,33,0,2,17,1,3,17,1,-1,-38,0,12,3,1,0,2,17,3,17,0,63,0};

typedef struct {
    int                 width;
    int                 height;
    uint8_t             *rgb;

    //
    struct SwsContext   *swsCtxt;
    AVFrame             *bgra;
    AVFrame             *yuv422p;

    // Mjpeg
    uint8_t             *mjpeg;
    int                 mjpeg_size;
    uint64_t            bits;
    int                 mjoff;
    int                 mjdec;
    int                 rsti;
    int                 dc0[3];

    // Q
    float               quantization[4][64];

    // H @@@
    int                 hcodes[255];
    int                 hsizes[255];

    //
    int                 x0l;
    int                 y0l;
    int                 x0c;
    int                 y0c;

    //
    float               dct[64];

} mjpeg_codec645;

static void bin_str(long bits, uint8_t len) {
    while(len > 0) {
        len--;
        printf("%d ", (bits >> len) & 0x01);
    }
}

static void fillCosCos645() {
    int x, y, r, c, ix, ir;
    double u, v, d;
    float f;
    for(ix = 0 ; ix < 64 ; ix++) {
        x = ix % 8;
        y = ix / 8;
        if (!y) v = 1. / sqrt(2.);
        if (!x) u = 1. / sqrt(2.);
        for(ir = 0 ; ir < 64 ; ir++) {
            c = ir % 8;
            r = ir / 8;
            zuvcoscos645[(ix << 6) | ir] = (float)(0.25 * u * v * cos((2.*x + 1) * c * M_PI * 0.0625) * cos((2.*y + 1) * r * M_PI * 0.0625));
        }
    }
}

mjpeg_codec645 *mjpeg_codec(int width, int height) {
    int r;
    mjpeg_codec645 *mjpeg = (mjpeg_codec645*)calloc(1, sizeof(mjpeg_codec645));
    mjpeg->width  = width;
    mjpeg->height = height;
    mjpeg->rgb    = (uint8_t*)malloc(width * height * sizeof(uint32_t));
    mjpeg->mjpeg  = (uint8_t*)malloc(width * height);
    //
    // SwScaleCtxt
    mjpeg->swsCtxt = sws_getCachedContext(mjpeg->swsCtxt,
            width, height, PIX_FMT_BGRA,
            width, height, PIX_FMT_YUV422P,
            SWS_BICUBIC, NULL, NULL, NULL
    );
    mjpeg->bgra = avcodec_alloc_frame();
    avcodec_get_frame_defaults(mjpeg->bgra);
    mjpeg->bgra->data[0] = mjpeg->rgb;
    mjpeg->bgra->linesize[0] = sizeof(uint32_t) * width;
    mjpeg->bgra->width = width;
    mjpeg->bgra->height = height;
    mjpeg->bgra->format = PIX_FMT_BGRA;
    mjpeg->yuv422p = avcodec_alloc_frame();
    avcodec_get_frame_defaults(mjpeg->yuv422p);
    r = avpicture_alloc((AVPicture*)mjpeg->yuv422p, PIX_FMT_YUV422P, width, height);
    mjpeg->yuv422p->width = width;
    mjpeg->yuv422p->height = height;
    mjpeg->yuv422p->format = PIX_FMT_YUV422P;

    return mjpeg;
}

static void check_encode(mjpeg_codec645 *codec, int rst) {
    uint8_t *optr = codec->mjpeg + codec->mjoff;
    if (!rst && (codec->mjdec & 32)) {
        int j, bol = 0;
        codec->bits <<= 64 - codec->mjdec;
        if (debug) printf("\n %016lX :", codec->bits);
        for(j = 0 ; j < 4 ; j++) {
            uint8_t b = (codec->bits >> (56 - 8*j)) & 0xFF;
            if (b != 0xFF) {
                *(optr++) = b;
                codec->mjoff += 1;
                if (debug) bin_str(b, 8);
            } else {
                *(optr++) = 0xFF;
                *(optr++) = 0x00;
                codec->mjoff += 2;
                if (debug) bin_str(0xFF, 8);
                if (debug) bin_str(0x00, 8);
            }
        }
        if (debug) printf("\n");
        codec->bits >>= 64 - codec->mjdec;
        codec->mjdec -= 32;
    } else if (rst) {
        uint8_t i, b;
        int add = codec->mjdec / 8;
        add = 8 - (codec->mjdec - 8 * add);
        if (add == 8) add = 0;
        codec->bits <<= add;       // balign
        codec->bits |= ff[add];    // 111
        codec->mjdec += add;
        //
        add = codec->mjdec / 8;
        codec->bits <<= 64 - codec->mjdec;
        for(i = 0 ; i < add ; i++) {
            b = (codec->bits >> (56 - 8 * i)) & 0xFF;
            if (b != 0xFF) {
                *(optr++) = b;
                codec->mjoff++;
                if (debug) bin_str(b, 8);
            } else {
                *(optr++) = 0xFF;
                *(optr++) = 0x00;
                codec->mjoff += 2;
                if (debug) bin_str(0xFF, 8);
                if (debug) bin_str(0x00, 8);
            }
        }
        codec->bits  = 0;       //
        codec->mjdec = 0;
    }
}

static int mjpeg_dcthuf645(mjpeg_codec645 *codec, int comp, int quant, int *hdc, int *hac, uint8_t *plan, int index, int linesize) {
    int i, ixyz;
    uint8_t *ptr = plan + index;
    float src[64];
    double log2 = 1. / log(2.);
    //
    uint8_t *optr = codec->mjpeg + codec->mjoff;

    // -128
    if (debug) printf("Y 8x8 :\n");
    for(i = 0 ; i < 64 ; i++) {
        src[i] = -128.f + ptr[i % 8];
        if (debug) printf("%.0f |", src[i]);
        if (((i+1) & 0x07) == 0) {
            ptr += linesize;
            if (debug) printf("\n");
//            printf("vnkee : %d : %ld : %d\n", linesize, ptr - plan, ptr[0]);
        }
    }
    if (debug) printf("\n");
    // DCT
    if (debug) printf("ZDCT :\n");
    for(ixyz = 0 ; ixyz < 64 ; ixyz++) {
        codec->dct[zigzag645[ixyz]] = 0;
        for(i = 0 ; i < 64 ; i++) {
            codec->dct[zigzag645[ixyz]] += src[i] * zuvcoscos645[(ixyz << 6) | i];
//            if (ixyz == 10) {
//                printf("%.3f ", zuvcoscos645[(ixyz<< 6) | i]);
//                if ((i & 0x07) == 0x07) printf("\n");
//            }
        }
        codec->dct[zigzag645[ixyz]] /= quantization645[(quant<< 6) | ixyz];
        //printf("%.0f ", src[ixyz]);
//        printf("%.3f ", codec->dct[zigzag645[ixyz]]);
        if (debug) printf("%3.0f |", round(codec->dct[zigzag645[ixyz]]));
        //printf("%.1f ", quantization645[(quant<< 6) | ixyz]);
//        printf("%.3f ", zuvcoscos645[(ixyz<< 6) | ixyz]);
        if (debug) if ((ixyz & 0x07) == 0x07) printf("\n");
    }
    if (debug) printf("\n");
    // Huffman
    int iz0 = 0;
    int mag;
    uint8_t symb;
    uint32_t enc;
    // DC
    int v = round(codec->dct[0]) - codec->dc0[comp];
    codec->dc0[comp] += v;
    if (v != 0) {
        if (v > 0) {
            mag = 1 + (int)(log2 * log(+v));
        } else {
            mag = 1 + (int)(log2 * log(-v));
            v = v - 1;
        }
        symb = mag & 0x0F;
        iz0  = 0;
        codec->bits <<= hdc[256 | symb];
        codec->bits |= hdc[symb];
        codec->bits <<= mag;
        codec->bits |= (v & ff[mag]);
        codec->mjdec += hdc[256 | symb] + mag;
        check_encode(codec, 0);
        if (debug) printf("(%d %d %d %d)\n", hdc[symb], hdc[256 | symb], mag, v < 0 ? v+1 : v);
    }
    // Ac
    for(i = 1 ; i < 64 ; i++) {
        v = round(codec->dct[i]);
        if (v != 0) {
            //printf("%d ", iz0);
            if (v > 0) {
                mag = 1 + (int)(log2 * log(+v));
            } else {
                mag = 1 + (int)(log2 * log(-v));
                v = v - 1;
            }
            symb = ((iz0 << 4) | mag) & 0xFF;
            iz0  = 0;
            codec->bits <<= hac[256 | symb];
            codec->bits |= hac[symb];
            codec->bits <<= mag;
            codec->bits |= v & ff[mag];
            codec->mjdec += hac[256 | symb] + mag;
            //
//            printf("%u ", symb);
//            printf("%d ", hac[symb]);
//            printf("%d | %d %d %016lX|", hac[256 | symb], v, mag, codec->bits);
////            if ((i & 0x07) == 0x07) printf("\n");

            if (debug) printf("(%d %d %d %d)", hac[symb], hac[256 | symb], mag, v < 0 ? v+1 : v);
            check_encode(codec, 0);
        } else {
            if (iz0 < 15) {
                iz0++;
            } else {
                int j;
                for(j = i+1 ; j < 64 ; j++) {
                    if (round(codec->dct[j])) break;
                }
                if (j == 64) break;
                *(optr++) = 0xF0;
                codec->mjoff += 1;
                iz0 = 0;
            }

        }
    }
    // EOB
    codec->bits <<= hac[256 + 0];
    codec->bits  |= hac[0];
    codec->mjdec += hac[256 + 0];     // eob
    check_encode(codec, 0);

    return 0;
}

int mjpeg_encode645(mjpeg_codec645 *codec) {
    int i, r, log = 0;
    // prep
//    fillCosCos645();

    // RGB -> YUV
    r = sws_scale(codec->swsCtxt, (const uint8_t**)codec->bgra->data, codec->bgra->linesize, 0, codec->height, codec->yuv422p->data, codec->yuv422p->linesize);
    printf("SwScale return %d : %16X - %16X - %16X\n", r
            , *((long*)(codec->yuv422p->data[0] + 64))
            , *((long*)(codec->yuv422p->data[1] + 64))
            , *((long*)(codec->yuv422p->data[2] + 64)));
//    FILE *filp = fopen("yuv.out", "wb");
//    fwrite(codec->yuv422p->data[0], codec->height, codec->width, filp);
//    fwrite(codec->yuv422p->data[1], codec->height, codec->width/2, filp);
//    fwrite(codec->yuv422p->data[2], codec->height, codec->width/2, filp);
//    fflush(filp);
//    fclose(filp);
    printf("YUV image saved (%d - %d - %d).\n", codec->yuv422p->linesize[0], codec->yuv422p->linesize[1], codec->yuv422p->linesize[2]);
    //
//    i = 0;
//    int o = 8;
//    for(o = 0 ; o < 32 ; o += 8) {
//        for(r = 0 ; r < 8 ; r++) {
//            for(i = 0 ; i < 8 ; i++) {
//                printf("%d %d.%d.%d|", codec->yuv422p->data[0][o + i + codec->yuv422p->linesize[0]*r]
//                             , codec->rgb[4*(o + i + codec->yuv422p->linesize[0]*r)]
//                             , codec->rgb[4*(o + i + codec->yuv422p->linesize[0]*r) + 1]
//                             , codec->rgb[4*(o + i + codec->yuv422p->linesize[0]*r) + 2]
//                );
//                //if ((r+1) % 0x07 == 0) i += codec->yuv422p->linesize[0];
//            }
//            printf("\n");
//        }
//        printf("\n");
//    }
//    printf("\n");
    // DCT
    int ib, ibmax = (codec->width * codec->height) >> 7;    // / 64 / 2
    codec->x0l = codec->y0l = codec->x0c = codec->y0c = codec->rsti = 0;
    for(ib = 0 ; ib < ibmax ; ib++) {
        //
        mjpeg_dcthuf645(codec, 0, 0, hdcl645, hacl645, codec->yuv422p->data[0], codec->x0l + codec->width * codec->y0l, codec->yuv422p->linesize[0]);
        if (log) printf("Called dct at index %d (%d)\n", codec->x0l + codec->width * codec->y0l, codec->yuv422p->linesize[0]);
        if (log) printf("%d Encoded lumin block at (%d, %d) : %d bytes\n", ib, codec->x0l, codec->y0l, codec->mjoff);
        codec->x0l += 8;
        if (codec->x0l >= codec->width) {
            codec->x0l  = 0;
            codec->y0l += 8;
        }
//        break;
        //
        mjpeg_dcthuf645(codec, 0, 0, hdcl645, hacl645, codec->yuv422p->data[0], codec->x0l + codec->width * codec->y0l, codec->yuv422p->linesize[0]);
        if (log) printf("Called dct at index %d (%d)\n", codec->x0l + codec->width * codec->y0l, codec->yuv422p->linesize[0]);
        if (log) printf("Encoded lumin block : %d bytes\n", codec->mjoff);
        codec->x0l += 8;
        if (codec->x0l >= codec->width) {
            codec->x0l  = 0;
            codec->y0l += 8;
        }
//        continue;
        //
        mjpeg_dcthuf645(codec, 1, 1, hdcc645, hacc645, codec->yuv422p->data[1], codec->x0c + codec->width * codec->y0c, codec->yuv422p->linesize[1]);
        if (log) printf("Encoded chrom u block : %d bytes\n", codec->mjoff);
        //
        mjpeg_dcthuf645(codec, 2, 1, hdcc645, hacc645, codec->yuv422p->data[2], codec->x0c + codec->width * codec->y0c, codec->yuv422p->linesize[2]);
        codec->x0c += 8;
        if (codec->x0c >= codec->width) {
            codec->x0c  = 0;
            codec->y0c += 8;
        }
        if (log) printf("Encoded chrom v block : %d bytes\n", codec->mjoff);

        // RST
        if (ib % 10 == 9) {
            check_encode(codec, 1);
            uint8_t *optr = codec->mjpeg + codec->mjoff;
            *(optr++) = 0xFF;
            *(optr++) = 0xD0 | codec->rsti;
            codec->rsti = (codec->rsti + 1) & 0x07;
            codec->mjoff += 2;
            codec->dc0[0] = codec->dc0[1] = codec->dc0[2] = 0;
//            if (ib == 19) break;
        }
    }
    printf("Encoding finished : %d bytes\n", codec->mjoff);
}


int enc_ffm_test() {
    int i, j, x, y, r;
//    //
//    FILE *filp = fopen("mjpeg_encode.out", "rb");
//    char tmp[220];
//    fread(tmp, 1, 220, filp);
//    fclose(filp);
//    for(i = 0 ; i < 220 ; i++) {
//        printf("%d,",tmp[i]);
//    }
//    return 0;

    // prep
    fillCosCos645();
    //
    mjpeg_codec645 *codec = mjpeg_codec(800, 448);
    // fill rgb
    i = 0;
    uint8_t *ptr = codec->rgb;
    uint8_t *tmp = codec->rgb;
    for(y = 0 ; y < codec->height ; y++) {
        for(x = 0 ; x < codec->width ; x++) {
            int g = (x >> 3) + (codec->width >> 3) * (y >> 3);
            int c = g & 0x03;
            g = (g << 4) & 0xFF;
            *(ptr++) = ((c == 0 || c == 1 || c == 3) ? g : 0);
            *(ptr++) = ((c == 1 || c == 2 || c == 3) ? g : 0);
            *(ptr++) = ((c == 0 || c == 2 || c == 3) ? g : 0);
            *(ptr++) = 0xFF;
//            i++;
//            if (y == 0 && x < 32) {
//                printf("%d %d %d %d |", g, tmp[0], tmp[1], tmp[2]);
//                if (((x+1) & 0x0F) == 0x00) printf("\n");
//            }
//            tmp += 4;
        }
    }
//    FILE *filp = fopen("rgb.out", "wb");
//    fwrite(codec->rgb, 1, sizeof(int) * codec->width * codec->height, filp);
//    fflush(filp);
//    fclose(filp);
//    printf("RGB image done.\n");
    //
    r = mjpeg_encode645(codec);
    printf("MJPEG encode done.\n");
    if (r < 0) return r;
    //
    uint16_t eoi = 0xD9FF;
    FILE *filp = fopen("mjpeg_encode.out", "wb");
    fwrite(header, 1, 220, filp);
    fwrite(codec->mjpeg, 1, codec->mjoff, filp);
    fwrite(&eoi, 1, 2, filp);
    fflush(filp);
    fclose(filp);
}



int ffm_test1() {
    int r;
    int width  = 800 ;
    int height = 448;
    AVCodec             *codec;
    AVCodecContext      *decoderCtxt;
    AVFrame             *picture;
    AVPacket            pkt;
    struct SwsContext   *swsCtxt = NULL;
    AVFrame             *origin;
    AVFrame             *scaled;
    //
    av_register_all();
    avcodec_register_all();
    //
    printf("0\n");
    codec = avcodec_find_decoder(CODEC_ID_MJPEG);
    printf("1 %p %p - %p\n", codec->encode, codec->encode2, codec->decode);
    //avcodec_get_context_defaults3(decoderCtxt, codec);
    decoderCtxt = avcodec_alloc_context3(codec);    // PIX_FMT_YUVJ422P
    decoderCtxt->coded_width  = width;
    decoderCtxt->coded_height = height;
    decoderCtxt->width  = width;
    decoderCtxt->height = height;
    decoderCtxt->pix_fmt = PIX_FMT_YUVJ422P;
    decoderCtxt->time_base.num = 1;
    decoderCtxt->time_base.num = 25;
    printf("2\n");
    r = avcodec_open2(decoderCtxt, codec, NULL);
    printf("3 : %p - %p\n", decoderCtxt->codec->encode, decoderCtxt->codec->encode2);
    picture = avcodec_alloc_frame();
    printf("4\n");
    // SwScaleCtxt
    swsCtxt = sws_getCachedContext(swsCtxt,
            width, height, PIX_FMT_BGRA,
            width, height, PIX_FMT_YUVJ422P,
            SWS_BICUBIC, NULL, NULL, NULL
    );
    printf("5\n");
    origin = avcodec_alloc_frame();
    avcodec_get_frame_defaults(origin);
    r = avpicture_alloc((AVPicture*)origin, PIX_FMT_BGRA, width, height);
    origin->width = width;
    origin->height = height;
    origin->format = PIX_FMT_BGRA;
    scaled = avcodec_alloc_frame();
    avcodec_get_frame_defaults(scaled);
    r = avpicture_alloc((AVPicture*)scaled, PIX_FMT_YUVJ422P, width, height);
    scaled->width = width;
    scaled->height = height;
    scaled->format = PIX_FMT_YUVJ422P;

    //
    r = sws_scale(swsCtxt, (const uint8_t**)origin->data, origin->linesize, 0, height, scaled->data, scaled->linesize);
    printf("6 : %d\n", r);
    //
    int got_pak;
    pkt.data = malloc(width * height);
    pkt.size = width * height;
    printf("Scaled : %dx%d\n", scaled->width, scaled->height);
    avcodec_encode_video2(decoderCtxt, &pkt, scaled, &got_pak);
    if (got_pak) {
        printf("Encoded : size = %d\n", pkt.size);
    } else {
        printf("No Encoded\n");
    }
    printf("7");
    //
    avpicture_free((AVPicture*)origin);
    av_free(origin);
    av_free(swsCtxt);
    av_free(picture);
    avcodec_close(decoderCtxt);
    av_free(decoderCtxt);
}

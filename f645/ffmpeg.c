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

//
static int      zigzag645[64] = {0};
static float    zuvcoscos645[64 * 64];      // {iz x (r,c)}
// Huffman
static  int     hdcl_codes645[255];
static  int     hdcl_sizes645[255];
static  int     hacl_codes645[255];
static  int     hacl_sizes645[255];
static  int     hdcc_codes645[255];
static  int     hdcc_sizes645[255];
static  int     hacc_codes645[255];
static  int     hacc_sizes645[255];
// Quantization
static  float   quantization645[4][64];


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


int mjpeg_dct645(mjpeg_codec645 *codec, int quant, uint8_t *plan, int index, int linesize) {
    int i, ixy;
    uint8_t *ptr = plan + index;
    float src[64];
    // -128
    for(i = 0 ; i < 64 ; i++) {
        src[i] = -128.f + ptr[i];
        if ((i+1) & 0x07 == 0) {
            ptr += linesize;
        }
    }
    // DCT
    for(ixy = 0 ; ixy < 64 ; ixy++) {
        codec->dct[ixy] = 0;
        for(i = 0 ; i < 64 ; i++) {
            codec->dct[ixy] += src[i] * zuvcoscos645[(ixy << 6) + i];
        }
        codec->dct[ixy] /= quantization645[quant][ixy];
    }
    // Huffman
    int iz0 = 0;
    for(i = 0 ; i < 64 ; i++) {
        int v = round(codec->dct[i]);
        if (v != 0) {
            if (v > 0) {

            } else {

            }

        } else {
            iz0++;
        }
    }


    return 0;
}

int mjpeg_encode645(mjpeg_codec645 *codec) {
    int r;
    // RGB -> YUV
    r = sws_scale(codec->swsCtxt, (const uint8_t**)codec->bgra->data, codec->bgra->linesize, 0, codec->height, codec->yuv422p->data, codec->yuv422p->linesize);
    printf("SwScale return %d\n", r);
    // DCT
    int ib, ibmax = (codec->width * codec->height) >> 6;
    codec->x0l = codec->y0l = codec->x0c = codec->y0c = 0;
    for(ib = 0 ; ib < ibmax ; ib++) {



        //
        codec->x0l += 8;
        if (codec->x0l >= codec->width) {
            codec->x0l  = 0;
            codec->y0l += 8;
        }
    }
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

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

int ffm_test1() {
    int r;
    AVCodec             *codec;
    AVCodecContext      *decoderCtxt;
    AVFrame             *picture;
    AVPacket            pkt;
    //
    av_register_all();
    avcodec_register_all();
    //
    codec = avcodec_find_decoder(CODEC_ID_MJPEG);
    decoderCtxt = avcodec_alloc_context3(codec);    // PIX_FMT_YUVJ422P
    r = avcodec_open2(decoderCtxt, codec, NULL);
    picture = avcodec_alloc_frame();

    //
    pkt.data = 0;
    pkt.size = 0;
    int got_pict;
    int len = avcodec_decode_video2(decoderCtxt, picture, &got_pict, &pkt);
    if (got_pict) {
    }

    //
    av_free(picture);
    avcodec_close(decoderCtxt);
    av_free(decoderCtxt);
}

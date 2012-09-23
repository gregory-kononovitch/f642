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
 * helper function, @@@ no check
 */
static int _get_marker645(mjpeg645_img *img) {
    int i;
    uint16_t key = phtobe16p(img->ptr);

    if (img->log > 2) printf("Key = %04X ; pos = %ld\n", key, img->ptr - img->data);
    for(i = 0 ; i < LAST ; i++) {
        if (img->log > 3) printf("Marker %04X (%04X) %s-\"%s\" at position %d\n"
                , img->markers[i].key, key
                , img->markers[i].code
                , img->markers[i].desc
                , img->offset
        );

        if (img->markers[i].key == key) {
            if (img->log > 3) printf("\tkey = %04X found, flags = %d\n", key, img->markers[i].flags);
            if (img->markers[i].flags & 0x01) {
                img->markers[i].length = phtobe16p(img->ptr + 2);
                if (img->log > 3) printf("\t\tkey = %04X found, length = %d\n", key, img->markers[i].length);
            }

            if (img->markers[i].load_data) {
                img->markers[i].load_data(img, img->ptr);
            }

            return i;
        }
    }
    return -1;
}

/**
 *
 */
static int skip_dsegment645(mjpeg645_img *img) {
    while(img->ptr < img->eof) {
        if (*img->ptr != 0xFF) {
            img->ptr++;
            img->offset++;
        } else if (phtobe16p(img->ptr) != 0xFF00) {
            return 0;
        } else {
            img->ptr++;
            img->offset++;
        }
    }
    return -1;
}


/**
 * Load and stop at end of next marker
 */
int load_next_marker645(mjpeg645_img *img) {
    int m;

    if (img->offset + 2 >= img->size) {
        LOG("End of file");
        return EOF;
    }
    if (*img->ptr != 0xFF) {
        if ((img->lm >= M645_RST0 && img->lm <= M645_RST7) || img->lm == M645_SOS) {
            //LOG("Skip");
            skip_dsegment645(img);
        } else {
            LOG("Bad alignment, no marker here");
            return -ENODATA;
        }
    }
    m = _get_marker645(img);
    if (m < 0) {
        LOG("Unmanaged / Unkonwn marker : %02X %02X", *img->ptr, *(img->ptr + 1));
        return -ENODATA;
    }

    if (img->offset + 2 + img->markers[m].length >= img->size) {
        LOG("Payload reached end of file");
        return EOF;
    }
    //
    img->ptr    += 2 + img->markers[m].length;
    img->offset += 2 + img->markers[m].length;
    img->lm = m;

    //
    return m;
}

static int sof0_load645(mjpeg645_img *img, uint8_t *data) {
    //
    img->height = phtobe16p(data + 2 + 2 + 1);
    img->width  = phtobe16p(data + 2 + 2 + 1 + 2);
    img->wxh    = img->width * img->height;

    //
    if (img->wxh == 0 || img->wxh > 4000000) {
        LOG("Image dimensions out of range : width = %d, height = %d, size = %d", img->width, img->height, img->wxh);
        return -1;
    }
    //LOG("Image dimensions : width = %d, height = %d, size = %d", img->width, img->height, img->wxh);
    //
    int nbc = *(data + 2 + 2 + 1 + 2 + 2), i;
    //LOG("  ¤ %d components:", nbc);
    for(i = 0 ; i < nbc ; i++) {
        uint8_t n = *(data + 10 + 3*i);
        uint8_t v = *(data + 10 + 3*i + 1);
        uint8_t h = v >> 4;
        v &= 0x0f;
        uint8_t t = *(data + 10 + 3*i + 2);
        //LOG("    ¤ %d: sampling horizontal = %d, vertical = %d, quantization table = %d", n, h, v, t);
    }

    // rgb32
    if (!img->pixels) {
        img->pixels = calloc(img->wxh, sizeof(uint32_t));
        if (!img->pixels) {
            LOG("Cannot allocate memory for decoding.");
            return -1;
        }
        //
        LOG("Allocated memory for target picture");
        img->flags |= (1 << 30);
    } else {
        //LOG("Don't allocate memory for decoding, already present.")
    }
    //
    return 0;
}


/**
 * @@@
 * todo: *[4]
 */
static int dqt_load645(mjpeg645_img *img, uint8_t *data) {
    int i;
    int *tmp;
    //
    uint8_t n = *(data + 2 + 2);
    n &= 0x0f;
    if (n == 0) {
        tmp = img->quantizY;
    } else {
        tmp = img->quantizUV;
    }
    //
    //printf("Load quantization table n°%d :\n", n);
    for(i = 0 ; i < 64 ; i++) {
        tmp[i] = data[2 + 3 + i];
        //printf(" %u", data[2 + 3 + i]);
    }
    //printf("\n");
    return 0;
}

static int dri_load645(mjpeg645_img *img, uint8_t *data) {
    //
    img->ri = phtobe16p(data + 2 + 2);
    //LOG("Restart interval set to %d", img->ri);
    return 0;
}
/**
 *
 */
static marker645 markers645[] = {
    {.key = 0xFFD8, .index = SOI , .flags = 0 , .code = "SOI",  .desc = "Start of image", .load_data = NULL}
  , {.key = 0xFFD9, .index = EOI , .flags = 0 , .code = "EOI",  .desc = "End of image", .load_data = NULL}
  , {.key = 0xFFC0, .index = SOF0, .flags = 1 , .code = "SOF0", .desc = "Baseline DCT", .load_data = sof0_load645}
  , {.key = 0xFFDA, .index = SOS , .flags = 1 , .code = "SOS",  .desc = "Start of scan", .load_data = NULL}
  , {.key = 0xFFDB, .index = DQT , .flags = 1 , .code = "DQT",  .desc = "Quantization table(s)", .load_data = dqt_load645}
  , {.key = 0xFFDD, .index = DRI , .flags = 1 , .code = "DRI",  .desc = "Restart interval", .load_data = dri_load645}
  , {.key = 0xFFE0, .index = APP0, .flags = 1 , .code = "APP0", .desc = "Application segment", .load_data = NULL}
  , {.key = 0xFFD0, .index = RST0, .flags = 0 , .code = "RST0", .desc = "Restart 0", .load_data = NULL}
  , {.key = 0xFFD1, .index = RST1, .flags = 0 , .code = "RST1", .desc = "Restart 1", .load_data = NULL}
  , {.key = 0xFFD2, .index = RST2, .flags = 0 , .code = "RST2", .desc = "Restart 2", .load_data = NULL}
  , {.key = 0xFFD3, .index = RST3, .flags = 0 , .code = "RST3", .desc = "Restart 3", .load_data = NULL}
  , {.key = 0xFFD4, .index = RST4, .flags = 0 , .code = "RST4", .desc = "Restart 4", .load_data = NULL}
  , {.key = 0xFFD5, .index = RST5, .flags = 0 , .code = "RST5", .desc = "Restart 5", .load_data = NULL}
  , {.key = 0xFFD6, .index = RST6, .flags = 0 , .code = "RST6", .desc = "Restart 6", .load_data = NULL}
  , {.key = 0xFFD7, .index = RST7, .flags = 0 , .code = "RST7", .desc = "Restart 7", .load_data = NULL}
  , {.key = 0x0000, .index = UNKN, .flags = 0 , .code = "UNKN", .desc = "UNKNOWN MARKER", .load_data = NULL}
};


/**
 * Allocate memory for data if NULL && size != 0
 */
mjpeg645_img *alloc_mjpeg645_image(void *data, int size) {
    //
    mjpeg645_img *img = calloc(1, sizeof(mjpeg645_img));
    //
    if (data || !size) {
        img->data = data;
        img->size = size;
        //
        img->ptr  = img->data;
        img->eof  = img->data + size;
        img->lm   = -1;
    } else {
        img->data = calloc(1, size);
        img->size = size;
        //
        img->ptr  = img->data;
        img->eof  = img->data + size;
        img->flags |= (1 << 31);
        img->lm   = -1;
    }
    //
    img->quantizY  = (int*)calloc(64, sizeof(int));
    img->quantizUV = (int*)calloc(64, sizeof(int));

    //
    img->markers = calloc(LAST, sizeof(marker645));
    memcpy(img->markers, &markers645, LAST * sizeof(marker645));
    return img;
}

/**
 * do not free data if extern
 */
void free_mjpeg645_image(mjpeg645_img **img) {
    if (!*img) return;
    if ((*img)->markers) free((*img)->markers);
    if ((*img)->flags & (1 << 30)) free((*img)->pixels);
    if ((*img)->flags & (1 << 31)) free((*img)->data);
    free(*img);
    *img = NULL;
}


/**
 * test
 */
int mjpeg_decode645(mjpeg645_img *img, uint8_t *mjpeg, int size, uint8_t *pix) {
    //
    img->data = img->ptr = mjpeg;
    img->size = size;
    img->eof = img->data + img->size;
    img->offset = 0;
    img->pixels = pix;
    if (!img->ext1) {
        img->ext1 = calloc(1024, 1024);
    }
    // Scan
    int m;
    while( (m = load_next_marker645(img)) > -1 ) {
//        LOG("Found marker %04X %s-\"%s\" (%X) at position %d for %u bytes (+2)"
//                , img->markers[m].key
//                , img->markers[m].code
//                , img->markers[m].desc
//                , img->markers[m].flags
//                , img->offset
//                , img->markers[m].length
//        );
        if (m == M645_SOS) break;
    }

    // Data Segment
    int off1 = img->offset;
    m = load_next_marker645(img);
    if (m != M645_RST0) {
        //LOG("Didn't found RST0 after SOS, returning");
        goto err;
    }
    int off2 = img->offset - 2;
//    LOG("Will try to decode data segment from [%d ; %d[", off1, off2);

    //
    img->ptr = img->data + off1;
    long c1 = ReadTSC();
    uint8_t *addr = (uint8_t*)decode645(img, img->ext1, off2 - off1);
    long c2 = ReadTSC();
    //LOG("Decode: reached %ld for %ld µ ; %.3f Hz", addr - img->data, c2 - c1, 1.5e9 / (c2 - c1));

    return 0;

 err:
     return -1;
}

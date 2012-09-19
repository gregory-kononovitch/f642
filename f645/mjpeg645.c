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

    printf("Key = %04X ; pos = %ld\n", key, img->ptr - img->data);
    for(i = 0 ; i < LAST ; i++) {
//        printf("Marker %04X (%04X) %s-\"%s\" at position %d\n"
//                , img->markers[i].key, key
//                , img->markers[i].code
//                , img->markers[i].desc
//                , img->offset
//        );

        if (img->markers[i].key == key) {
            printf("\tkey = %04X found, flags = %d\n", key, img->markers[i].flags);
            if (img->markers[i].flags & 0x01) {
                img->markers[i].length = phtobe16p(img->ptr + 2);
                printf("\t\tkey = %04X found, length = %d\n", key, img->markers[i].length);
            }
            return i;
        }
    }
    return -1;
}

/**
 * Stop at FF of next marker
 */
int next_marker645(mjpeg645_img *img) {
    int m;

    if (img->offset + 2 >= img->size) {
        LOG("End of file");
        return EOF;
    }
    if (*img->ptr != 0xFF) {
        LOG("Bad alignment, no marker here");
        return -ENODATA;
    }
    m = _get_marker645(img);
    if (m < 0) {
        LOG("Unmanaged / Unkonwn marker");
        return -ENODATA;
    }


    if (img->offset + 2 + img->markers[m].length >= img->size) {
        LOG("Payload reached end of file");
        return EOF;
    }
    //
    img->ptr    += 2 + img->markers[m].length;
    img->offset += 2 + img->markers[m].length;

    //
    return m;
}






/**
 *
 */
static marker645 markers645[] = {
    {.key = 0xFFD8, .index = SOI , .flags = 0 , .code = "SOI",  .desc = "Start of image"}
  , {.key = 0xFFD9, .index = EOI , .flags = 0 , .code = "EOI",  .desc = "End of image"}
  , {.key = 0xFFC0, .index = SOF0, .flags = 1 , .code = "SOF0", .desc = "Baseline DCT"}
  , {.key = 0xFFDA, .index = SOS , .flags = 1 , .code = "SOS",  .desc = "Start of scan"}
  , {.key = 0xFFDB, .index = DQT , .flags = 1 , .code = "DQT",  .desc = "Quantization table(s)"}
  , {.key = 0xFFDD, .index = DRI , .flags = 1 , .code = "DRI",  .desc = "Restart interval"}
  , {.key = 0xFFE0, .index = APP0, .flags = 1 , .code = "APP0", .desc = "Application segment"}
  , {.key = 0xFFD0, .index = RST0, .flags = 0 , .code = "RST0", .desc = "Restart 0"}
  , {.key = 0xFFD1, .index = RST1, .flags = 0 , .code = "RST1", .desc = "Restart 1"}
  , {.key = 0xFFD2, .index = RST2, .flags = 0 , .code = "RST2", .desc = "Restart 2"}
  , {.key = 0xFFD3, .index = RST3, .flags = 0 , .code = "RST3", .desc = "Restart 3"}
  , {.key = 0xFFD4, .index = RST4, .flags = 0 , .code = "RST4", .desc = "Restart 4"}
  , {.key = 0xFFD5, .index = RST5, .flags = 0 , .code = "RST5", .desc = "Restart 5"}
  , {.key = 0xFFD6, .index = RST6, .flags = 0 , .code = "RST6", .desc = "Restart 6"}
  , {.key = 0xFFD7, .index = RST7, .flags = 0 , .code = "RST7", .desc = "Restart 7"}
  , {.key = 0x0000, .index = UNKN, .flags = 0 , .code = "UNKN", .desc = "UNKNOWN MARKER"}
};


/**
 * Allocate data if NULL
 */
mjpeg645_img *alloc_mjpeg645_image(void *data, int size) {
    //
    mjpeg645_img *img = calloc(1, sizeof(mjpeg645_img));
    //
    if (data) {
        img->data = data;
        img->size = size;
        img->ptr  = img->data;
        img->eof  = img->data + size;
    } else {
        img->data = calloc(1, size);
        img->size = size;
        img->ptr  = img->data;
        img->eof  = img->data + size;
        img->flags |= (1 << 31);
    }
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
    if ((*img)->flags & (1 << 31)) free((*img)->data);
    free(*img);
    *img = NULL;
}

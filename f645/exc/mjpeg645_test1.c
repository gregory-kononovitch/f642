/*
 * file    : mjpeg645-test1.c
 * project : f640
 *
 * Created on: Sep 18, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */




#include "../mjpeg645.h"

static void dump645(mjpeg645_img *img, int len) {
    int i;
    for(i = 0 ; i < len ; i++) {
        printf("%02X ", *(img->ptr + i));
    }
    printf("\n");
}


int main() {
    int lenb = 72027;
    void *tmp = calloc(1, lenb);

    //
    FILE *filp = fopen("/home/greg/t509/u610-equa/mjpeg800x448-8.dat", "rb");
    fread(tmp, 1, lenb, filp);
    fclose(filp);

    //
    mjpeg645_img *src = alloc_mjpeg645_image(tmp, lenb);

    // Scan
    LOG("Scan:");
    dump645(src, 40);
    int m;
    while( (m = load_next_marker645(src)) > -1 ) {
        LOG("Found marker %04X %s-\"%s\" (%X) at position %d for %u bytes (+2)"
                , src->markers[m].key
                , src->markers[m].code
                , src->markers[m].desc
                , src->markers[m].flags
                , src->offset
                , src->markers[m].length
        );
    }

    return 0;
}

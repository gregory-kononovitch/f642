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
    int i;
    //int lenb = 72027;
    int lenb = 612311;
    void *tmp = calloc(1, lenb);

    //
    //FILE *filp = fopen("/home/greg/t509/u610-equa/mjpeg800x448-8.dat", "rb");
    FILE *filp = fopen("hdc.dat", "rb");
    fread(tmp, 1, lenb, filp);
    fclose(filp);

    //
    mjpeg645_img *src = alloc_mjpeg645_image(tmp, lenb);

//    // Scan
//    LOG("Scan:");
//    dump645(src, 35);
//    int m;
//    while( (m = load_next_marker645(src)) > -1 ) {
//        LOG("Found marker %04X %s-\"%s\" (%X) at position %d for %u bytes (+2)"
//                , src->markers[m].key
//                , src->markers[m].code
//                , src->markers[m].desc
//                , src->markers[m].flags
//                , src->offset
//                , src->markers[m].length
//        );
//        if (m == M645_SOS) break;
//    }
//
//    // Data Segment
//    int off1 = src->offset;
//    dump645(src, 8);
//    m = load_next_marker645(src);
//    if (m != M645_RST0) {
//        LOG("Didn't found RST0 after SOS, returning");
//        goto err;
//    }
//    int off2 = src->offset - 2;
//    LOG("Will try to translate data segment from [%d ; %d[", off1, off2);
//
//    //
//    long symb = huffman645(src->data + off1, NULL);
//    LOG("Huffman: found symbol %d", symb);

    // ###
    //
    uint8_t *tres = calloc(1024, 1024);
    filp = fopen("hdc.out", "rb");
    fread(tres, 1, 1024*1024, filp);
    fclose(filp);
    printf("Results:\n");
    for(i = 0 ; i < 32 ; i++) printf("%u ", tres[i]);
    printf("\n");
    //
    uint8_t *hres = calloc(1024, 1024);
    long symb = huffman645(src->data, hres);
    LOG("Huffman: found symbol %d", symb);
    printf("Asm:\n");
    for(i = 0 ; i < 32 ; i++) printf("%u ", hres[i]);
    printf("\n");

    return 0;

err:
    free_mjpeg645_image(&src);
    return -1;
}

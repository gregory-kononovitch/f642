/*
 * file    : f650_brodge.c
 * project : f640
 *
 * Created on: Sep 6, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#include "../f650.h"
#include "../brodge/brodge650.h"





int main() {
    int width = 800;
    int height = 448;
    long l1, l2;
    brodge650 *brodge = brodge_init(width, height, 2);
    bgra650 bgra;
    //
    // Image
    bgra_alloc650(&bgra, brodge->width, brodge->height);
    //
    brodge_exec(brodge, &bgra);
    //
    int i, c;
    for(c = 0 ; c < 3 ; c++) {
        int off = c * brodge->width * brodge->height;
        for(i = 0 ; i < 8 ; i++) {
            printf("%d : %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f\n", i
                    , brodge->img[off + brodge->width * i + 0], brodge->img[off + brodge->width * i + 1]
                    , brodge->img[off + brodge->width * i + 2], brodge->img[off + brodge->width * i + 3]
                    , brodge->img[off + brodge->width * i + 4], brodge->img[off + brodge->width * i + 5]
                    , brodge->img[off + brodge->width * i + 6], brodge->img[off + brodge->width * i + 7]
            );
        }
        printf("\n");
    }

    printf("\n");
    for(i = 0 ; i < 8 ; i++) {
        printf("%d : %8X %8X %8X %8X %8X %8X %8X %8X\n", i
                , bgra.data[brodge->width * i + 0], bgra.data[brodge->width * i + 1]
                , bgra.data[brodge->width * i + 2], bgra.data[brodge->width * i + 3]
                , bgra.data[brodge->width * i + 4], bgra.data[brodge->width * i + 5]
                , bgra.data[brodge->width * i + 6], bgra.data[brodge->width * i + 7]
        );
    }

    //
    l1 = ReadTSC();
    bgra_clear2650(&bgra);
    l2 = ReadTSC();
    printf("Clear 2 : %ld\n", (l2 - l1));

    //
    l1 = ReadTSC();
    bgra_clear650(&bgra);
    l2 = ReadTSC();
    printf("Clear 1 : %ld\n", (l2 - l1));

    //
    l1 = ReadTSC();
    bgra_fill650(&bgra, 0xff000000);
    l2 = ReadTSC();
    printf("Fill 1 : %ld\n", (l2 - l1));

    //
    l1 = ReadTSC();
    bgra_fill2650(&bgra, 0xff000000);
    l2 = ReadTSC();
    printf("Fill 2 : %ld\n", (l2 - l1));

    brodge_free(&brodge);
    return 0;
}

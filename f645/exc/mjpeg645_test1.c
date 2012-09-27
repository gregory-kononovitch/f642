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

// asm
extern int64_t ReadTSC();

int asmtest(void *tmp, int lenb) {
    int j, k;
    uint8_t *ptr = tmp, *lim = tmp + lenb;
    while(ptr < lim) {
        if (*ptr == 0xFF) {
            j++;
            if (ptr[1] == 0x00) {
                k++;
            }
            if (ptr[1] == 0xD9) {
                break;
            }
        }
        ptr++;
    }
}

    static int rowsIz[] = {0, 0, 1, 2, 1, 0, 0, 1, 2, 3, 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 6, 5, 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 6, 7, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 7, 6, 5, 4, 3, 4, 5, 6, 7, 7, 6, 5, 6, 7, 7, };
    static int colsIz[] = {0, 1, 0, 0, 1, 2, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 6, 7, 7, 6, 5, 4, 3, 2, 3, 4, 5, 6, 7, 7, 6, 5, 4, 5, 6, 7, 7, 6, 7, };
int exportIdctUvCosCosf() {
    int iz, i;
    uint8_t *ptr = calloc(1, 4);
    float *f = (float*)ptr;
    uint32_t *fi = (uint32_t*)ptr;
    double u, v, d;

    *fi = 0x3B4731BE;
    printf("f = %.15f\n", *f);
    return 0;

    printf("\t\tdd\t");
    for(iz = 0 ; iz < 64 ; iz++) {
        int r = rowsIz[iz];
        int c = colsIz[iz];
        for(i = 0 ; i < 64 ; i++) {
            int x = i % 8;
            int y = i / 8;
            // uv
            if (r == 0) v = 1. / sqrt(2.);
            else v = 1.;
            if (c == 0) u = 1. / sqrt(2.);
            else u = 1.;
            d = .25 * u * v;
            //
//            d *= cos(0.0675 * (2.*x + 1.) * c * M_PI);
//            d *= cos(0.0675 * (2.*y + 1.) * r * M_PI);
            *f = (float)d;
//            printf("0x%08X%s", *fi, i%8==7?"":",");
//            if (i % 8 == 7) printf("\n\t\tdd\t");
            if (i % 8 == 0) printf("0x%08X = %.15f  / %.15f\n", *fi, *f, d);
        }
    }
}

int test_scan645() {
    int i, j, k;
    int lenb = 72025;   // 612311;
    long c1, c2;
    uint8_t *tmp = (uint8_t*)calloc(1, lenb);

    FILE *filp = fopen("/home/greg/t509/u610-equa/mjpeg800x448-8.dat", "rb");
    fread(tmp, 1, lenb, filp);
    fclose(filp);

    //
    j = k = 0;
    c1 = ReadTSC();
    for(i = 0 ; i < lenb ; i++) {
        if (tmp[i] == 0xFF) {
            j++;
            if (tmp[i+1] == 0x00) {
                k++;
            }
        }
    }
    c2 = ReadTSC();
    printf("%d FF (%d FF00) in file for %ld\n", j, k, (c2 - c1));

    //
    i = j = k = 0;
    uint8_t *ptr = tmp + 220, *lim = tmp + lenb;
    c1 = ReadTSC();
    while(ptr < lim) {
        if (*ptr == 0xFF) {
            j++;
            if (ptr[1] == 0x00) {
                k++;
            }
        }
        ptr++;
    }
    c2 = ReadTSC();
    printf("%d FF (%d FF00) in file for %ld : %02X%02X\n", j, k, (c2 - c1), tmp[lenb-2], tmp[lenb-1]);

    // scan test
    int res[20] = {0};

    c1 = ReadTSC();
    uint8_t *ret = scan645(tmp, lenb, res);
    c2 = ReadTSC();
    printf("Scan   : %ld in %.1f Kµ (%.3f) KHz  |  %d  |  %d  |  %d  |  %d / %d |\n", ret, 0.001*(c2 - c1), 1.5e6 / (c2 - c1), res[0], res[1], res[2], res[3], 72027);

//    uint8_t *l = (uint8_t*) tmp;
//    for(i = 0 ; i < 16 ; i++) l[i] = i;
//    l[2] = 0xFF;
//    l[3] = 0x00;
    //
    uint8_t *la = (uint8_t*) res;
    memset(res, 0, 16);

    c1 = ReadTSC();
    ret = scan645o(tmp + 220, lenb - 220, res);
    c2 = ReadTSC();

    char *dump(uint8_t *p) {static char c[89];int i;for(i=0;i<16;i++) snprintf(c+3*i,10,"%02X ",p[i]);return c;}
    printf("Ai : %s\n", dump(la));
    printf("Bi : %s\n", dump(la+16));
    printf("Ci : %s\n", dump(la+32));
    printf("%016lX : %016lX - %016lX\n", ret);
    printf("Scan-o : %ld in %.1f Kµ (%.3f) KHz  |  %d  |  %d  |  %d  |  %d - %d |\n", ret, 0.001*(c2 - c1), 1.5e6 / (c2 - c1), res[0], res[1], res[2], res[3], 72025);



    return 0;
}

static void dump(uint64_t u, int prec, int len) {
    int i;
    long l;
    uint64_t m = 0xffffffffffffffff;
//    u >>= 64 - prec;
//    if (prec == 64) {
//        u &= 0x7FFFFFFFFFFFFFFFL;
//    }
    for(i = 0 ; i < len ; i++) {
        l = u >> (prec - 1 - i);
        printf("%ld ", l);//(u & (1 << (63 - i))) ? 1 : 0);
        m >>= 1;
        //u &= (1 << (prec - i)) - 1;
        u &= m;
    }
}

extern int test_acl645(uint64_t data, int off, int *res);

int testacl2() {
    int i, j;
    int lenb = 72025;   // 612311;
    long c1, c2;
    uint8_t *tmp = (uint8_t*)calloc(1, lenb);
    uint64_t *ll = (uint64_t*)tmp;
    uint64_t lo;

    //
    FILE *filp = fopen("/home/greg/t509/u610-equa/mjpeg800x448-8.dat", "rb");
    fread(tmp, 1, lenb, filp);
    fclose(filp);
    //
    tmp  += 220;
    lenb -= 220;
    int off = lenb << 3;
    //
    int *res = calloc(1024, 16);
    res[1] = off;
    int r;
    long c = 2;
    lo = *ll;
    while(1) {
        long l = 1 + (int)(log(c) / log(2.) + .000001);
        lo = c << (64 - l);
        off = 64;

//        printf("\n    :"); dump(c, 8, 8);
//        printf("\nTest %ld, len %d\n", lo, l);
        r = test_acl645(lo, off, res);
        //dump(lo, 64, 16);
        //printf("Return %2d %3d %d %d %d . %d\n", l, res[0], res[1], res[2], res[3], 64 - res[1]);
        if (r == 0 && (64 - res[1]) == l) {
            dump(lo, 64, 16);
            printf("Return %2d - %3d : %d %d %d\n", l, res[0], res[1], res[2], res[3]);
        }
        //
        int o = off - res[1];
        off = res[1];
        c++;
        //if (c > 127) break;
        if (c >= (1<<16)) break;
    }

}



//int testacl() {
//
//    uint64_t code = 0xABFD7FA7F97DAAABL;
//    int off = 64;
//    int *res = calloc(1024, 16);
//    res[1] = 64;
//    int r;
//    do {
//        code <<= (off - res[1]);
//        off = res[1];
//        r = test_acl645(code, off, res);
//        printf("Return %d : %d %d %d %d\n", r, res[0], res[1], res[2], res[3]);
//    } while(r == 0 && res[1] > 0);
//
//    printf("Return %d : %d %d %d %d\n", r, res[0], res[1], res[2], res[3]);
//    return 0;
//}

extern int ffm_test1();
extern int enc_ffm_test();
int main() {
    int i, j;
    long c1, c2;

    //return testacl2();
    //return ffm_test1();
    return enc_ffm_test();

    //
    FILE *filp = fopen("/home/greg/t509/u610-equa/mjpeg800x448-8.dat", "rb");
    fseek(filp, 0, SEEK_END);
    fpos_t p;
    fgetpos(filp, &p);
    int lenb = p.__pos;   // 612311;
    uint8_t *tmp = (uint8_t*)calloc(1, lenb);
    fseek(filp, 0, SEEK_SET);
    fread(tmp, 1, lenb, filp);
    fclose(filp);
    printf("Load %d bytes\n", lenb);

    //
    mjpeg645_img *mjpeg = alloc_mjpeg645_image(tmp, lenb);
    mjpeg->pixels = NULL;
    //
    uint8_t *log = mjpeg->ext1 = calloc(1024, 1024);

    // Scan

    //
    while(1) {
        mjpeg->ext1 = log;
        c1 = ReadTSC();
        long r = mjpeg_decode645(mjpeg, NULL, lenb, NULL);
        c2 = ReadTSC();
        LOG("Decode: return %ld (%ld) for %ld µ", r , r - (long)mjpeg->data, c2 - c1);

        int *part = (int*)log;
        float *fart = (float*)log;
        for(i = 0 ; i < 100000 ; i++) {
            if (part[i] == -9999) {
                printf("X\n");
            } else if (part[i] == -9998) {
                printf("| ");
            } else if (part[i] == -10000) {
                break;
            } else if (*((long*)(log + 4*i)) == 0) {
                if (*((long*)(log + 4*i + 8)) == 0)
                    if (*((long*)(log + 4*i + 16)) == 0)
                        if (*((long*)(log + 4*i + 24)) == 0)
                            if (*((long*)(log + 4*i + 32)) == 0)
                                if (*((long*)(log + 4*i + 40)) == 0)
                                    if (*((long*)(log + 4*i + 48)) == 0)
                                        if (*((long*)(log + 4*i + 56)) == 0)
                            break;
            } else {
                printf("%d  ", part[i]);
                //printf("%.2f  ", fart[i]);
                //printf("%08X  ", part[i]);
            }
        }
        printf("\n");

//        if (mjpeg->size > 10000) {
//            continue;
//        }
        break;

    //    printf("Decode sequence:\n");
    //    int w = 35;
    //    for(j = 0 ; j < 24 ; j++) {
    //        for(i = 0 ; i < w ; i++) {
    //            printf("%u ", hres[i + w * j]);
    //        }
    //        printf("\n");
    //    }
    //    printf("Done\n");

        int stats[64] = {0};
        int min = 0;
        int max = 0;
        printf("Decode sequence:\n|");
        for(i = 0 ; i < 70024 ; i++) {
            if (log[i] != 0xFF) {
                printf("%3u|", log[i]);
            } else {
                printf(" - |\n|", log[i]);
                if (*((long*)(log + i + 1)) == 0) break;
            }
            if (log[i] == 255 && log[i-2] == 0) {
                min++;
                if (log[i-1] > max) max = log[i-1];
                stats[log[i-1]]++;
            }
        }
        printf("\n");
        printf("Done\n");

        printf("Stats for %d blocks, max = %d\n", min, max);
        max = 0;
        for(i = 0 ; i < 64 ; i++) {
            max += stats[i];
        }
        min = 0;
        for(i = 0 ; i < 64 ; i++) {
            min += stats[i];
            printf("NB[%d] = %d  ( %.2f  -  %.2f )\n", i, stats[i], 100. * stats[i] / max, 100.*min/max);
        }
    }

// --------------------------------------------------------------------------------
//    // ###
//    long l1, l2;
//    //
//    uint8_t *tres = calloc(1024, 1024);
//    filp = fopen("hdc.out", "rb");
//    fread(tres, 1, 1024*1024, filp);
//    fclose(filp);
//    printf("Results:\n");
//    for(i = 0 ; i < 32 ; i++) printf("%u ", tres[i]);
//    printf("\n");
//    //
//    //
//    printf("Starting asm tests\n");
//    long symb = 0;
//    uint8_t *add;
//    int cofs = 500000;
//    uint16_t cod = 55807;
//    l1 = ReadTSC();
//    for(i = cofs/50000 ; i >= 0 ; i--) {    // 10 * 50000
//        //add = (uint8_t*)hu2fman645(mjpeg->data, 50000, hres);
//        //add = (uint8_t*)hu2fman645(&cod, 1, hres);
//        add = (uint8_t*)huf4man645(&cod, hres);
//    }
//    l2 = ReadTSC();
//    printf("End of asm tests\n");
//    LOG("Huffman: stop at %ld for %ld = %ld (%ld M - %.1f ms)", add - mjpeg->data, l2 - l1, (l2 - l1) / ((cofs/50000)*50000), 1.*(l2 - l1) / 1.5e6, (l2 - l1) / 1000000);
//    //
//    printf("Asm: %ld\n", add);
//    for(i = 0 ; i < 32 ; i++) printf("%u ", hres[i]);
//    printf("\n");
//    //
//    i = 0;
//    while(i < 1024*1024 && (hres[i] == tres[i])) i++;
//    printf("Equals n'til %d\n", i);





    return 0;

err:
    free_mjpeg645_image(&mjpeg);
    return -1;
}

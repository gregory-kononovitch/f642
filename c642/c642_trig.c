/*
 * c642_trig.c
 *
 * Date Oct 12, 2011
 * Author and copyright (C) 2011 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

/*
 * function: 0 -> 512 = 1 turn
 * args : num , den = int/long
 */
static int nb_val = 512;
int main(int argc, char *argv[])
{
    int i, fd;
    signed short *cosinus, *val;
    double v;
    //
    fd = open("c642-trig.bin", O_WRONLY | O_CREAT | O_TRUNC);
    if ( fd < 1 ) {
        printf("Pb with file, exiting.\n");
        return -1;
    }
    printf("File open.\n");

    //
    cosinus = malloc( 3 * nb_val * sizeof(short));
    if ( !cosinus ) {
        printf("Memory pb, exiting.\n");
        close(fd);
        return -1;
    }
    printf("Data allocated.\n");

    //
    val = cosinus;
    for(i = 0 ; i < nb_val ; i++) {
        v = 0x7FFF * cos(2. * M_PI * i / nb_val);
        cosinus[i] = (signed short) v;

        //
        if ( ((4 * i) != nb_val) && ((4 * i) != 3 * nb_val) ) {
            v = tan(2. * M_PI * i / 512);
            if ( v < 1 ) {
                cosinus[nb_val + 2 * i] = (signed short) (v * 0x7FFF);
                cosinus[nb_val + 2 * i + 1] = 0x7FFF;
            } else if ( v == 1 ) {
                cosinus[nb_val + 2 * i] = 1;
                cosinus[nb_val + 2 * i + 1] = 1;
            } else if ( abs(v) < 16 ) {
                cosinus[nb_val + 2 * i] = (signed short) (v * 0x1000);
                cosinus[nb_val + 2 * i + 1] = 0x1000;
            } else if ( abs(v) < 0x7FFF ) {
                cosinus[nb_val + 2 * i] = (signed short) v;
                cosinus[nb_val + 2 * i + 1] = 1;
            }
        } else if ((4 * i) != nb_val) {
            cosinus[nb_val + 2 * i] = -0x7FFF;
            cosinus[nb_val + 2 * i + 1] = 0;
        } else {
            cosinus[nb_val + 2 * i] = 0x7FFF;
            cosinus[nb_val + 2 * i + 1] = 0;
        }
    }

    //
    i = write(fd, cosinus, 3 * nb_val * sizeof(short));
    if ( i != 3 * nb_val * sizeof(short) ) {
        printf("Pb generating the file, exiting.\n");
        free(cosinus);
        close(fd);
        return -1;
    }
    printf("File succesfully generated.\n");

    free(cosinus);
    close(fd);
    return 0;
}

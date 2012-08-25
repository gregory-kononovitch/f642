/*
 * file    : f643_test.c
 * project : f640
 *
 * Created on: Aug 25, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <unistd.h>
#include <string.h>

#include "f643.h"


int main(int argc, char *argv[]) {

    struct f643 *f643 = f643_alloc(512, 288, 20);
    //
    f643_init_x264(f643, 5, 0);
    f643_init_mp4(f643, 90000);
    //
    f643_open_file("tst.mp4", f643);
    f643->pix_in = PIX_FMT_BGR24;
    f643_setup_x264_codec(f643);
    f643_open_x264_track(f643);
    //
    while(f643_add_x264_frame(f643) > -1) {

    }
    //
    f643_close_x264_codec(f643);
    f643_close_file(f643);
    f643_free(&f643);
    return 0;
}

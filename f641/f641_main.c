/*
 * file    : f641_main.c
 * project : f640
 *
 * Created on: Dec 19, 2011
 * Author and copyright (C) 2011 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */



#include "../f640/f640_queues.h"

#define f641_HZ             30

#define F641_SNAPED_422     0x00000001  // DQBUF
#define F641_SNAPED_MJPEG   0x00000002  // DQBUF & read_frame
#define F641_SNAPED_MPEG    0x00000004  // read_frame
#define F641_SNAPED         0x0000000F

#define F641_DECODED_422    0x00000010
#define F641_DECODED_422P   0x00000020
#define F641_DECODED_420P   0x00000040
#define F641_DECODED        0x000000F0


#define F641_WATCHED        0x00000004


#define F641_GRAYED         0x00000008
#define F641_EDGED          0x00000010
#define F641_CONVERTED      0x00000020
#define F641_TAGGED         0x00000040
#define F641_BROADCASTED    0x00000080
#define F641_RECORDED       0x00000100
#define F641_RELEASED       0x00000200

struct f641_im {
    long sleep_ms;
    int  fact;
};


int f641_process(struct f640_stone *stone) {
    struct f641_im *im = stone->private;

    usleep(im->sleep_ms * (1000 + 1000 * im->fact * random() / RAND_MAX));
}

int main(int argc, char *argv[]) {


    return 0;
}

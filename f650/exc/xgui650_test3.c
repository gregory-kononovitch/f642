/*
 * file    : xgui650_test3.c
 * project : f640 (| f642 | t508.f640)
 *
 * Created on: Sep 14, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <errno.h>

#include "../f650.h"
#include "../f650_fonts.h"
#include "../brodge/brodge650.h"
#include "../layout/f650_layout.h"
#include "../../f691/f691.h"

#include "../../f640/f641/f641_v4l2.h"


static char escape = 0;

static int notify_key_pressed0(void *ext, unsigned long key, int x, int y, int mask, uint64_t time) {
    desk654 *desk = (desk654*)ext;

    switch(key) {
        case XK_Escape:
            escape = 1;
            break;
    }
    return 0;
}

static int notify_key_released0(void *ext, unsigned long key, int x, int y, int mask, uint64_t time) {
    desk654 *desk = (desk654*)ext;


    return 0;
}

    // Mouse
static int notify_mouse_motion0(void *ext, int x, int y, int mask, uint64_t time) {
    desk654 *desk = (desk654*)ext;

    return 0;
}

static int notify_button_pressed0(void *ext, int button, int x, int y, int mask, uint64_t time) {
    desk654 *desk = (desk654*)ext;


    return 0;
}

static int notify_button_released0(void *ext, int button, int x, int y, int mask, uint64_t time) {
    desk654 *desk = (desk654*)ext;


    return 0;
}

static int notify_scroll0(void *ext, int button, int x, int y, int mask, uint64_t time) {
    desk654 *desk = (desk654*)ext;


    return 0;
}


/*
 *
 */
extern long yuv422togray32(void *gray32, void *yuv422, int widtha16, int height);

static int test() {
    int r;

    //
    Status st = XInitThreads();
    FOG("XInitThreads return %d",st);
    //
//    XrmInitialize();
//    FOG("XrmInitialize done");

    //
    xgui691 *gui = xgui_create691(640, 480, 1);
    if (!gui) return -1;
    long period = 57143;
    //
    r = xgui_open_window691(gui, "Test");
    if (r) {
        LOG("Can't open window, returning");
        return -1;
    }
    LOG("Window opened %p :%dx%d", gui->pix1, gui->width, gui->height);

    //
    int i;
    long l;
    struct timeval tv0, tv1, tv2, tv3, tv4;
    struct timeval tvb1, tvb2;
    long broad;
    struct f641_v4l2_parameters v4l2;
    struct v4l2_buffer frame;
    memset(&v4l2, 0, sizeof(struct f641_v4l2_parameters));
    f641_setup_v4l2(&v4l2, "/dev/video1", gui->width, gui->height, 0x56595559, 30, 2);
    f641_prepare_buffers(&v4l2);

    //
    events691 events;
    memset(&events, 0, sizeof(events691));
    events.notify_key_pressed  = notify_key_pressed0;
    xgui_listen691(gui, &events, NULL);

    //
    f641_stream_on(&v4l2);

    //
    long num_frame = 0;
    gettimeofday(&tv0, NULL);
    gettimeofday(&tv1, NULL);
    gettimeofday(&tv3, NULL);
    while(!escape) {
        // DeQueue
        memset(&frame, 0, sizeof(struct v4l2_buffer));
        frame.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        frame.memory = V4L2_MEMORY_MMAP;
        if(ioctl(v4l2.fd, VIDIOC_DQBUF, &frame) == -1) {
            FOG("VIDIOC_DQBUF: %s\n", strerror(errno));
            break;
        }

        gettimeofday(&tvb1, NULL);

        // Convert
        yuv422togray32(gui->pix1, v4l2.buffers[frame.index].start, v4l2.width, v4l2.height);

        // Put
        show691(gui, 0, 0, 0, 0, 0, gui->width, gui->height);

        //
        gettimeofday(&tvb2, NULL);
        timersub(&tvb2, &tvb1, &tvb2);
        broad += tvb2.tv_usec;

        // EnQueue
        if(ioctl(v4l2.fd, VIDIOC_QBUF, &frame) == -1) {
            FOG("VIDIOC_QBUF: %s\n", strerror(errno));
            break;
        }
        //
        num_frame++;
        //
        if (num_frame % 70 == 0) {
            gettimeofday(&tv4, NULL);
            timersub(&tv4, &tv3, &tv4);
            LOG("Frame %ld for %.3f Hz for %ld µs", num_frame, 70. / (1. * tv4.tv_sec + 0.000001 * tv4.tv_usec), broad / 70);
            broad = 0;
            gettimeofday(&tv3, NULL);
        }
        //
    }
    //
    f641_stream_off(&v4l2);
    //
    xgui_stop691(gui);
    xgui_close_window691(gui);
    xgui_free691(&gui);
    return 0;
}



int main() {

    test();


    return 0;
}

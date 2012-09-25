/*
 * file    : xgui650_test3.c
 * project : f640 (| f642 | t508.f640)
 *
 * Created on: Sep 14, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

//extern "C" {
#include <errno.h>
#include <sys/ioctl.h>
#include <time.h>
#include <sys/time.h>

#include "../f650.h"
#include "../f650_fonts.h"
#include "../brodge/brodge650.h"
#include "../layout/f650_layout.h"
#include "../../f691/f691.h"

#include "../../f640/f641/f641_v4l2.h"

#include "../../f645/mjpeg645.h"

extern long yuv422togray32(void *gray32, void *yuv422, int widtha16, int height);

//}
//#include "../../f642/f642_x264.hh"
//using namespace t508::f642;


typedef struct test3_ {
    int pause;
    int save;
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
} test3;


static char escape = 0;

static int notify_key_pressed0(void *ext, unsigned long key, int x, int y, int mask, uint64_t time) {
    test3 *test = (test3*)ext;

    switch(key) {
        case XK_Escape:
            escape = 1;
            pthread_mutex_lock(&test->mutex);
            test->pause = 0;
            pthread_cond_broadcast(&test->cond);
            pthread_mutex_unlock(&test->mutex);
            break;
        case XK_space:
                pthread_mutex_lock(&test->mutex);
                test->pause = !test->pause;
                pthread_cond_broadcast(&test->cond);
                pthread_mutex_unlock(&test->mutex);
            break;
        case XK_Return:
            test->save = 1;
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

static int test() {
    int r;

    //
    Status st = XInitThreads();
    FOG("XInitThreads return %d",st);
    //
//    XrmInitialize();
//    FOG("XrmInitialize done");

    //
    int format = 0;     //0x47504A4D;        // 0x56595559
    FILE *filp = NULL;
    //
    xgui691 *gui = xgui_create691(800, 448, 1);
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
    int lenb = 72025;   // 612311;
    uint8_t *tmp;
    long l;
    struct timeval tv0, tv1, tv2, tv3, tv4;
    struct timeval tvb1, tvb2;
    long broad;
    long uops = 0;
    struct f641_v4l2_parameters v4l2;
    struct v4l2_buffer frame;
    memset(&v4l2, 0, sizeof(struct f641_v4l2_parameters));
    if (format == 0x56595559) {
        f641_setup_v4l2(&v4l2, "/dev/video0", gui->width, gui->height, 0x56595559, 30, 3);
        f641_prepare_buffers(&v4l2);
    } else if (format == 0x47504A4D) {
        f641_setup_v4l2(&v4l2, "/dev/video2", gui->width, gui->height, 0x47504A4D, 24, 10);
        f641_prepare_buffers(&v4l2);
    } else if (!format) {
        int lenb = 72025;   // 612311;
        tmp = (uint8_t*)malloc(lenb);
        filp = fopen("/home/greg/t509/u610-equa/mjpeg800x448-8.dat", "rb");
        fread(tmp, 1, lenb, filp);
        fclose(filp);
        filp = NULL;
    }
    //
    //filp = fopen("y800x448-422-2.dat", "wb");


    //

    //
    mjpeg645_img *mjpeg = alloc_mjpeg645_image(NULL, 0);
    mjpeg->log = 0;
    mjpeg->pixels = (uint8_t*)gui->pix1;

    // Thread
    test3 *test = (test3*)calloc(1, sizeof(test3));
    pthread_mutex_init(&test->mutex, NULL);
    pthread_cond_init(&test->cond, NULL);

    //
    events691 events;
    memset(&events, 0, sizeof(events691));
    events.notify_key_pressed  = notify_key_pressed0;
    xgui_listen691(gui, &events, test);

    //
    if (format > 0) {
        f641_stream_on(&v4l2);
    }

    //
    long num_frame = 0;
    gettimeofday(&tv0, NULL);
    gettimeofday(&tv1, NULL);
    gettimeofday(&tv3, NULL);
    while(!escape) {
        pthread_mutex_lock(&test->mutex);
        while(test->pause) {
            pthread_cond_wait(&test->cond, &test->mutex);
        }
        pthread_mutex_unlock(&test->mutex);

        if (format > 0) {
            // DeQueue
            memset(&frame, 0, sizeof(struct v4l2_buffer));
            frame.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            frame.memory = V4L2_MEMORY_MMAP;
            if(ioctl(v4l2.fd, VIDIOC_DQBUF, &frame) == -1) {
                FOG("VIDIOC_DQBUF: %s\n", strerror(errno));
                break;
            }
        }

        gettimeofday(&tvb1, NULL);

        // Convert
        if (format == 0x56595559) {
            yuv422togray32(gui->pix1, (uint8_t*)v4l2.buffers[frame.index].start, v4l2.width, v4l2.height);
        } else if (format == 0x47504A4D) {
            uops += mjpeg_decode645(mjpeg, (uint8_t*)v4l2.buffers[frame.index].start, v4l2.buffers[frame.index].length, (uint8_t*)gui->pix1);
        } else if (!format) {
            uops += mjpeg_decode645(mjpeg, tmp, lenb, (uint8_t*)gui->pix1);
        } else {
            // ###
            if (num_frame >= 100 && num_frame < 101) {
                fwrite(gui->pix1, 1, gui->width * gui->height, filp);
            }
        }


        // Put
        xgui_show691(gui, 0, 0, 0, 0, 0, gui->width, gui->height);
//        show691(gui, 0, 0, 0, 0, 0, gui->width, gui->height);

        //
        gettimeofday(&tvb2, NULL);
        timersub(&tvb2, &tvb1, &tvb2);
        broad += tvb2.tv_usec;

        if (format > 0) {
            // EnQueue
            if(ioctl(v4l2.fd, VIDIOC_QBUF, &frame) == -1) {
                FOG("VIDIOC_QBUF: %s\n", strerror(errno));
                break;
            }
        } else if (!format) {
            usleep(10000);
        }
        //
        num_frame++;
        //
        int fl = 60;
        if (num_frame % fl == 0) {
            gettimeofday(&tv4, NULL);
            timersub(&tv4, &tv3, &tv4);
            LOG("Frame %ld for %.3f Hz for %ld µs and %ld µops", num_frame, 1. * fl / (1. * tv4.tv_sec + 0.000001 * tv4.tv_usec), broad / fl, uops / fl);
            uops = 0;
            broad = 0;
            gettimeofday(&tv3, NULL);
        }
        //
    }
    //
    //
    if (format == 0x56595559) {
        f641_stream_off(&v4l2);
    } else if (format == 0x47504A4D) {
        f641_stream_off(&v4l2);
        free_mjpeg645_image(&mjpeg);
    } else if (!format) {

    } else {
        // ### raw svg
        if (filp) {
            fflush(filp);
            fclose(filp);
        }
    }
    //
    xgui_stop691(gui);
    xgui_close_window691(gui);
    xgui_free691(&gui);
    return 0;
}



int main(int argc, char *argv[]) {

    test();


    return 0;
}

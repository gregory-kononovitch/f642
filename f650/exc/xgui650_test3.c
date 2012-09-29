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
#include "../../f642/f642_x264.h"
#include "../../f645/mjpeg645.h"

extern long yuv422togray32(void *gray32, void *yuv422, int widtha16, int height);

//}
//#include "../../f642/f642_x264.hh"
//using namespace t508::f642;


typedef struct test3_ {
    int         pause;
    int         save;
    int         encode;
    pthread_mutex_t mutex;
    pthread_cond_t  cond;

    // x264
    f642x264    *x264;
    struct timeval pts;
    queue642    *queue;
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

//
void *xgui_th650(void *p) {
    int r;
    struct timeval tv;
    test3 *test = (test3*)p;

    printf("Thread x264 started\n");
    while(test->encode) {
//        printf("Wait queue from add frame\n");
        int i = dequ2ue642(test->queue, &tv);
        if (i < 0) break;
        ;
        f642_addFrame(test->x264, &test->queue->yuv[i], tv);
        ;
        enqu1ue642(test->queue, i);
//        printf("Enqueued from add frame\n");
    }

    return NULL;
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
    int width = 800;
    int height = 448;
    int fps = 24;
    int format = 0;     //0x47504A4D;        // 0x56595559
    int save = 0;
    FILE *filp = NULL;
    //
    xgui691 *gui = xgui_create691(width, height, 1);
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
        f641_setup_v4l2(&v4l2, "/dev/video1", gui->width, gui->height, 0x47504A4D, fps, 10);
        f641_prepare_buffers(&v4l2);
    } else if (!format) {
        int lenb = 161012; //72025;   // 612311;
        tmp = (uint8_t*)malloc(lenb);
//        filp = fopen("/home/greg/t509/u610-equa/mjpeg800x448-8.dat", "rb");
        filp = fopen("../f645/mjpeg_encode.out", "rb");
        fread(tmp, 1, lenb, filp);
        fclose(filp);
        filp = NULL;
    }
    //
    printf("1\n");

    // mjpeg
    mjpeg645_img *mjpeg = alloc_mjpeg645_image(NULL, 0);
    mjpeg->log = 0;
    mjpeg->pixels = (uint8_t*)gui->pix1;

    // x264
    f642x264 *x264 = 0;
    queue642 *queue = 0;
    if (save) {
        printf("1\n");
        f642x264 *x264 = init642_x264(gui->width, gui->height, fps, 4, 1);
        x264->loglevel = 3;
        f642_open(x264, "test.mkv3", 1);
        printf("2\n");
        queue642 *queue = queue_open642(x264, 3);
        printf("3\n");
    }

    // Thread
    test3 *test = (test3*)calloc(1, sizeof(test3));
    test->x264 = x264;
    test->queue = queue;
    test->encode = 1;
    gettimeofday(&test->pts, NULL);
    pthread_mutex_init(&test->mutex, NULL);
    pthread_cond_init(&test->cond, NULL);

    //
    events691 events;
    memset(&events, 0, sizeof(events691));
    events.notify_key_pressed  = notify_key_pressed0;
    xgui_listen691(gui, &events, test);

    //
    if (save && format >= 0) {
        pthread_attr_init(&test->queue->attr);
        pthread_create(&test->queue->thread, &test->queue->attr, xgui_th650, test);
    }
    if (format > 0) {
        f641_stream_on(&v4l2);
    }

    //
    int db = 0;
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
        int f;
        if (format == 0x56595559) {
            yuv422togray32(gui->pix1, (uint8_t*)v4l2.buffers[frame.index].start, v4l2.width, v4l2.height);
        } else if (format == 0x47504A4D) {
            if (save) f = dequ1ue642(test->queue);
            if (save && f < 0) continue;
            uops += mjpeg_decode645(mjpeg, (uint8_t*)v4l2.buffers[frame.index].start, v4l2.buffers[frame.index].length, db ? (uint8_t*)gui->pix2 : (uint8_t*)gui->pix1);
            timersub(&v4l2.buf.timestamp, &tv0, &v4l2.buf.timestamp);
            if (save) enqu2ue642(test->queue, f, v4l2.buf.timestamp);
        } else if (!format) {
            if (save) f = dequ1ue642(test->queue);
            if (save && f < 0) continue;
            uops += mjpeg_decode645(mjpeg, tmp, lenb, (uint8_t*)gui->pix1);
            if (save) enqu2ue642(test->queue, f, tvb1);
        } else {
            // ###
            if (num_frame >= 100 && num_frame < 101) {
                fwrite(gui->pix1, 1, gui->width * gui->height, filp);
            }
        }

        // Put
        xgui_show691(gui, db, 0, 0, 0, 0, gui->width, gui->height);
//        db = 0;
        if (db) {
            db = 0;
        } else {
            db = 1;
        }

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
    LOG("Thread test3 ended");
    //usleep(1000000);
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
    if (save) {
        queue_close642(&test->queue);
        LOG("Queue closed");
        f642_close(test->x264);
        LOG("X264 closed");
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

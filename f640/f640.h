/*
 * f640.h
 *
 *  Created on: Sep 11, 2011
 *  Author and copyright (C) 2011 : Gregory Kononovitch
 *  License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 */

#ifndef F640_H_
#define F640_H_

#define F640_RESET       "\033[0m"//(0)
#define F640_UNDER       "\033[4m"//(0)
#define F640_ERASE_LINE  "\033[2J"
#define F640_BOLD        "\033[1m"//(1)
#define F640_FG_BLACK    "\033[30m"//(30)
#define F640_FG_RED      "\033[31m"//(31)
#define F640_FG_GREEN    "\033[32m"//(32)
#define F640_FG_BROWN    "\033[33m"//(33)
#define F640_FG_BLUE     "\033[34m"//(34)
#define F640_FG_PURPLE   "\033[35m"//(35)
#define F640_FG_CYAN     "\033[36m"//(36)
#define F640_FG_GREY     "\033[37m"//(37)


// The SCALE macro converts a value (sv) from one range (sf -> sr)
//   to another (df -> dr).
#define SCALE(df, dr, sf, sr, sv) (((sv - sf) * (dr - df) / (sr - sf)) + df)


#include <stddef.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include <sys/ioctl.h>
#include <linux/videodev2.h>

/*
 *
 */
typedef struct {
    void *start;
    size_t length;
} v4l2_buffer_t;


#include "../../f610-equa/f611/f611_common.h"

#include "f640_graphics.h"

#include <pthread.h>


struct f640_grid {
    int         width;
    int         height;
    int         size;

    int         wlen;
    int         hlen;
    int         gsize;

    int         cols;
    int         rows;
    int         num;

    uint16_t    *index_grid;
    uint32_t    *grid_index;
    int64_t     *grid_coefs;
};

struct f640_line {
    int             index;
    // Data
    struct v4l2_buffer buf;
    v4l2_buffer_t   *buffers;           // ref (tab)
    int             last;               // in (v4l buffer index)
    int             actual;             // in (v4l buffer index)
    int             previous_line;
    uint64_t        frame;

    // Grid
    struct f640_grid    *grid;          // ref
    int                 *width;         // Copies
    int                 *height;
    int                 *cell_width;
    int                 *cell_height;
    int                 *rows;
    int                 *cols;
    long                *rms;
    long                *grid_min;       // out
    long                *grid_max;       // out
    long                *grid_values;   // alloced
    long                grid_th;        // in

    // Graphics
    enum PixelFormat    srcFormat;
    enum PixelFormat    dstFormat;
    struct f640_image   *gry;           // out
    struct f640_image   *yuv;           // in ( / out )
    struct f640_image   *rgb;           // out
    AVPicture           *yuvp;          // MJPEG
    uint8_t             *mjpeg;

    // Decode
//    AVCodecContext      *decoderCtxt;
//    AVFrame             *picture;
//    AVPacket            avpkt;

    // Store
    struct output_stream *stream;       // ref

    // Broadcast
    struct SwsContext   *swsCtxt;
    struct f051_log_env *log_env;       // ref

    //
    struct timeval tv00, tv01, tvd0, tvd1, tv10, tv11, tv20, tv21, tv30, tv31, tv40, tv41;
    double t, t0, td, t1, t2, t3, t4;
};

#define F640_MULTI_MAX 3
struct f640_video_queue {
    struct f640_line    *lineup;
    int     *lines;
    int     size;
    int     next;
    int     last;
    int     real;

    uint64_t    nb_in;
    uint64_t    nb_out;
    uint64_t    oldest;
    uint64_t    done[ 2 * F640_MULTI_MAX ];

    pthread_mutex_t mutex;
    pthread_cond_t  cond;
};
extern int f640_init_queue(struct f640_video_queue *queue, struct f640_line *lineup, int count);
extern int f640_enqueue_buffer(struct f640_video_queue *queue, struct v4l2_buffer *v4l2_buf);
extern int f640_enqueue_line(struct f640_video_queue *queue, int line);
extern int f640_dequeue_line(struct f640_video_queue *queue);
extern int f640_free_line(struct f640_video_queue *queue, int line);
extern int f640_dequeue_sync(struct f640_video_queue *queue);
extern int f640_sync(struct f640_video_queue *queue, uint64_t f);
extern int f640_uns_queue_size(struct f640_video_queue *queue);

struct f640_video_lines {
    struct f640_grid        *grid;
    struct f640_video_queue snaped;
    struct f640_video_queue buffered;
    struct f640_video_queue decoded;
    struct f640_video_queue watched;
    struct f640_video_queue converted;
    struct f640_video_queue recorded;
    struct f640_video_queue released;

    int fd;
    pthread_mutex_t ioc;

    int  fps;
    long rms;
    long grid_min_value;
    long grid_max_value;
    long recorded_frames;

    //
    int fd_stream;
    int fd_grid;
    int fd_edge;
};

struct f640_processing_point {
    struct f640_video_queue snaped;
};

struct f640_imaging_point {
    struct f640_video_queue watched;
};

struct f640_broadcast_point {
    struct f640_video_queue converted;
};

struct f640_release_point {
    struct f640_video_queue recorded;
};

struct f640_v4l_point {
    int fd;
    pthread_mutex_t ioc;
};

extern struct f640_grid* f640_make_grid(int width, int height, int factor);
extern struct f640_line* f640_make_lineup(v4l2_buffer_t *buffers, int nbuffers, struct f640_grid *grid, enum PixelFormat dstFormat, struct output_stream *stream, struct f051_log_env *log_env, long threshold);

extern void* f640_decode(void *video_lines);
extern void* f640_watch(void *video_lines);
extern void* f640_watch_mj(void *video_lines);
extern void* f640_convert(void *video_lines);
extern void* f640_record(void *video_lines);
extern void* f640_record_mj(void *video_lines);
extern void* f640_release(void *video_lines);

double f640_duration(struct timeval tv1, struct timeval tv0);
double f640_frequency(struct timeval tv1, struct timeval tv0);

#endif /* F640_H_ */

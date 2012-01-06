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

    pthread_mutex_t mutex_coefs;
    int64_t     *grid_coefs;
    int32_t     *grid_ratio;
};

/*
 * nb0 : real           16 : ~0.5 s     int32_t for suming & int16_t for result
 * nb1 : first agreg     8 : ~4.0 s     int32_t for suming & int16_t for result
 * nb2 : second agreg   16 : ~1.0 mn    int32_t for suming & int16_t for result
 * nb3 : third agreg    32 : ~0.5 h     int32_t for suming & int16_t for result
 */
struct f640_grid2 {
    int         nb0;
    int         shift0;
    int         mask0;
    int32_t     *grid_ratio_0;    //

    int         nb01;
    int         shift01;
    int         mask01;

    int         nb1;
    int         shift1;
    int         mask1;
    int16_t     *grid_ratio_10;   // avg, min, max, del :   1 * cells
    int16_t     *grid_ratio_1;    // avg, min, max, del : nb1 * cells

    int         nb2;
    int         shift2;
    int         mask2;
    int16_t     *grid_ratio_20;   // avg, min, max, del :   1 * cells
    int16_t     *grid_ratio_2;    //

    int         nb3;
    int         shift3;
    int         mask3;
    int16_t     *grid_ratio_30;   // avg, min, max, del :   1 * cells
    int16_t     *grid_ratio_3;    //

    //
    pthread_mutex_t mutex;
    int         index0;
    int         index1;
    int         index2;
    int         index3;

    // Sky
    uint16_t    *acc;
    uint16_t    *sky;
    uint8_t     *skyDif;
};

struct f640_line {
    struct f640_line    *lineup;            // +
    int                 lineup_length;      // +
    int                 index;

    // Raw Data
    v4l2_buffer_t       *buffers;           // ref (tab)
    // Infos
    union {
        // V4L2
        struct v4l2_buffer buf;
        // FFMpeg infos
        struct {
            int ffm_index;
            int ffm_bytes_used;
            struct timeval ffm_timestamp;
        };
    };

    //
    int             last;                   // in (v4l buffer index)
    int             actual;                 // in (v4l buffer index)
    int             previous_line;
    uint64_t        frame;
    int             *histo;                 // +

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
    int                 flaged;

    // Grid2
    int                 time2;

    // Graphics
    enum PixelFormat    srcFormat;
    enum PixelFormat    dstFormat;
    uint8_t             *mjpeg;         // DQ / get_frame           // 2        * 500Ko
    AVPicture           *yuvp;          // decode MJPEG / MPEG      // 1,5 - 2  * 500Ko
    struct f640_image   *yuv;           // in ( / out )             // 2        * 500Ko

    struct f640_image   *gry;           // out                      // 1        * 500Ko
    struct f640_image   *rgb;           // out                      // 3        * 500Ko
                                                                    // 3Mo - 3.75Mo - 4Mo


    // Store
    struct output_stream *stream;       // ref

    // Convert
    struct SwsContext   *swsCtxt;

    // Broadcast
    struct f051_log_env *log_env;       // ref

    //
    struct timeval tv00, tv01, tvd0, tvd1, tv10, tv11, tv20, tv21, tve0, tve1, tvg0, tvg1, tvb0, tvb1, tvr0, tvr1, tv40, tv41;
    double t, t0, td, t1, t2, te, tg, tb, tr, t4;
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
    struct f640_grid2       *grid2;


    struct f640_video_queue snaped;
    struct f640_video_queue buffered;
    struct f640_video_queue decoded;
    struct f640_video_queue watched;
    struct f640_video_queue edged;
    struct f640_video_queue converted;
    struct f640_video_queue recorded;
    struct f640_video_queue released;

    int fd;
    pthread_mutex_t ioc;

    int  fps;
    long rms;
    long grid_min_value;
    long grid_max_value;

    //
    int  recording;
    char record_path[256];
    long recorded_frames;
    struct timeval tvr;

    //
    char stream_path[256];
    int fd_stream;
    char grid_path[256];
    int fd_grid;
    char edge_path[256];
    int fd_edge;
    char get_path[256];
    int fd_get;
    char grid10_path[256];
    int fd_grid10;
    char grid20_path[256];
    int fd_grid20;
    char grid30_path[256];
    int fd_grid30;
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

extern struct f640_grid*  f640_make_grid(int width, int height, int factor);
extern struct f640_grid2* f640_make_grid2(struct f640_grid *grid);
extern struct f640_line*  f640_make_lineup(v4l2_buffer_t *buffers, int nbuffers, struct f640_grid *grid, enum PixelFormat dstFormat, struct output_stream *stream, struct f051_log_env *log_env, long threshold);

extern void* f640_decode(void *video_lines);
extern void* f640_watch(void *video_lines);
extern void* f640_watch_mj(void *video_lines);
extern void* f640_gray_mj(void *video_lines);
extern void* f640_convert(void *video_lines);
extern void* f640_record(void *video_lines);
extern void* f640_record_mj(void *video_lines);
extern void* f640_release(void *video_lines);
extern void* f640_read(void *video_lines);

double f640_duration(struct timeval tv1, struct timeval tv0);
double f640_frequency(struct timeval tv1, struct timeval tv0);

#endif /* F640_H_ */

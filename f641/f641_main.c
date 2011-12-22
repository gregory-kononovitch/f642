/*
 * file    : f641_main.c
 * project : f640
 *
 * Created on: Dec 19, 2011
 * Author and copyright (C) 2011 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#include "../f640/f641/f641.h"


#define F641_HZ             5

#define F641_SNAPED_422     0x00000001  // DQBUF
#define F641_SNAPED_MJPEG   0x00000002  // DQBUF & read_frame
#define F641_SNAPED_MPEG    0x00000004  // read_frame
#define F641_SNAPED         0x0000000F

#define F641_DECODED_422    0x00000010
#define F641_DECODED_422P   0x00000020
#define F641_DECODED_420P   0x00000040
#define F641_DECODED        0x000000F0


#define F641_WATCHED        0x00000100
#define F641_EDGED          0x00000200
#define F641_GRIDED         0x00000400


#define F641_CONVERTED      0x00001000
#define F641_GRAYED         0x00002000
#define F641_TAGGED         0x00004000

#define F641_BROADCASTED    0x00010000
#define F641_RECORDED       0x00020000

#define F641_RELEASED       0x80000000

struct f641_im {
    int k;
};


int f641_process_sn(struct f640_stone *stone) {
    static long frame = 1;
    struct f641_im *im = stone->private;
    usleep(1000 * 1000 / (2 * F641_HZ) );

    pthread_spin_lock(&stone->spin);
    stone->frame = frame++;
    pthread_spin_unlock(&stone->spin);

    return 0;
}

// 5 - 20ms
int f641_process_de(struct f640_stone *stone) {
    struct f641_im *im = stone->private;
    usleep( 1000 * (1000 + 1000 * 3 * random() / RAND_MAX) / (2 * F641_HZ));
    return 0;
}

// 5 - 15ms
int f641_process_wa(struct f640_stone *stone) {
    struct f641_im *im = stone->private;
    usleep( 1000 * (1000 + 1000 * 2 * random() / RAND_MAX) / (3 * F641_HZ));
    return 0;
}

// 5 - 15ms
int f641_process_cv(struct f640_stone *stone) {
    struct f641_im *im = stone->private;
    usleep( 1000 * (1000 + 1000 * 2 * random() / RAND_MAX) / (3 * F641_HZ));
    return 0;
}

// 1 - 1.5ms
int f641_process_tg(struct f640_stone *stone) {
    struct f641_im *im = stone->private;
    usleep( 1000 * (1000 + 500 * 1 * random() / RAND_MAX) / (7 * F641_HZ));
    return 0;
}

// 1 - 1.5ms
int f641_process_br(struct f640_stone *stone) {
    struct f641_im *im = stone->private;
    usleep( 1000 * (750 + 500 * 1 * random() / RAND_MAX) / (5 * F641_HZ));
    return 0;
}

// 1 - 10ms
int f641_process_rc(struct f640_stone *stone) {
    struct f641_im *im = stone->private;
    usleep( 1000 * (1000 + 500 * 1 * random() / RAND_MAX) / (5 * F641_HZ));
    return 0;
}

int f641_process_re(struct f640_stone *stone) {
    struct f641_im *im = stone->private;
    return 0;
}

static int debug_test = 0;
static int f641_queue_test() {
    int nb = 15;
    int i;
    struct timeval tv1, tv2, tv3;
    struct f640_stone *stones = calloc(nb, sizeof(struct f640_stone));
    for(i = 0 ; i < nb ; i++) {
        f640_make_stone(&stones[i], i);
        stones[i].stones = stones;
        stones[i].private = calloc(1, sizeof(struct f641_im));
        ((struct f641_im*)stones[i].private)->k = i;
    }

    long actbf[2] = {F641_SNAPED, -1};
    struct f640_queue *buffers = f640_make_queue(stones, nb, actbf, 0, 0, 0);

    long actsn[2] = {F641_DECODED, -1};
    struct f640_queue *snapped = f640_make_queue(stones, nb, actsn, 0, 0, 5);

    long actde[3] = {F641_WATCHED, F641_CONVERTED, -1};
    struct f640_queue *decoded = f640_make_queue(stones, nb, actde, 0, 0, 5);

    long actft[2] = {F641_TAGGED, -1};
    struct f640_queue *fortagg = f640_make_queue(stones, nb, actft, F641_WATCHED | F641_CONVERTED, 0, 0);

    long acttg[3] = {F641_BROADCASTED, F641_RECORDED, -1};
    struct f640_queue *tagged  = f640_make_queue(stones, nb, acttg, 0, 1, 0);

    long actfr[2] = {F641_RELEASED, -1};
    struct f640_queue *forrel  = f640_make_queue(stones, nb, actfr, F641_BROADCASTED | F641_RECORDED, 0, 0);

    f640_make_thread(1, F641_SNAPED,        buffers, 1, snapped, 0, 0, f641_process_sn);
    f640_make_thread(4, F641_DECODED,       snapped, 1, decoded, 0, 0, f641_process_de);
    f640_make_thread(4, F641_WATCHED,       decoded, 1, fortagg, 0, 2, f641_process_wa);
    f640_make_thread(4, F641_CONVERTED,     decoded, 1, fortagg, 0, 0, f641_process_cv);
    f640_make_thread(3, F641_TAGGED,        fortagg, 1, tagged,  0, 0, f641_process_tg);
    f640_make_thread(1, F641_BROADCASTED,   tagged,  1, forrel,  0, 1, f641_process_br);
    f640_make_thread(1, F641_RECORDED,      tagged,  1, forrel,  0, 1, f641_process_rc);

    usleep(100*1000);

    for(i = 0 ; i < nb ; i++) {
        f640_enqueue(buffers, 0, i, F641_RELEASED);
    }
    printf("%d Buffers ready\n", nb);

    usleep(300*1000);

    struct timespec ts;
    ts.tv_sec = 5;
    ts.tv_nsec = 0;
    long cpt = 0;
    clock_t times1, times2;
    times1 = clock();
    gettimeofday(&tv1, NULL);
    while(1) {
        // Dequeue
        if (debug_test) printf("Try DeQueue for release\n");
        int key = f640_dequeue(forrel, 1, F641_RELEASED);
        if (debug_test) printf("DeQueued %d\n", key);
        // Check
        if ( key < 0) {
            printf("End of in queue, exiting\n");
            break;
        }
        struct f640_stone *stone = &forrel->stones[key];

        // Processing
        if (debug_test) printf("Releasing %d (%ld)\n", key, stone->frame);
        pthread_spin_lock(&stone->spin);
        stone->frame  = 0;
        stone->status = 0;
        pthread_spin_unlock(&stone->spin);


        // Backtrack

        // Enqueue
        f640_enqueue(buffers, 0, key, F641_RELEASED);
        cpt++;

        if ( (cpt % (2 * F641_HZ)) == 0) {
            times2 = clock();
            gettimeofday(&tv2, NULL);
            timersub(&tv2, &tv1, &tv3);
            printf("Duree : %lus.%03ldms | times = %7lu | cpu = %3.1f", tv3.tv_sec, tv3.tv_usec/1000, times2 - times1, 100. * (times2 - times1) / (1000000 * tv3.tv_sec + tv3.tv_usec));
            f640_dump_queue(buffers);
            f640_dump_queue(snapped);
            f640_dump_queue(decoded);
            f640_dump_queue(fortagg);
            f640_dump_queue(tagged);
            f640_dump_queue(forrel);
            times1 = clock();
            gettimeofday(&tv1, NULL);
        }
    }

    return 0;
}


/*
 *
 */
extern int f641_get_capabilities(struct f641_v4l2_parameters *prm);
extern int f641_set_input(struct f641_v4l2_parameters *prm);
extern int f641_list_controls(struct f641_v4l2_parameters *prm);
extern int f641_set_pix_format(struct f641_v4l2_parameters *prm);
extern int f641_set_parm(struct f641_v4l2_parameters *prm);
extern int f641_set_mmap(struct f641_v4l2_parameters *prm, int nb_buffers);
extern int f641_set_uptr(struct f641_v4l2_parameters *prm, int nb_buffers);
extern int f641_stream_on(struct f641_v4l2_parameters *prm);
extern int f641_test_dq(struct f641_v4l2_parameters *prm, int nb);
extern int f641_free_mmap(struct f641_v4l2_parameters *prm);

extern struct f641_v4l2_parameters *f641_init_v4l2(struct f641_appli *appli);
extern void f641_free_v4l2(struct f641_v4l2_parameters *prm);

int f641_v4l2() {
    int i, r;
    struct f641_appli           appli;
    struct f641_process_data    proc_data;
    struct f641_v4l2_parameters *v4l2;



//    // Capture
//    if ( prm.capture ) {
//        r = f641_stream_on(&prm);
//        r = f641_test_dq(&prm, 10);
//    }
//
//    // UnMap
//    if ( prm.capture ) {
//        r = f641_free_mmap(&prm);
//    }
//
//    return 0;

    // APP DATA
    appli.width  = 1024;
    appli.height = 576;
    appli.size   = appli.width * appli.height;
    appli.frames_pers = 30;

    snprintf(appli.device, sizeof(appli.device), "/dev/video0");

    appli.recording = 1;
    snprintf(appli.record_path, sizeof(appli.record_path), "/test/work/tust.avi");

    snprintf(appli.stream_path, sizeof(appli.stream_path), "/dev/t030/t030-%d", 1);
    appli.fd_stream = open(appli.stream_path, O_WRONLY);

    snprintf(appli.edge_path, sizeof(appli.edge_path), "/dev/t030/t030-%d", 2);
    appli.fd_edge = open(appli.edge_path, O_WRONLY);

    snprintf(appli.grid_path, sizeof(appli.grid_path), "/dev/t030/t030-%d", 3);
    appli.fd_grid = open(appli.grid_path, O_WRONLY);

    snprintf(appli.get_path, sizeof(appli.get_path), "/dev/t030/t030-%d", 4);
    appli.fd_get = open(appli.get_path, O_RDONLY);

    snprintf(appli.grid10_path, sizeof(appli.grid10_path), "/dev/t030/t030-%d", 5);
    appli.fd_grid10 = open(appli.grid10_path, O_WRONLY);

    snprintf(appli.grid20_path, sizeof(appli.grid20_path), "/dev/t030/t030-%d", 6);
    appli.fd_grid20 = open(appli.grid20_path, O_WRONLY);

    snprintf(appli.grid30_path, sizeof(appli.grid30_path), "/dev/t030/t030-%d", 7);
    appli.fd_grid30 = open(appli.grid30_path, O_WRONLY);

    // PROC DATA
    proc_data.proc_len = 10;
    proc_data.tv0;
    proc_data.decoded_format = PIX_FMT_YUVJ422P;                 // YUV : PIX_FMT_YUYV422, MJPEG : PIX_FMT_YUVJ422P, MPEG : PIX_FMT_YUV420P
    proc_data.broadcast_format = PIX_FMT_BGR24;               // PIX_FMT_BGR24, PIX_FMT_GRAY8
    proc_data.grid = f640_make_grid(appli.width, appli.height, 32);
    proc_data.grid2 = f640_make_grid2(proc_data.grid);
    proc_data.recording_codec = CODEC_ID_MJPEG;       // CODEC_ID_MJPEG, CODEC_ID_RAWVIDEO, CODEC_ID_MPEG1VIDEO, CODEC_ID_MPEG4
    proc_data.record_format = PIX_FMT_YUVJ422P;                  // MJPEG -> PIX_FMT_YUVJ422P, CODEC_ID_MPEG1VIDEO -> PIX_FMT_YUV420P
    proc_data.recorded_frames = 0;
    appli.process = &proc_data;

    // V4L2 PRM
    v4l2 = f641_init_v4l2(&appli);

    // STONES
    struct f640_line *lineup = f640_make_lineup(v4l2->buffers, proc_data.proc_len, proc_data.grid, PIX_FMT_BGR24, NULL, NULL, 350);
    struct f640_stone *stones = calloc(proc_data.proc_len, sizeof(struct f640_stone));
    for(i = 0 ; i < proc_data.proc_len ; i++) {
        f640_make_stone(&stones[i], i);
        stones[i].stones = stones;
        stones[i].private = &lineup[i];
    }

    // QUEUES
    long actbf[2] = {F641_SNAPED, -1};
    struct f640_queue *released = f640_make_queue(stones, proc_data.proc_len, actbf, 0, 0, 0);

    long actsn[2] = {F641_DECODED, -1};
    struct f640_queue *snapped = f640_make_queue(stones, proc_data.proc_len, actsn, 0, 0, 5);

    long actde[4] = {F641_WATCHED, F641_CONVERTED, F641_GRAYED, -1};
    struct f640_queue *decoded = f640_make_queue(stones, proc_data.proc_len, actde, 0, 0, 5);

    long actpr[2] = {F641_GRIDED, -1};
    struct f640_queue *processed = f640_make_queue(stones, proc_data.proc_len, actpr, F641_WATCHED | F641_CONVERTED | F641_GRAYED, 1, 0);

    long actgr[2] = {F641_TAGGED, -1};
    struct f640_queue *grided = f640_make_queue(stones, proc_data.proc_len, actgr, 0, 0, 0);

    long acttg[3] = {F641_BROADCASTED, F641_RECORDED, -1};
    struct f640_queue *tagged  = f640_make_queue(stones, proc_data.proc_len, acttg, 0, 1, 0);

    long actfr[2] = {F641_RELEASED, -1};
    struct f640_queue *broaded  = f640_make_queue(stones, proc_data.proc_len, actfr, F641_BROADCASTED | F641_RECORDED, 0, 0);

    // THREAD
    f641_make_group(1, F641_SNAPED,        released,  1, snapped,   0, 0, NULL, NULL, NULL);
    f641_make_group(4, F641_DECODED,       snapped,   1, decoded,   0, 0, NULL, NULL, NULL);
    f641_make_group(4, F641_WATCHED,       decoded,   1, processed, 0, 2, NULL, NULL, NULL);
    f641_make_group(4, F641_CONVERTED,     decoded,   1, processed, 0, 0, NULL, NULL, NULL);
    f641_make_group(4, F641_GRAYED,        decoded,   1, processed, 0, 0, NULL, NULL, NULL);
    f641_make_group(1, F641_GRIDED,        processed, 1, grided,    0, 1, NULL, NULL, NULL);
    f641_make_group(1, F641_TAGGED,        grided,    1, tagged,    0, 0, NULL, NULL, NULL);
    f641_make_group(1, F641_BROADCASTED,   tagged,    1, broaded,   0, 1, NULL, NULL, NULL);
    f641_make_group(1, F641_RECORDED,      tagged,    1, broaded,   0, 1, NULL, NULL, NULL);

}




/*
 *
 */
int main(int argc, char *argv[]) {

    f641_v4l2();
    //f641_queue_test();

    return 0;
}

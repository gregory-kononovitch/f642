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
#define F641_SKYED          0x00000800


#define F641_CONVERTED      0x00001000
#define F641_GRAYED         0x00002000
#define F641_TAGGED         0x00004000

#define F641_BROADCASTED    0x00010000
#define F641_RECORDED       0x00020000

#define F641_LOGGED         0x40000000
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
/*
 *
 */
static int f640_getopts(struct f641_appli *appli, int argc, char *argv[])
{
    int c;
    int index = 0;
    static struct option long_opts[] =
    {
        {"help",            no_argument,       NULL, 'h'},
        {"quiet",           no_argument,       NULL, 'q'},
        {"verbose",         no_argument,       NULL, 'v'},
        {"version",         no_argument,       NULL, 'i'},
        {"no-capture",      no_argument,       NULL, '0'},
        {"debug",           no_argument,       NULL, 'D'},
        {"file",            no_argument,       NULL, 'a'},
        {"norecord",        no_argument,       NULL, 'r'},
        {"function",        required_argument, NULL, 'o'},
        {"stream",          required_argument, NULL, 's'},
        {"grid",            required_argument, NULL, 'g'},
        {"edge",            required_argument, NULL, 'e'},
        {"grid10",          required_argument, NULL, '1'},
        {"grid20",          required_argument, NULL, '2'},
        {"grid30",          required_argument, NULL, '3'},
        {"get",             required_argument, NULL, 'w'},
        {"buffers",         required_argument, NULL, 'b'},
        {"frames",          required_argument, NULL, 'f'},
        {"fps",             required_argument, NULL, 'z'},
        {"device",          required_argument, NULL, 'd'},
        {"trigger",         required_argument, NULL, 't'},
        {"palette",         required_argument, NULL, 'y'},
        {"width",           required_argument, NULL, 'W'},
        {"height",          required_argument, NULL, 'H'},
        {"bwidth",          required_argument, NULL, 0xff0},
        {"bheight",         required_argument, NULL, 0xff1},
        {"perst",           required_argument, NULL, 0xff2},
        {"path",            required_argument, NULL, 0xff10},
        {"t030",            no_argument,       NULL, 0xff20},
        {"show-all",        no_argument,       NULL, 'A'},
        {"show-caps",       no_argument,       NULL, 'P'},
        {"show-inputs",     no_argument,       NULL, 'I'},
        {"show-controls",   no_argument,       NULL, 'C'},
        {"show-formats",    no_argument,       NULL, 'F'},
        {"show-frame-size", no_argument,       NULL, 'S'},
        {"show-frame-rate", no_argument,       NULL, 'R'},
        {"show-video-std",  no_argument,       NULL, 'V'},
        {NULL, 0, NULL, 0}
    };
    char *opts = "hqvi0Dar:s:g:e:1:2:3:o:b:f:z:d:t:y:W:H:APICFSRV";

    while(1)
    {
        if ( ( c = getopt_long(argc, argv, opts, long_opts, NULL) ) < 0 ) break;
        switch(c)
        {
            case 'h':
                printf("Help\n");
                break;
//            case 'q':
//                quiet = 1;
//                break;
//            case 'v':
//                verbose = 1;
//                break;
//            case 'i':
//                printf("Version 0.1\n");
//                break;
//            case '0':
//                capture = 0;
//                break;
//            case 'D':
//                DEBUG = 1;
//                break;
//            case 'a':
//                v4l2 = 0;
//                recording = 0;
//                break;
            case 'r':
                appli->process->norecord = 1;
                break;
//            case 's':
//                stream_no = atoi(optarg);
//                printf("Stream : %d\n", stream_no);
//                break;
//            case 'g':
//                grid_no = atoi(optarg);
//                printf("Grid : %d\n", grid_no);
//                break;
//            case 'e':
//                edge_no = atoi(optarg);
//                printf("Edge : %d\n", edge_no);
//                break;
//            case 'w':
//                get_no = atoi(optarg);
//                printf("Write : %d\n", get_no);
//                break;
//            case '1':
//                grid10_no = atoi(optarg);
//                printf("Grid 10 : %d\n", grid10_no);
//                break;
//            case '2':
//                grid20_no = atoi(optarg);
//                printf("Grid 20 : %d\n", grid20_no);
//                break;
//            case '3':
//                grid30_no = atoi(optarg);
//                printf("Grid 30 : %d\n", grid30_no);
//                break;
            case 'o':
                appli->functions = atoi(optarg);
                break;
            case 'b':
                appli->process->proc_len = atoi(optarg);
                break;
            case 'f':
                appli->max_frame = atoi(optarg);
                break;
            case 'z':
                appli->frames_pers = atoi(optarg);
                printf("Frame per second : \n", appli->frames_pers);
                break;
            case 'd':
                snprintf(appli->device, sizeof(appli->device), "/dev/video%s", optarg);
                break;
            case 't':
                appli->threshold = atoi(optarg);
                printf("Threshold : %d\n", appli->threshold);
                break;
            case 'y':
                switch(atoi(optarg)) {
                    case 0 : appli->palette = 0x56595559; break;
                    case 1 : appli->palette = 0x47504A4D; break;
                }
                break;
            case 'W':
                appli->width = atoi(optarg);
                break;
            case 'H':
                appli->height = atoi(optarg);
                break;
            case 0xff0:
                appli->broadcast_width = atoi(optarg);
                break;
            case 0xff1:
                appli->broadcast_height = atoi(optarg);
                break;
            case 0xff2:
                appli->recording_perst = atoi(optarg);
                break;
            case 0xff10:
                strcpy(appli->file_path, optarg);
                break;
            case 0xff20:
                appli->process->t030 = 1;
                break;
//            case 'A':
//                show_all = 1;
//                break;
//            case 'P':
//                show_caps = 1;
//                break;
//            case 'I':
//                show_inputs = 1;
//                break;
//            case 'C':
//                show_controls = 1;
//                break;
//            case 'F':
//                show_formats = 1;
//                break;
//            case 'S':
//                show_framsize = 1;
//                break;
//            case 'R':
//                show_framints = 1;
//                break;
//            case 'V':
//                show_vidstds = 1;
//                break;
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
extern int f641_set_defaults(struct f641_v4l2_parameters *prm, int gain, int sharp, int white, int expo);

extern struct f641_v4l2_parameters *f641_init_v4l2(struct f641_appli *appli, uint32_t palette);
extern void f641_free_v4l2(struct f641_v4l2_parameters *prm);

extern void f641_attrib_skying_422(struct f641_thread_operations *ops);
extern void f641_attrib_clouding(struct f641_thread_operations *ops);

int f641_v4l2(int argc, char *argv[]) {
    int i, j, r;
    struct f641_appli           appli;
    struct f641_process_data    proc_data;
    struct f641_v4l2_parameters *v4l2;

    // APP DEFAULT DATA
    memset(&appli, 0, sizeof(struct f641_appli));
    memset(&proc_data, 0, sizeof(struct f641_process_data));

    appli.functions = 1;

    appli.width  = 1024;
    appli.height = 576;
    appli.size   = appli.width * appli.height;
    appli.palette = 0;
    appli.frames_pers = 5;
    appli.recording_rate = 24;
    appli.recording_perst = appli.frames_pers;
    appli.viewing_rate = 24;
    appli.max_frame = 0xFFFFFFFFFFFFFFL;
    appli.threshold = 1024;
    snprintf(appli.device, sizeof(appli.device), "/dev/video0");

    proc_data.proc_len = 16;
    appli.process = &proc_data;

    // Parameters
    f640_getopts(&appli, argc, argv);
    appli.size   = appli.width * appli.height;

    //
    appli.recording = 1;
    snprintf(appli.record_path, sizeof(appli.record_path), "/test/snap/tust.avi");

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

    printf("Appli OK\n");

    // PROC DATA
    proc_data.tv0;
    proc_data.decoded_format = PIX_FMT_YUVJ422P;                 // YUV : PIX_FMT_YUYV422, MJPEG : PIX_FMT_YUVJ422P, MPEG : PIX_FMT_YUV420P
    if (appli.functions == 0 || appli.functions == 10) {
        if (appli.palette == 0x56595559) {
            proc_data.decoded_format = PIX_FMT_YUYV422;
        } else {
            proc_data.decoded_format = PIX_FMT_YUVJ422P;
        }
        if (proc_data.t030) {
            proc_data.broadcast_format = PIX_FMT_BGR24;               // PIX_FMT_BGR24, PIX_FMT_GRAY8, PIX_FMT_BGRA
            proc_data.broadcast_height = appli.height;
            proc_data.broadcast_width  = appli.width;       // 889
            proc_data.screen_height = appli.height;
            proc_data.screen_width  = appli.width;
        } else {
            proc_data.broadcast_format = PIX_FMT_BGRA;               // PIX_FMT_BGR24, PIX_FMT_GRAY8, PIX_FMT_BGRA
            proc_data.broadcast_height = 500;
            proc_data.broadcast_width  = round(1. * appli.width * proc_data.broadcast_height / appli.height);       // 889
            proc_data.screen_height = 500;
            proc_data.screen_width  = 1024;
        }
    } else if (appli.functions == 3) {
        appli.width  = 544;
        appli.height = 288;
        appli.process->decoded_format = PIX_FMT_YUYV422;
//        appli.frames_pers = 5;
//        appli.recording_perst = 5;
        proc_data.broadcast_format = PIX_FMT_GRAY8;               // PIX_FMT_BGR24, PIX_FMT_GRAY8, PIX_FMT_BGRA
        proc_data.broadcast_width  = 544;
        proc_data.broadcast_height = 288;
    } else {
        proc_data.broadcast_format = PIX_FMT_BGR24;
        proc_data.broadcast_width  = appli.width;
        proc_data.broadcast_height = appli.height;
    }
    proc_data.grid  = f640_make_grid(appli.width, appli.height, 32);
    printf("Appli.grid OK\n");
    if (appli.functions != 3) {
        proc_data.grid2 = f640_make_grid2(proc_data.grid);
        printf("Appli.grid2 OK\n");
    }
    if (appli.functions == 2) {
        proc_data.recording_codec = CODEC_ID_RAWVIDEO;       // CODEC_ID_MJPEG, CODEC_ID_RAWVIDEO, CODEC_ID_MPEG1VIDEO, CODEC_ID_MPEG4
        proc_data.record_format = PIX_FMT_GRAY8;                  // MJPEG -> PIX_FMT_YUVJ422P, CODEC_ID_MPEG1VIDEO -> PIX_FMT_YUV420P
    } else {
        proc_data.recording_codec = CODEC_ID_MJPEG;       // CODEC_ID_MJPEG, CODEC_ID_RAWVIDEO, CODEC_ID_MPEG1VIDEO, CODEC_ID_MPEG4
        proc_data.record_format = PIX_FMT_YUVJ422P;                  // MJPEG -> PIX_FMT_YUVJ422P, CODEC_ID_MPEG1VIDEO -> PIX_FMT_YUV420P
    }

    proc_data.recorded_frames = 0;

    printf("Appli.Process OK\n");

    // V4L2 PRM
    if (appli.functions < 10) {
        if (appli.functions == 2 || appli.functions == 3) {
            v4l2 = f641_init_v4l2(&appli, 0x56595559);      //0x47504A4D;   // 0x56595559
        } else if (appli.functions == 1) {
            v4l2 = f641_init_v4l2(&appli, 0x47504A4D);      //0x47504A4D;   // 0x56595559
        } else if (appli.functions == 0){
            if (appli.palette) {
                v4l2 = f641_init_v4l2(&appli, appli.palette);
            } else {
                v4l2 = f641_init_v4l2(&appli, 0x47504A4D);      //0x47504A4D;   // 0x56595559
            }
        } else {
            v4l2 = f641_init_v4l2(&appli, 0x47504A4D);      //0x47504A4D;   // 0x56595559
        }
        appli.width  = v4l2->width;
        appli.height = v4l2->height;
        appli.size   = appli.width * appli.height;
        appli.frames_pers = v4l2->frames_pers;
        printf("V4L2 initialized\n");
    } else if (appli.functions < 20) {

    } else {
        return -1;
    }

    // FFMPEG
    f641_init_ffmpeg(&appli);
    printf("FFMpeg initialized\n");

    // LINES
    struct f640_line *lineup = NULL;
    if (appli.functions < 10) {
        lineup = f640_make_lineup(v4l2->buffers, proc_data.proc_len, proc_data.grid, proc_data.broadcast_format, NULL, NULL, 350);
    } else if (appli.functions < 20) {
        void* buffer = calloc(proc_data.proc_len, 256 * 1024);
        v4l2_buffer_t *buffers = calloc(proc_data.proc_len, sizeof(v4l2_buffer_t));
        for(i = 0 ; i < proc_data.proc_len ; i++) {
            buffers[i].start  = buffer + i * 256 * 1024;
            buffers[i].length = 256 * 1024;
        }
        lineup = f640_make_lineup(buffers, proc_data.proc_len, proc_data.grid, proc_data.broadcast_format, NULL, NULL, 350);
        for(i = 0 ; i < proc_data.proc_len ; i++) {
            lineup[i].buf.index  = i;
            lineup[i].buf.length = 256 * 1024;
        }
    }

    // STONES
    struct f640_stone *stones = calloc(proc_data.proc_len, sizeof(struct f640_stone));
    for(i = 0 ; i < proc_data.proc_len ; i++) {
        f640_make_stone(&stones[i], i);
        stones[i].stones = stones;
        stones[i].private = &lineup[i];
    }
    proc_data.stones = stones;
    printf("Stones %d OK\n", 0);

    // FUNCTION
    struct f640_queue *queues[32];
    struct f640_queue *released  = NULL;
    struct f640_queue *snapped   = NULL;
    struct f640_queue *decoded   = NULL;
    struct f640_queue *processed = NULL;
    struct f640_queue *grided    = NULL;
    struct f640_queue *tagged    = NULL;
    struct f640_queue *broaded   = NULL;
    struct f640_queue *logged    = NULL;


    // QUEUES
    i = 0;
    if (appli.functions == 0) {
        long actbf[2] = {F641_SNAPED, -1};
        released = queues[i++] = f640_make_queue(stones, proc_data.proc_len, actbf, 0, 0, 0);

        long actsn[2] = {F641_DECODED, -1};
        snapped = queues[i++] = f640_make_queue(stones, proc_data.proc_len, actsn, 0, 0, 5);

        long actde[2] = {F641_CONVERTED, -1};
        decoded = queues[i++] = f640_make_queue(stones, proc_data.proc_len, actde, 0, 0, 5);

        long actpr[2] = {F641_TAGGED, -1};
        processed = queues[i++] = f640_make_queue(stones, proc_data.proc_len, actpr, F641_CONVERTED, 1, 0);

        long acttg[3] = {F641_BROADCASTED, F641_RECORDED, -1};
        tagged  = queues[i++] = f640_make_queue(stones, proc_data.proc_len, acttg, 0, 1, 0);

        long actbr[2] = {F641_LOGGED, -1};
        broaded  = queues[i++] = f640_make_queue(stones, proc_data.proc_len, actbr, F641_BROADCASTED | F641_RECORDED, 0, 0);

        long actlg[2] = {F641_RELEASED, -1};
        logged   = queues[i++] = f640_make_queue(stones, proc_data.proc_len, actlg, 0, 0, 0);

        queues[i++] = NULL;
    } else if (appli.functions == 1) {
        long actbf[2] = {F641_SNAPED, -1};
        released = queues[i++] = f640_make_queue(stones, proc_data.proc_len, actbf, 0, 0, 0);

        long actsn[2] = {F641_DECODED, -1};
        snapped = queues[i++] = f640_make_queue(stones, proc_data.proc_len, actsn, 0, 0, 5);

        long actde[4] = {F641_WATCHED, F641_CONVERTED, F641_GRAYED, -1};
        decoded = queues[i++] = f640_make_queue(stones, proc_data.proc_len, actde, 0, 0, 5);

        long actpr[2] = {F641_GRIDED, -1};
        processed = queues[i++] = f640_make_queue(stones, proc_data.proc_len, actpr, F641_WATCHED | F641_CONVERTED | F641_GRAYED, 1, 0);

        long actgr[2] = {F641_TAGGED, -1};
        grided = queues[i++] = f640_make_queue(stones, proc_data.proc_len, actgr, 0, 0, 0);

        long acttg[3] = {F641_BROADCASTED, F641_RECORDED, -1};
        tagged  = queues[i++] = f640_make_queue(stones, proc_data.proc_len, acttg, 0, 1, 0);

        long actbr[2] = {F641_LOGGED, -1};
        broaded  = queues[i++] = f640_make_queue(stones, proc_data.proc_len, actbr, F641_BROADCASTED | F641_RECORDED, 0, 0);

        long actlg[2] = {F641_RELEASED, -1};
        logged   = queues[i++] = f640_make_queue(stones, proc_data.proc_len, actlg, 0, 0, 0);

        queues[i++] = NULL;
    } else if (appli.functions == 2) {
        long actbf[2] = {F641_SNAPED, -1};
        released = queues[i++] = f640_make_queue(stones, proc_data.proc_len, actbf, 0, 0, 0);

        long actsn[2] = {F641_SKYED, -1};
        snapped = queues[i++] = f640_make_queue(stones, proc_data.proc_len, actsn, 0, 0, 0);

        long actpr[2] = {F641_TAGGED, -1};
        processed = queues[i++] = f640_make_queue(stones, proc_data.proc_len, actpr, 0, 1, 0);

        long acttg[3] = {F641_BROADCASTED, F641_RECORDED, -1};
        tagged  = queues[i++] = f640_make_queue(stones, proc_data.proc_len, acttg, 0, 1, 0);

        long actbr[2] = {F641_LOGGED, -1};
        broaded  = queues[i++] = f640_make_queue(stones, proc_data.proc_len, actbr, 0, 0, 0);

        long actlg[2] = {F641_RELEASED, -1};
        logged   = queues[i++] = f640_make_queue(stones, proc_data.proc_len, actlg, 0, 0, 0);

        queues[i++] = NULL;
    } else if (appli.functions == 3) {
        long actbf[2] = {F641_SNAPED, -1};
        released = queues[i++] = f640_make_queue(stones, proc_data.proc_len, actbf, 0, 0, 0);

        long actsn[2] = {F641_CONVERTED, -1};
        snapped = queues[i++] = f640_make_queue(stones, proc_data.proc_len, actsn, 0, 1, 0);

        long actdc[2] = {F641_SKYED, -1};
        decoded = queues[i++] = f640_make_queue(stones, proc_data.proc_len, actdc, 0, 1, 0);

        long acttg[3] = {F641_BROADCASTED, F641_RECORDED, -1};
        processed = queues[i++] = f640_make_queue(stones, proc_data.proc_len, acttg, 0, 1, 0);

        long actbr[2] = {F641_LOGGED, -1};
        broaded  = queues[i++] = f640_make_queue(stones, proc_data.proc_len, actbr, 0, 0, 0);

        long actlg[2] = {F641_RELEASED, -1};
        logged   = queues[i++] = f640_make_queue(stones, proc_data.proc_len, actlg, 0, 0, 0);

        queues[i++] = NULL;
    } else if (appli.functions == 10) {
        long actbf[2] = {F641_SNAPED, -1};
        released = queues[i++] = f640_make_queue(stones, proc_data.proc_len, actbf, 0, 0, 0);

        long actsn[2] = {F641_DECODED, -1};
        snapped = queues[i++] = f640_make_queue(stones, proc_data.proc_len, actsn, 0, 0, 5);

        long actde[4] = {F641_CONVERTED, -1};
        decoded = queues[i++] = f640_make_queue(stones, proc_data.proc_len, actde, 0, 0, 5);

        long actpr[2] = {F641_TAGGED, -1};
        processed = queues[i++] = f640_make_queue(stones, proc_data.proc_len, actpr, F641_CONVERTED, 1, 0);

        long acttg[3] = {F641_BROADCASTED, -1};
        tagged  = queues[i++] = f640_make_queue(stones, proc_data.proc_len, acttg, 0, 1, 0);

        long actbr[2] = {F641_LOGGED, -1};
        broaded  = queues[i++] = f640_make_queue(stones, proc_data.proc_len, actbr, F641_BROADCASTED, 0, 0);

        queues[i++] = NULL;
    }

    printf("Queues OK\n");

    // THREAD
    i = 0;
    struct f640_thread *groups[32];//     = calloc(32, sizeof(struct f640_thread*));
    struct f640_thread *snaping     = NULL;
    struct f640_thread *decoding    = NULL;
    struct f640_thread *watching    = NULL;
    struct f640_thread *converting  = NULL;
    struct f640_thread *graying     = NULL;
    struct f640_thread *griding     = NULL;
    struct f640_thread *tagging     = NULL;
    struct f640_thread *broading    = NULL;
    struct f640_thread *recording   = NULL;
    struct f640_thread *logging     = NULL;
    struct f640_thread *releasing   = NULL;

    if (appli.functions == 0) {
        snaping     = groups[i++]  = f641_make_group(1, F641_SNAPED,        released,  1, snapped,   0, 0, f641_attrib_v4l2_snaping);
        decoding    = groups[i++]  = f641_make_group(3, F641_DECODED,       snapped,   1, decoded,   0, 0, f641_attrib_decoding_mjpeg);
        converting  = groups[i++]  = f641_make_group(3, F641_CONVERTED,     decoded,   1, processed, 0, 0, f641_attrib_converting_torgb);
        tagging     = groups[i++]  = f641_make_group(1, F641_TAGGED,        processed, 1, tagged,    0, 0, f641_attrib_tagging);
        broading    = groups[i++]  = f641_make_group(1, F641_BROADCASTED,   tagged,    1, broaded,   0, 1, f641_attrib_broadcasting);
        recording   = groups[i++]  = f641_make_group(1, F641_RECORDED,      tagged,    1, broaded,   0, 1, f641_attrib_recording);
        logging     = groups[i++]  = f641_make_group(1, F641_LOGGED,        broaded,   1, logged,    0, 1, f641_attrib_logging);
        releasing   = groups[i++]  = f641_make_group(1, F641_RELEASED,      logged,    1, released,  0, 0, f641_attrib_v4l2_desnaping);
        groups[i++] = NULL;
    } else if (appli.functions == 1) {
        snaping     = groups[i++]  = f641_make_group(1, F641_SNAPED,        released,  1, snapped,   0, 0, f641_attrib_v4l2_snaping);
        decoding    = groups[i++]  = f641_make_group(3, F641_DECODED,       snapped,   1, decoded,   0, 0, f641_attrib_decoding_mjpeg);
        watching    = groups[i++]  = f641_make_group(3, F641_WATCHED,       decoded,   1, processed, 0, 2, f641_attrib_watching_422p);
        converting  = groups[i++]  = f641_make_group(3, F641_CONVERTED,     decoded,   1, processed, 0, 0, f641_attrib_converting_torgb);
        graying     = groups[i++]  = f641_make_group(3, F641_GRAYED,        decoded,   1, processed, 0, 0, f641_attrib_edging);
        griding     = groups[i++]  = f641_make_group(1, F641_GRIDED,        processed, 1, grided,    0, 1, f641_attrib_griding);
        tagging     = groups[i++]  = f641_make_group(1, F641_TAGGED,        grided,    1, tagged,    0, 0, f641_attrib_tagging);
        broading    = groups[i++]  = f641_make_group(1, F641_BROADCASTED,   tagged,    1, broaded,   0, 1, f641_attrib_broadcasting);
        recording   = groups[i++]  = f641_make_group(1, F641_RECORDED,      tagged,    1, broaded,   0, 1, f641_attrib_recording);
        logging     = groups[i++]  = f641_make_group(1, F641_LOGGED,        broaded,   1, logged,    0, 1, f641_attrib_logging);
        releasing   = groups[i++]  = f641_make_group(1, F641_RELEASED,      logged,    1, released,  0, 0, f641_attrib_v4l2_desnaping);
        groups[i++] = NULL;
    } else if (appli.functions == 2) {
        snaping     = groups[i++]  = f641_make_group(1, F641_SNAPED,        released,  1, snapped,   0, 0, f641_attrib_v4l2_snaping);
        converting  = groups[i++]  = f641_make_group(1, F641_SKYED,         snapped,   1, processed, 0, 0, f641_attrib_skying_422);
        tagging     = groups[i++]  = f641_make_group(1, F641_TAGGED,        processed, 1, tagged,    0, 0, f641_attrib_tagging);
        broading    = groups[i++]  = f641_make_group(1, F641_BROADCASTED,   tagged,    1, broaded,   0, 1, f641_attrib_broadcasting);
        recording   = groups[i++]  = f641_make_group(1, F641_RECORDED,      tagged,    1, NULL,      0, 1, f641_attrib_saving);
        logging     = groups[i++]  = f641_make_group(1, F641_LOGGED,        broaded,   1, logged,    0, 1, f641_attrib_logging);
        releasing   = groups[i++]  = f641_make_group(1, F641_RELEASED,      logged,    1, released,  0, 0, f641_attrib_v4l2_desnaping);
        groups[i++] = NULL;
    } else if (appli.functions == 3) {
        snaping     = groups[i++]  = f641_make_group(1, F641_SNAPED,        released,  1, snapped,   0, 0, f641_attrib_v4l2_snaping);
        converting  = groups[i++]  = f641_make_group(1, F641_CONVERTED,     snapped,   1, decoded,   0, 1, f641_attrib_converting_torgb);
        graying     = groups[i++]  = f641_make_group(1, F641_SKYED,         decoded,   1, processed, 0, 1, f641_attrib_clouding);
        broading    = groups[i++]  = f641_make_group(1, F641_BROADCASTED,   processed, 1, broaded,   0, 1, f641_attrib_broadcasting);
        recording   = groups[i++]  = f641_make_group(1, F641_RECORDED,      processed, 1, NULL,      0, 1, f641_attrib_saving);
        logging     = groups[i++]  = f641_make_group(1, F641_LOGGED,        broaded,   1, logged,    0, 1, f641_attrib_logging);
        releasing   = groups[i++]  = f641_make_group(1, F641_RELEASED,      logged,    1, released,  0, 0, f641_attrib_v4l2_desnaping);
        groups[i++] = NULL;
    } else if (appli.functions == 10) {
        snaping     = groups[i++]  = f641_make_group(1, F641_SNAPED,        released,  1, snapped,   0, 0, f641_attrib_reading_mjpeg);
        decoding    = groups[i++]  = f641_make_group(3, F641_DECODED,       snapped,   1, decoded,   0, 0, f641_attrib_decoding_mjpeg);
        converting  = groups[i++]  = f641_make_group(3, F641_CONVERTED,     decoded,   1, processed, 0, 0, f641_attrib_converting_torgb);
        tagging     = groups[i++]  = f641_make_group(1, F641_TAGGED,        processed, 1, tagged,    0, 0, f641_attrib_tagging);
        broading    = groups[i++]  = f641_make_group(1, F641_BROADCASTED,   tagged,    1, broaded,   0, 1, f641_attrib_broadcasting);
        logging     = groups[i++]  = f641_make_group(1, F641_LOGGED,        broaded,   1, released,  0, 1, f641_attrib_logging);
        groups[i++] = NULL;
    }
    printf("Thread OK\n");

    // INIT RESSOURCES
    i = 0;
    while(groups[i] != NULL) {
        for(j = 0 ; j < groups[i]->group_size ; j++) {
            printf("Init RES of thread %d - %d = \"%s\"\n", i, j, groups[i]->group[j].name);
            r = f641_init_thread(&groups[i]->group[j], &appli);
        }
        i++;
    }
    printf("Init ressources done\n");

    // FILL FIRST QUEUE
    usleep(100*1000);
    for(i = 0 ; i < proc_data.proc_len ; i++) {
        lineup[i].buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        lineup[i].buf.memory = V4L2_MEMORY_MMAP;
        lineup[i].buf.index  = i;

        f640_enqueue(logged, 0, i, F641_RELEASED);
    }
    printf("%d Buffers ready\n", proc_data.proc_len);
    usleep(300*1000);

    // STREAM ON
    if (appli.functions < 10) {
        f641_stream_on(v4l2);
        printf("Stream ON\n");
    } else if (appli.functions < 20) {

    }

    // LAUNCH
    gettimeofday(&appli.process->tv0, NULL);
    i = 0;
    while(groups[i] != NULL) {
        for(j = 0 ; j < groups[i]->group_size ; j++) {
            pthread_create(&groups[i]->group[j].id, NULL, f641_loop2,   (void *)(&groups[i]->group[j]));
        }
        i++;
    }
    printf("Launch done\n");

    if (appli.functions < 10 && appli.functions != 2 && appli.functions != 3) {
        //
        appli.logging = 0;
        initscr();                      /* Start curses mode            */
        scrl(0);
        raw();                          /* Line buffering disabled      */
        keypad(stdscr, TRUE);           /* We get F1, F2 etc..          */
        noecho();                       /* Don't echo() while we do getch */
        start_color();                  /* Start color                  */
        init_pair(1, COLOR_BLACK, COLOR_GREEN);
        init_pair(2, COLOR_BLACK, COLOR_RED);

        if (appli.functions == 2) {
            usleep(2000*1000);
            f641_set_defaults(v4l2, 119, 22, 4000, 2047);
        }

        int rows, cols;
        getmaxyx(stdscr, rows, cols);     /* get the number of rows and columns */
        int ctrl = 0;
        int selected = 0;
        int32_t value = 0;

        while(1) {
            int ch = getch();

            if (ch == KEY_CANCEL || ch == KEY_F(1)) {
                endwin();

                snaping->loop = 0;
                usleep(2000 / appli.frames_pers);

                i = 0;
                while(queues[i] != NULL) {
                    queues[i]->run = 0;

                    pthread_mutex_lock(&queues[i]->mutex);
                    pthread_cond_broadcast(&queues[i]->cond);
                    pthread_mutex_unlock(&queues[i]->mutex);

                    i++;
                }
                usleep(300*1000);
                printf("\n");
                return 0;
            } else if (ch == '\n') {
                if (selected && value != v4l2->controls_value[ctrl]) {
                    struct v4l2_control control;
                    control.id = v4l2->controls[ctrl].id;
                    control.value = value;
                    ioctl(v4l2->fd, VIDIOC_S_CTRL, &control);
                    control.id = v4l2->controls[ctrl].id;
                    control.value = 0;
                    if ( ioctl(v4l2->fd, VIDIOC_G_CTRL, &control) > -1 ) {
                        v4l2->controls_value[ctrl] = control.value;
                        value = control.value;
                    }
                    selected = 0;
                } else if (selected) {
                    selected = 0;
                } else {
                    selected = 1;
                    value = v4l2->controls_value[ctrl];
                }
            } else if (ch == KEY_LEFT && selected) {
                value -= v4l2->controls[ctrl].step;
                if (value < v4l2->controls[ctrl].minimum) value = v4l2->controls[ctrl].minimum;
            } else if (ch == KEY_RIGHT && selected) {
                value += v4l2->controls[ctrl].step;
                if (value > v4l2->controls[ctrl].maximum) value = v4l2->controls[ctrl].maximum;
            } else if (ch == KEY_PPAGE && selected) {
                if (v4l2->controls[ctrl].step > 0) {
                    int tmp = (v4l2->controls[ctrl].maximum - v4l2->controls[ctrl].minimum) / (12 * v4l2->controls[ctrl].step);
                    if (tmp == 0) tmp = v4l2->controls[ctrl].step;
                    value -= tmp;
                    if (value < v4l2->controls[ctrl].minimum) value = v4l2->controls[ctrl].minimum;
                }
            } else if (ch == KEY_NPAGE && selected) {
                if (v4l2->controls[ctrl].step > 0) {
                    int tmp = (v4l2->controls[ctrl].maximum - v4l2->controls[ctrl].minimum) / (12 * v4l2->controls[ctrl].step);
                    if (tmp == 0) tmp = v4l2->controls[ctrl].step;
                    value += tmp;
                    if (value > v4l2->controls[ctrl].maximum) value = v4l2->controls[ctrl].maximum;
                }
            } else if (ch == KEY_UP) {
                selected = 0;
                if (ctrl) ctrl--;
                else ctrl = v4l2->nb_controls - 1;
                value = v4l2->controls_value[ctrl];
            } else if (ch == KEY_DOWN) {
                selected = 0;
                ctrl = (ctrl + 1) % v4l2->nb_controls;
                value = v4l2->controls_value[ctrl];
            } else if (ch == '-') {
                if (selected) {
                    value = v4l2->controls_value[ctrl] - v4l2->controls[ctrl].step;
                    if (value < v4l2->controls[ctrl].minimum) value = v4l2->controls[ctrl].minimum;
                    if (selected && value != v4l2->controls_value[ctrl]) {
                        struct v4l2_control control;
                        control.id = v4l2->controls[ctrl].id;
                        control.value = value;
                        ioctl(v4l2->fd, VIDIOC_S_CTRL, &control);
                        control.id = v4l2->controls[ctrl].id;
                        control.value = 0;
                        if ( ioctl(v4l2->fd, VIDIOC_G_CTRL, &control) > -1 ) {
                            v4l2->controls_value[ctrl] = control.value;
                            value = control.value;
                        }
                    }
                } else {
                    appli.recording_perst++;
                }
            } else if (ch == '+') {
                if (selected) {
                    value = v4l2->controls_value[ctrl] + v4l2->controls[ctrl].step;
                    if (value > v4l2->controls[ctrl].maximum) value = v4l2->controls[ctrl].maximum;
                    if (selected && value != v4l2->controls_value[ctrl]) {
                        struct v4l2_control control;
                        control.id = v4l2->controls[ctrl].id;
                        control.value = value;
                        ioctl(v4l2->fd, VIDIOC_S_CTRL, &control);
                        control.id = v4l2->controls[ctrl].id;
                        control.value = 0;
                        if ( ioctl(v4l2->fd, VIDIOC_G_CTRL, &control) > -1 ) {
                            v4l2->controls_value[ctrl] = control.value;
                            value = control.value;
                        }
                    }
                } else {
                    if (appli.recording_perst > 1) appli.recording_perst--;
                    else appli.recording_perst = 1;
                }
            } else if (ch == 'i') {     // Iso
                proc_data.showIsos = proc_data.showIsos ? 0 : 1;
            } else if (ch == 'm') {     // Mire
                proc_data.showMire = proc_data.showMire ? 0 : 1;
            } else if (ch == 'a') {     // Angles
                proc_data.showAngles = proc_data.showAngles ? 0 : 1;
            } else if (ch == 'r') {     // Rules
                proc_data.showRules = proc_data.showRules ? 0 : 1;
            } else if (ch == 'c') {     // Colors
                proc_data.showColor = (proc_data.showColor + 1) % 5;
            } else if (ch == KEY_F(5)) {     // Record
                proc_data.norecord = proc_data.norecord ? 0 : 1;
            } else if (ch == 'p') {     // Photo
                proc_data.flag_photo = 1;
            } else {
                move(rows - 1, 2);
                printw("%c typed", ch);
            }

            // RAZ
            getmaxyx(stdscr, rows, cols);     /* get the number of rows and columns */
            for(i = rows - 3 ; i < rows ; i++) {
                for(j = 0 ; j < cols ; j++) {
                    move(i, j);
                    printw(" ");
                }
            }


            move(rows - 2, 0);
            if (selected) {
                if (value == v4l2->controls_value[ctrl]) attron(COLOR_PAIR(1));
                else attron(COLOR_PAIR(2));
            }
            struct v4l2_querymenu querymenu;
            switch(v4l2->controls[ctrl].type) {
                case V4L2_CTRL_TYPE_INTEGER:
                    printw("%-32s = %d (%d%) : %d < %d  +%d", v4l2->controls[ctrl].name, value, 100 * (value - v4l2->controls[ctrl].minimum) / (v4l2->controls[ctrl].maximum - v4l2->controls[ctrl].minimum)
                            , v4l2->controls[ctrl].minimum, v4l2->controls[ctrl].maximum, v4l2->controls[ctrl].step);
                    break;
                case V4L2_CTRL_TYPE_BOOLEAN:
                    printw("%-32s : %s", v4l2->controls[ctrl].name, value ? "true" : "false");
                    break;
                case V4L2_CTRL_TYPE_MENU:
                    memset(&querymenu, 0, sizeof(struct v4l2_querymenu));
                    querymenu.id    = v4l2->controls[ctrl].id;
                    querymenu.index = value;
                    if(ioctl(v4l2->fd, VIDIOC_QUERYMENU, &querymenu)) {
                        printw("%-32s : error", v4l2->controls[ctrl].name);
                    } else {
                        printw("%-32s : %s (%d < %d +%d)", v4l2->controls[ctrl].name, querymenu.name
                                , v4l2->controls[ctrl].minimum, v4l2->controls[ctrl].maximum, v4l2->controls[ctrl].step);
                    }
                    break;
                default:
                    printw("%-32s : ??", v4l2->controls[ctrl].name);
                    break;
            }
            if (selected) {
                if (value == v4l2->controls_value[ctrl]) attroff(COLOR_PAIR(1));
                else attroff(COLOR_PAIR(2));
            }


            move(rows - 1, 20);
            printw("rcs %d", appli.recording_perst);
            refresh();

        }
        endwin();                       /* End curses mode                */
    } else if (appli.functions < 20 && appli.functions != 2  && appli.functions != 3) {
        //
        appli.logging = 0;
        initscr();                      /* Start curses mode            */
        scrl(0);
        raw();                          /* Line buffering disabled      */
        keypad(stdscr, TRUE);           /* We get F1, F2 etc..          */
        noecho();                       /* Don't echo() while we do getch */
        start_color();                  /* Start color                  */
        init_pair(1, COLOR_BLACK, COLOR_GREEN);
        init_pair(2, COLOR_BLACK, COLOR_RED);

        int rows, cols;
        getmaxyx(stdscr, rows, cols);     /* get the number of rows and columns */
        int tmp = INT32_MAX;
        int ctrl = 0;
        int selected = 0;
        int32_t value = 0;

        while(1) {
            int ch = getch();

            if (ch == KEY_CANCEL || ch == KEY_F(1)) {
                endwin();

                snaping->loop = 0;
                usleep(2000 / appli.frames_pers);

                i = 0;
                while(queues[i] != NULL) {
                    queues[i]->run = 0;

                    pthread_mutex_lock(&queues[i]->mutex);
                    pthread_cond_broadcast(&queues[i]->cond);
                    pthread_mutex_unlock(&queues[i]->mutex);

                    i++;
                }
                usleep(300*1000);
                printf("\n");
                return 0;
            } else if (ch == '\n') {
            } else if (ch == KEY_LEFT && selected) {
            } else if (ch == KEY_RIGHT && selected) {
            } else if (ch == KEY_UP) {
            } else if (ch == KEY_DOWN) {
            } else if (ch == '-') {
                appli.viewing_rate--;
            } else if (ch == '+') {
                appli.viewing_rate++;
            } else if (ch == 'p') {
                if (tmp == INT32_MAX) {
                    tmp = appli.viewing_rate;
                    appli.viewing_rate = 0;
                } else {
                    appli.viewing_rate = tmp;
                    tmp = INT32_MAX;
                }
            } else {
                move(rows - 1, 2);
                printw("%c typed", ch);
            }

            // RAZ
            getmaxyx(stdscr, rows, cols);     /* get the number of rows and columns */
            for(i = rows - 3 ; i < rows ; i++) {
                for(j = 0 ; j < cols ; j++) {
                    move(i, j);
                    printw(" ");
                }
            }

            //
            move(rows - 1, 20);
            printw("fps %d", appli.viewing_rate);
            refresh();

        }
        endwin();                       /* End curses mode                */
    } else {
        //
        void *grid = calloc(1, sizeof(long) * (6 + appli.process->grid->cols * appli.process->grid->rows));
        while(1) {
            r = read(appli.fd_get, grid, sizeof(long) * (6 + appli.process->grid->cols * appli.process->grid->rows) );
            if (r == sizeof(long) * (6 + appli.process->grid->cols * appli.process->grid->rows)) {
                if (*((int*)(grid+20)) == 999999) {
                    appli.level = *((int*)(grid+24));
                    printf("\nREAD : Level set to %d", appli.level);
                } else {
                    printf("READ  %dbytes : %dx%d\n", r, *((int*)(grid+16)), *((int*)(grid+20)));
                    pthread_mutex_lock(&appli.process->grid->mutex_coefs);
                    memcpy(appli.process->grid->grid_coefs, grid + 6 * sizeof(long), r - 6 * sizeof(long));
                    pthread_mutex_unlock(&appli.process->grid->mutex_coefs);
                }
            } else if (r > 0) {
              printf("READ: wrong size read %d / %dx%d, discarding\n", r, appli.process->grid->cols, appli.process->grid->rows);
            } else if (r) {
//                printf("\nREAD: Interrupt %d", r);
                usleep(1000*1000);  // @@@ bug
            }
        }
    }
}


void f641_pi(int len, int to_be_saved) {
    int i = 0;
    int prec;
    int *d2 = calloc(len, sizeof(int));
    int sigm = -1, higm = 0;
    int sigd =  1;
    int siga = -1;
    int *d3 = calloc(len, sizeof(int));
    int step = 1;
    int dist[10];
    struct timeval tv00, tv0, tv1;
    uint8_t *save = calloc(4 + 2 * len, sizeof(uint8_t));

    //
    d2[0] = 0;
    d3[0] = 2;
    for(i = 1 ; i < len ; i++) {
        d2[i] = 6;
        d3[i] = 6;
    }

    //
    gettimeofday(&tv0, NULL);
    int stepm = 333 * len / 100;
    int len1 = len - 1;
    int den = 3;
    int r = 0;
    int s = 0;
    int div = 0;
    int fl = 2;

    gettimeofday(&tv00, NULL);
    for(step = 2 ; step < stepm ; step++) {
        den += 2;
        r = 0;
        s = 0;  // < 10 * step

        // Multiply
        for(i = len - 1 ; i > sigm ; i--) {
            s = r + step * d2[i];
            if (s > 9) {
                r = s / 10;
                d2[i] = s % 10;
            } else {
                r = 0;
                d2[i] = s;
            }
        }

        // Divide
        div = d2[sigd - 1];
        for(i = sigd ; i < len ; i++) {
            if (div >= den) {
                d2[i-1] = div / den;
                div = 10 * (div % den) + d2[i];
            } else {
                d2[i-1] = 0;
                div = 10 * div + d2[i];
            }
        }
        if (div >= den) d2[len1] = div / den;
        else d2[len1] = 0;

        // Add
        r = 0;
        for(i = len - 1 ; i > siga ; i--) {
            s = r + d2[i] + d3[i];
            if (s < 10) {
                r = 0;
                d3[i] = s;
            } else {
                r = 1;
                d3[i] = s - 10;
            }
        }


        if (step > 12 && step % 10 == 0) {
            sigm += 3;
            sigd += 3;
            siga += 3;
            fl = floor(log10(step)) + 1;

            if (step % 1500 == 0) {
                int sig = 0;
                gettimeofday(&tv1, NULL);
                timersub(&tv1, &tv0, &tv0);

                for(sig = 0 ; sig < len ; sig++) {
                    if (d2[sig] != 0) break;
                }

                if (to_be_saved) {
                    printf("Last %lu.%06ld (%f) : %d.%d%d%d", tv0.tv_sec, tv0.tv_usec, 3500000. / 1500. * (tv0.tv_sec + tv0.tv_usec/1000000.) / 3600., d3[0], d3[1], d3[2], d3[3]);
    //                for(i = 4 ; i < 25 ; i++) printf("%d", d3[i]);
                    printf(" - sig = %d / %d | ", sigm, sig);
                }

                sigm = sig - log10(step) - 3;
                sigd = sigm + 2;
                siga = sigm;

                if (to_be_saved) {
                    memset(dist, 0, 40);
                    for(i = higm ; i < sigm ; i++) dist[d3[i]]++;
                    printf("Dist : ");
                    for(i = 0 ; i < 10 ; i++) printf("%d: %02.1f|", i, 1. * (sigm - higm) / (dist[i] + 0.001));
                    printf("\n");

                    *((int*)save) = step;
                    for(i = 0 ; i < len ; i++) {
                        save[4 + i] = d2[i];
                    }
                    for(i = 0 ; i < len ; i++) {
                        save[4 + len + i] = d3[i];
                    }
                    FILE *filp;
                    filp = fopen("pi1.dat", "wb");
                    fwrite(save, 4 + 2 * len, 1, filp);
                    fflush(filp);
                    fclose(filp);
                }

                higm = sigm;
                gettimeofday(&tv0, NULL);
            }
        }
    }
    gettimeofday(&tv1, NULL);
    timersub(&tv1, &tv00, &tv00);

    printf("Last %lu.%06ld : %d.", tv00.tv_sec, tv00.tv_usec, d3[0]);
    for(i = 1 ; i < 50 ; i++) printf("%d", d3[i]);
    printf(" - ");
    for(i = len - 50 ; i < len ; i++) printf("%d", d3[i]);
    printf("\n");

    *((int*)save) = step;
    for(i = 0 ; i < len ; i++) {
        save[4 + i] = d2[i];
    }
    for(i = 0 ; i < len ; i++) {
        save[4 + len + i] = d3[i];
    }
    FILE *filp;
    filp = fopen("pi1.dat", "wb");
    fwrite(save, 4 + 2 * len, 1, filp);
    fflush(filp);
    fclose(filp);

    free(d2);
    free(d3);
    free(save);
}

void f641_pi_(int len, int to_be_saved) {
    int i = 0;
    int prec;
    int *d2 = calloc(len, sizeof(int));
    int sigm = 0, higm = 0;
    int sigd = 0;
    int siga = 0;
    int *d3 = calloc(len, sizeof(int));
    int step = 1;
    int dist[10];
    struct timeval tv00, tv0, tv1;
    uint8_t *save = calloc(4 + 2 * len, sizeof(uint8_t));

    //
    d2[0] = 0;
    d2[1] = 0;
    d2[2] = 2;
    d3[0] = 0;
    d3[1] = 2;
    d3[2] = 9;
    for(i = 3 ; i < len ; i++) {
        d2[i] = 6;
        d3[i] = 3;
    }

    //
    gettimeofday(&tv0, NULL);
    int len1 = len - 1;
    int den = 5;
    int r = 0;
    int s = 0;
    int div = 0;
    int fl = 3;

    gettimeofday(&tv00, NULL);
    for(step = 3 ; step < INT_MAX ; step++) {
        den += 2;
        r = 0;
        s = 0;  // < 10 * step

        // Multiply
        while(sigm < len1 && d2[sigm] == 0) sigm++;
        if (sigm == len1) break;
        sigm -= fl;

        for(i = len - 1 ; i > sigm ; i--) {
            s = r + step * d2[i];
            if (s > 9) {
                r = s / 10;
                d2[i] = s % 10;
            } else {
                r = 0;
                d2[i] = s;
            }
        }

        // Divide
        div = d2[sigm++];
        for(i = sigm ; i < len ; i++) {
            if (div >= den) {
                d2[i-1] = div / den;
                div = 10 * (div % den) + d2[i];
            } else {
                d2[i-1] = 0;
                div = 10 * div + d2[i];
            }
        }
        if (div >= den) d2[len1] = div / den;
        else d2[len1] = 0;

        // Add
        r = 0;
        for(i = len - 1 ; i > sigm ; i--) {
            s = r + d2[i] + d3[i];
            if (s < 10) {
                r = 0;
                d3[i] = s;
            } else {
                r = 1;
                d3[i] = s - 10;
            }
        }


        if (step > 12 && step % 100 == 0) {
            fl = floor(log10(step)) + 3;

            if (step % 1500 == 0) {
                gettimeofday(&tv1, NULL);
                timersub(&tv1, &tv0, &tv0);

                if (to_be_saved) {
                    printf("Last %lu.%06ld (%f) : %d.%d%d%d", tv0.tv_sec, tv0.tv_usec, 3500000. / 1500. * (tv0.tv_sec + tv0.tv_usec/1000000.) / 3600., d3[0], d3[1], d3[2], d3[3]);
    //                for(i = 4 ; i < 25 ; i++) printf("%d", d3[i]);
                    printf(" - sig = %d | ", sigm);
                }

                if (to_be_saved) {
//                    memset(dist, 0, 40);
//                    for(i = higm ; i < sigm ; i++) dist[d3[i]]++;
//                    printf("Dist : ");
//                    for(i = 0 ; i < 10 ; i++) printf("%d: %02.1f|", i, 1. * (sigm - higm) / (dist[i] + 0.001));
                    printf("\n");

                    *((int*)save) = step;
                    for(i = 0 ; i < len ; i++) {
                        save[4 + i] = d2[i];
                    }
                    for(i = 0 ; i < len ; i++) {
                        save[4 + len + i] = d3[i];
                    }
                    FILE *filp;
                    filp = fopen("pi1.dat", "wb");
                    fwrite(save, 4 + 2 * len, 1, filp);
                    fflush(filp);
                    fclose(filp);
                }
                gettimeofday(&tv0, NULL);
            }
        }
    }
    gettimeofday(&tv1, NULL);
    timersub(&tv1, &tv00, &tv00);

    printf("Last %lu.%06ld for %d iter : %d.", tv00.tv_sec, tv00.tv_usec, step, d3[1]);
    for(i = 2 ; i < 50 ; i++) printf("%d", d3[i]);
    printf(" - ");
    for(i = len - 50 ; i < len ; i++) printf("%d", d3[i]);
    printf("\n");

    *((int*)save) = step;
    for(i = 0 ; i < len ; i++) {
        save[4 + i] = d2[i];
    }
    for(i = 0 ; i < len ; i++) {
        save[4 + len + i] = d3[i];
    }
    FILE *filp;
    filp = fopen("pi1.dat", "wb");
    fwrite(save, 4 + 2 * len, 1, filp);
    fflush(filp);
    fclose(filp);

    // Compare
    free(save);
    filp = fopen("/home/greg/t509/t508-java/pi1000k.dat", "rb");
    save = calloc(4 + 2 * 1024 * 1024, sizeof(uint8_t));
    fread(save, 4 + 2 * 1024*1024, 1, filp);
    fclose(filp);

    for(i = 0 ; i < len ; i++) {
        if (d3[i+1] != save[4 + 1024*1024 + i]) break;
    }
    printf("Compare : %d sigs\n", i);

    free(d2);
    free(d3);
    free(save);
}

void f641_e1(int len, int to_be_saved) {
    int i = 0;
    int prec;
    int *d2 = calloc(len, sizeof(int));
    int sigd =  0;
    int siga = -1;
    int *d3 = calloc(len, sizeof(int));
    int step = 1;
    int dist[10];
    struct timeval tv00, tv0, tv1;
    uint8_t *save = calloc(4 + 2 * len, sizeof(uint8_t));

    //
    d2[0] = 0;
    d2[1] = 5;
    d3[0] = 2;
    d3[1] = 5;
    for(i = 2 ; i < len ; i++) {
        d2[i] = 0;
        d3[i] = 0;
    }

    //
    gettimeofday(&tv0, NULL);
    int stepm = 333 * len / 100;
    int len1 = len - 1;
    int r = 0;
    int s = 0;
    int div = 0;

    gettimeofday(&tv00, NULL);
    for(step = 3 ; step < 100000000 ; step++) {
        r = 0;
        s = 0;

        // Divide
        while(sigd < len && d2[sigd] == 0) sigd++;
        if (sigd == len) break;
        div = d2[sigd];
        for(i = sigd+1 ; i < len ; i++) {
            if (div >= step) {
                d2[i-1] = div / step;
                div = 10 * (div % step) + d2[i];
            } else {
                d2[i-1] = 0;
                div = 10 * div + d2[i];
            }
        }
        if (div >= step) d2[len1] = div / step;
        else d2[len1] = 0;

        // Add
        r = 0;
        sigd--;
        for(i = len - 1 ; i > sigd ; i--) {
            s = r + d2[i] + d3[i];
            if (s < 10) {
                r = 0;
                d3[i] = s;
            } else {
                r = 1;
                d3[i] = s - 10;
            }
        }

        if ( sigd % 500 == 0 ) {
            gettimeofday(&tv1, NULL);
            timersub(&tv1, &tv0, &tv0);

            printf("Last %lu.%06ld for %d : e = %d.", tv0.tv_sec, tv0.tv_usec, step, d3[0]);
            for(i = 1 ; i < 50 ; i++) printf("%d", d3[i]);
            printf(" - ");
            for(i = len - 50 ; i < len ; i++) printf("%d", d3[i]);
            printf("\n");
            gettimeofday(&tv0, NULL);
        }
    }
    gettimeofday(&tv1, NULL);
    timersub(&tv1, &tv00, &tv00);

    printf("Last %lu.%06ld for %d : e = %d.", tv00.tv_sec, tv00.tv_usec, step, d3[0]);
    for(i = 1 ; i < 50 ; i++) printf("%d", d3[i]);
    printf(" - ");
    for(i = len - 50 ; i < len ; i++) printf("%d", d3[i]);
    printf("\n");

    *((int*)save) = step;
    for(i = 0 ; i < len ; i++) {
        save[4 + i] = d2[i];
    }
    for(i = 0 ; i < len ; i++) {
        save[4 + len + i] = d3[i];
    }
    FILE *filp;
    filp = fopen("e1.dat", "wb");
    fwrite(save, 4 + 2 * len, 1, filp);
    fflush(filp);
    fclose(filp);

    free(d2);
    free(d3);
    free(save);
}

void f641_pi4(int len, int to_be_saved) {
    int i = 0;
    int prec = len + floor(log10(len)) + 2;
    int sigm = floor(log10(len)) + 2;
    int sigd =  floor(log10(len)) + 3;
    int *d3 = calloc(prec, sizeof(int));
    int step = 333 * (len+1) / 100;
    int dist[10];
    struct timeval tv00, tv0, tv1;
    uint8_t *save = calloc(4 + len, sizeof(uint8_t));

    len++;
    //
    d3[0] = 2;
    d3[1] = 5;
    for(i = 2 ; i < prec ; i++) {
        d3[i] = 0;
    }

    //
    gettimeofday(&tv0, NULL);
    int den = 2 * step + 3;
    int r = 0;
    int s = 0;
    int div = 0;
    int it = 0;
    gettimeofday(&tv00, NULL);
    for(; step > 0 ; step--) {

        // Divide
        den -= 2;
        div  = d3[0];
        for(i = 1 ; i < sigd ; i++) {
            if (div >= den) {
                d3[i-1] = div / den;
                div = 10 * (div % den) + d3[i];
            } else {
                d3[i-1] = 0;
                div = 10 * div + d3[i];
            }
        }

        // Multiply
        r = 0;
        s = 0;
        for(i = sigm ; i > -1 ; i--) {
            s = r + step * d3[i];
            if (s > 9) {
                r = s / 10;
                d3[i] = s % 10;
            } else {
                r = 0;
                d3[i] = s;
            }
        }

        //
        d3[0] += 2;

        it++;
        if (it % 10 == 0) {
            sigd += 3;
            sigm += 3;
            if (sigd > prec) sigd = prec;
            if (sigm >= prec) sigm = prec - 1;

            gettimeofday(&tv1, NULL);
            timersub(&tv1, &tv0, &tv1);
            if (to_be_saved && tv1.tv_sec > 60) {
                printf("it = %d, prec = %d\n", it, sigd);
                gettimeofday(&tv0, NULL);

                *((int*)save) = it;
                for(i = 0 ; i < prec ; i++) {
                    save[4 + i] = d3[i];
                }
                FILE *filp;
                filp = fopen("pi4.dat", "wb");
                fwrite(save, 4 + prec, 1, filp);
                fflush(filp);
                fclose(filp);
            }
        }
    }
    gettimeofday(&tv1, NULL);
    timersub(&tv1, &tv00, &tv00);

    printf("Last %lu.%06ld : %d.", tv00.tv_sec, tv00.tv_usec, d3[0]);
    for(i = 1 ; i < 50 ; i++) printf("%d", d3[i]);
    printf(" - ");
    for(i = len - 50 ; i < len ; i++) printf("%d", d3[i]);
    printf("\n");

    *((int*)save) = 333 * (len+1) / 100;
    for(i = 0 ; i < prec ; i++) {
        save[4 + i] = d3[i];
    }
    FILE *filp;
    filp = fopen("pi4.dat", "wb");
    fwrite(save, 4 + prec, 1, filp);
    fflush(filp);
    fclose(filp);

    free(d3);
    free(save);
}


void f641_pi3(int len) {
    int i = 0;
    int prec;
    int *d2 = calloc(len, sizeof(int));
    int sigm = -1, higm = 0;
    int sigd =  1;
    int siga = -1;
    int *d3 = calloc(len, sizeof(int));
    int step = 1;
    int dist[10];
    struct timeval tv0, tv1;

    //
    d2[0] = 0;
    d3[0] = 2;
    for(i = 1 ; i < len ; i++) {
        d2[i] = 6;
        d3[i] = 6;
    }

    //
    gettimeofday(&tv0, NULL);
    int *d2end = d2 + len - 1;
    for(step = 2 ; step < 40000 ; step++) {
        int den = 2 * step + 1;
        int r = 0;
        int s = 0;

        // Multiply
        for(i = len - 1 ; i > sigm ; i--) {
            s = r + step * d2[i];
            r = s / 10;
            d2[i] = s % 10;
        }

        // Divide
        int div = d2[0];

        for(i = sigd ; i < len ; i++) {
            d2[i-1] = div / den;
            div = 10 * (div % den) + d2[i];
        }

        // Add
        r = 0;
        for(i = len - 1 ; i > siga ; i--) {
            s = r + d2[i] + d3[i];
            if (s < 10) {
                r = 0;
                d3[i] = s;
            } else {
                r = 1;
                d3[i] = s - 10;
            }
        }

        if (step > 12 && step % 10 == 0) {
            sigm += 3;
            sigd += 3;
            siga += 3;

            if (step % 1500 == 0) {
                int sig = 0;
                gettimeofday(&tv1, NULL);
                timersub(&tv1, &tv0, &tv0);

                for(sig = 0 ; sig < len ; sig++) {
                    if (d2[sig] != 0) break;
                }

                printf("Last %lu.%06ld (%f) : %d.%d%d%d", tv0.tv_sec, tv0.tv_usec, 3500000. / 1500. * (tv0.tv_sec + tv0.tv_usec/1000000.) / 3600., d3[0], d3[1], d3[2], d3[3]);
                for(i = 4 ; i < 25 ; i++) printf("%d", d3[i]);
                printf(" - sig = %d / %d\n", sigm, sig);

                sigm = sig - log10(step) - 3;
                sigd = sigm + 2;
                siga = sigm;

                memset(dist, 0, 40);
                for(i = higm ; i < sigm ; i++) dist[d3[i]]++;
                printf("Dist : ");
                for(i = 0 ; i < 10 ; i++) printf("%d : %2.1f | ", i, 1. * (sigm - higm) / (dist[i] + 0.001));
                printf("\n");

                higm = sigm;
                gettimeofday(&tv0, NULL);
            }
        }
    }
    gettimeofday(&tv1, NULL);
    timersub(&tv1, &tv0, &tv0);

    printf("Last %lu.%06ld : %d.", tv0.tv_sec, tv0.tv_usec, d3[0]);
    for(i = 1 ; i < 100 ; i++) printf("%d", d3[i]);
    printf("\n");

}

void f641_pi2(int len) {
    int i = 0;
    int prec;
    int *d2 = calloc(len, sizeof(int));
    int sigm = -1;
    int sigd =  1;
    int siga = -1;
    int *d3 = calloc(len, sizeof(int));
    int step = 1;
    struct timeval tv0, tv1;

    //
    d2[0] = 0;
    d3[0] = 2;
    for(i = 1 ; i < len ; i++) {
        d2[i] = 6;
        d3[i] = 6;
    }

    //
    gettimeofday(&tv0, NULL);
    int *d2end = d2 + len - 1, *p2, *d2sigm = d2 - 1, *d2sigd = d2 + 1, *d2siga = d2 - 1;
    int *d3end = d3 + len - 1, *p3;
    for(step = 2 ; step < 40000 ; step++) {
        int den = 2 * step + 1;
        long r = 0;
        long s = 0;

        // Multiply
        p2 = d2end;
        while(p2 > d2sigm) {
            s = r + *p2 * step;
            if (s > 9) {
                r = s / 10;
                *p2 = s % 10;
            } else {
                r = 0;
                *p2 = s;
            }
            p2--;
        }

        // Divide
        p2 = d2sigd - 1;
        int div = *p2;
        while(p2 < d2end) {
            if (div >= den) {
                *(p2++) = div / den;
                div = 10 * (div % den) + *p2;
            } else {
                *(p2++) = 0;
                div = 10 * div + *p2;
            }
        }
        *p2 = div / den;

        // Add
        r = 0;
        p2 = d2end;
        p3 = d3end;
        while(p2 > d2siga) {
            s = r + *(p2--) + *p3;
            if (s < 10) {
                r = 0;
                *(p3--) = s;
            } else {
                r = 1;
                *(p3--) = s - 10;
            }
        }

        if (step > 12 && step % 10 == 0) {
            d2sigm += 3;
            d2sigd += 3;
            d2siga += 3;

            if (step % 1500 == 0) {
                int sig = 0;
                gettimeofday(&tv1, NULL);
                timersub(&tv1, &tv0, &tv0);

                for(sig = 0 ; sig < len ; sig++) {
                    if (d2[sig] != 0) break;
                }

                printf("Last %lu.%06ld (%f) : %d.%d%d%d", tv0.tv_sec, tv0.tv_usec, 3500000. / 1500. * (tv0.tv_sec + tv0.tv_usec/1000000.) / 3600., d3[0], d3[1], d3[2], d3[3]);
                for(i = 4 ; i < 25 ; i++) printf("%d", d3[i]);
                printf(" - sig = %d / %d\n", sigm, sig);

                gettimeofday(&tv0, NULL);
            }
        }
    }
    gettimeofday(&tv1, NULL);
    timersub(&tv1, &tv0, &tv0);

    printf("Last %lu.%06ld : %d.", tv0.tv_sec, tv0.tv_usec, d3[0]);
    for(i = 1 ; i < 100 ; i++) printf("%d", d3[i]);
    printf("\n");

}



/*
 *
 */
int main(int argc, char *argv[]) {
    int r;

//    f641_pi_(1024*1024 + 64, 1);
//    //f641_e1(1024*1024 + 64, 1);
//    return 0;
//
//    for(r = 1000 ; r < 10000 ; r += 1000) {
//        f641_pi_(r, 0);
//    }
//
//    for(r = 10000 ; r < 20000 ; r += 1000) {
//        f641_e1(r, 0);
//    }
//
//    return 0;

    f641_v4l2(argc, argv);

    //f641_queue_test();

    return 0;
}

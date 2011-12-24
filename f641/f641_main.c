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
    char *opts = "hqvi0Dar:s:g:e:1:2:3:b:f:z:d:t:y:W:H:APICFSRV";

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
//            case 'r':
//                recording = 0;
//                break;
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
//            case 'b':
//                nb_buffers = atoi(optarg);
//                break;
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
//            case 't':
//                trigger = atoi(optarg);
//                printf("Threshold : %d\n", trigger);
//                break;
//            case 'y':
//                switch(atoi(optarg)) {
//                    case 0 : palette = 0x56595559; break;
//                    case 1 : palette = 0x47504A4D; break;
//                }
//                break;
            case 'W':
                appli->width = atoi(optarg);
                break;
            case 'H':
                appli->height = atoi(optarg);
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

extern struct f641_v4l2_parameters *f641_init_v4l2(struct f641_appli *appli);
extern void f641_free_v4l2(struct f641_v4l2_parameters *prm);

int f641_v4l2(int argc, char *argv[]) {
    int i, j, r;
    struct f641_appli           appli;
    struct f641_process_data    proc_data;
    struct f641_v4l2_parameters *v4l2;

    // APP DEFAULT DATA
    appli.width  = 1024;
    appli.height = 576;
    appli.size   = appli.width * appli.height;
    appli.frames_pers = 5;
    appli.max_frame = 0xFFFFFFFFFFFFFFL;
    snprintf(appli.device, sizeof(appli.device), "/dev/video0");

    // Parameters
    f640_getopts(&appli, argc, argv);
    appli.size   = appli.width * appli.height;

    //
    appli.recording = 1;
    snprintf(appli.record_path, sizeof(appli.record_path), "/work/test/tust.avi");

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

    printf("Appli.Process OK\n");

    // V4L2 PRM
    v4l2 = f641_init_v4l2(&appli);
    appli.width  = v4l2->width;
    appli.height = v4l2->height;
    appli.size   = appli.width * appli.height;
    appli.frames_pers = v4l2->frames_pers;
    printf("V4L2 initialized\n");

    // FFMPEG
    f641_init_ffmpeg(&appli);
    printf("FFMpeg initialized\n");


    // STONES
    struct f640_line *lineup = f640_make_lineup(v4l2->buffers, proc_data.proc_len, proc_data.grid, PIX_FMT_BGR24, NULL, NULL, 350);
    struct f640_stone *stones = calloc(proc_data.proc_len, sizeof(struct f640_stone));
    for(i = 0 ; i < proc_data.proc_len ; i++) {
        f640_make_stone(&stones[i], i);
        stones[i].stones = stones;
        stones[i].private = &lineup[i];
    }
    proc_data.stones = stones;
    printf("Stones %d OK\n", 0);

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

    long actbr[2] = {F641_LOGGED, -1};
    struct f640_queue *broaded  = f640_make_queue(stones, proc_data.proc_len, actbr, F641_BROADCASTED | F641_RECORDED, 0, 0);

    long actlg[2] = {F641_RELEASED, -1};
    struct f640_queue *logged   = f640_make_queue(stones, proc_data.proc_len, actlg, 0, 0, 0);

    printf("Queues OK\n");

    // THREAD
    struct f640_thread *groups[32];//     = calloc(32, sizeof(struct f640_thread*));
    struct f640_thread *snaping     = groups[0]  = f641_make_group(1, F641_SNAPED,        released,  1, snapped,   0, 0, f641_attrib_v4l2_snaping);
    struct f640_thread *decoding    = groups[1]  = f641_make_group(2, F641_DECODED,       snapped,   1, decoded,   0, 0, f641_attrib_decoding_mjpeg);
    struct f640_thread *watching    = groups[2]  = f641_make_group(1, F641_WATCHED,       decoded,   1, processed, 0, 2, f641_attrib_watching_422p);
    struct f640_thread *converting  = groups[3]  = f641_make_group(1, F641_CONVERTED,     decoded,   1, processed, 0, 0, f641_attrib_converting_torgb);
    struct f640_thread *graying     = groups[4]  = f641_make_group(1, F641_GRAYED,        decoded,   1, processed, 0, 0, f641_attrib_edging);
    struct f640_thread *griding     = groups[5]  = f641_make_group(1, F641_GRIDED,        processed, 1, grided,    0, 1, f641_attrib_griding);
    struct f640_thread *tagging     = groups[6]  = f641_make_group(1, F641_TAGGED,        grided,    1, tagged,    0, 0, f641_attrib_tagging);
    struct f640_thread *broading    = groups[7]  = f641_make_group(1, F641_BROADCASTED,   tagged,    1, broaded,   0, 1, f641_attrib_broadcasting);
    struct f640_thread *recording   = groups[8]  = f641_make_group(1, F641_RECORDED,      tagged,    1, broaded,   0, 1, f641_attrib_recording);
    struct f640_thread *logging     = groups[9]  = f641_make_group(1, F641_LOGGED,        broaded,   1, logged,    0, 1, f641_attrib_logging);
    struct f640_thread *releasing   = groups[10] = f641_make_group(1, F641_RELEASED,      logged,    1, released,  0, 0, f641_attrib_v4l2_desnaping);
    groups[11] = NULL;

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
        f640_enqueue(released, 0, i, F641_RELEASED);
    }
    printf("%d Buffers ready\n", proc_data.proc_len);
    usleep(300*1000);

    // STREAM ON
    f641_stream_on(v4l2);
    printf("Stream ON\n");

    // LAUNCH
    gettimeofday(&appli.process->tv0, NULL);
//    // snap
//    pthread_create(&groups[0]->group[0].id, NULL, f641_loop2,   (void *)(&groups[0]->group[0]));
//    // decode
//    pthread_create(&groups[1]->group[0].id, NULL, f641_loop2,   (void *)(&groups[1]->group[0]));
//    pthread_create(&groups[1]->group[1].id, NULL, f641_loop2,   (void *)(&groups[1]->group[1]));
//    pthread_create(&groups[1]->group[2].id, NULL, f641_loop2,   (void *)(&groups[1]->group[2]));
//    pthread_create(&groups[1]->group[3].id, NULL, f641_loop2,   (void *)(&groups[1]->group[3]));
//    // watch
//    pthread_create(&groups[2]->group[0].id, NULL, f641_loop2,   (void *)(&groups[2]->group[0]));
//    pthread_create(&groups[2]->group[1].id, NULL, f641_loop2,   (void *)(&groups[2]->group[1]));
//    pthread_create(&groups[2]->group[2].id, NULL, f641_loop2,   (void *)(&groups[2]->group[2]));
//    pthread_create(&groups[2]->group[3].id, NULL, f641_loop2,   (void *)(&groups[2]->group[3]));
    i = 0;
    while(groups[i] != NULL) {
        for(j = 0 ; j < groups[i]->group_size ; j++) {
            pthread_create(&groups[i]->group[j].id, NULL, f641_loop2,   (void *)(&groups[i]->group[j]));
        }
        i++;
    }
    printf("Launch done\n");

    //
    while(1) {
        r = read(appli.fd_get, appli.process->grid, sizeof(long) * (6 + appli.process->grid->cols * appli.process->grid->rows) );
        if (r == sizeof(long) * (6 + appli.process->grid->cols * appli.process->grid->rows)) {
            printf("READ  %dbytes : %dx%d\n", r, *((int*)(appli.process->grid+16)), *((int*)(appli.process->grid+20)));
//            pthread_mutex_lock(&lines->grid->mutex_coefs);
//            memcpy(lines->grid->grid_coefs, grid + 6 * sizeof(long), r - 6 * sizeof(long));
//            pthread_mutex_unlock(&lines->grid->mutex_coefs);
        } else if (r > 0) {
          printf("READ: wrong size read (%d), discarding\n", r);
        } else if (r) {
            printf("READ: Interrupt %d\n", r);
            usleep(1000*1000);  // bug
        }
    }
}




/*
 *
 */
int main(int argc, char *argv[]) {
    int r;

    f641_v4l2(argc, argv);

    //f641_queue_test();

    return 0;
}

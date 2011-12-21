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
int main(int argc, char *argv[]) {

    f641_queue_test();

    return 0;
}

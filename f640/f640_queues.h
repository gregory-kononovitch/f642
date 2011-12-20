/*
 * file    : f640_queues.h
 * project : f640
 *
 * Created on: Dec 19, 2011
 * Author and copyright (C) 2011 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#ifndef F640_QUEUES_H_
#define F640_QUEUES_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>


struct f640_stone {
    const int key;

    long                frame;
    long                status;
    pthread_spinlock_t  spin;

    void*   private;
};


struct f640_queue {
    struct f640_stone   *stones;    // [index] = stone[key]
    int                 length;

    //
    long                *nexts;
    long                maxi;
    long                mini;

    //
    int                 outs;
    long                *foractions;
    int                 **queue;     // [index] = key
    int                 *next_in;    // index
    int                 *next_out;   // index

    // Constraints
    long                constraints;

    // Speed / order
    int                 ecart;

    int                 *backtrack;
    long                *backs;
    long                bmaxi;
    long                bmini;


    //
    pthread_mutex_t mutex;
    pthread_cond_t  cond;

    //

};
struct f640_queue   *f640_make_queue(struct f640_stone *stones, int length, int *foractions, int *constraints, int ecart);
int                 f640_dequeue(struct f640_queue *queue, int block, int foraction);
void                f640_enqueue(struct f640_queue *queue, int block, int key, int action);
void                f640_backtrack(struct f640_queue *queue, int block, int key);

#define F640_UNBLOCK    0
#define F640_BLOCK      1

struct f640_thread {
    int                 action;
    struct f640_queue   *queue_in;
    int                 block_dequeue;
    struct f640_queue   *queue_out;
    int                 block_enqueue;

    int                 backtrack;

    int                 (*process)(struct f640_stone *stone);
};

void *f640_loop(void* prm);

#endif /* F640_QUEUES_H_ */

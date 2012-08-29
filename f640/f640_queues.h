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

#define F640_UNBLOCK    0
#define F640_BLOCK      1

#define F640_RAW        0
#define F640_ORDERED    1
#define F640_NN_1       2



struct f640_stone {
    int key;
    struct f640_stone   *stones;
    int type;
    long                frame;
    long                status;
    pthread_spinlock_t  spin;

    void*   private;
};

struct f640_tab {
    int     length;

    // Queue
    int     *tab;

    // Backtrace
    long    *nexts;
    long    mini;
    long    maxi;
};
int  f640_add(struct f640_tab *tab, struct f640_stone *stone);
void f640_del(struct f640_tab *tab, struct f640_stone *stone);

struct f640_queue {
    struct f640_stone   *stones;    // [index] = stone[key]
    int                 length;

    //
    int                 outs;
    long                *foractions;
    struct f640_tab     *queues;     // [index] = key

    // Constraints
    long                constraints;

    // Speed / order
    int                 nn_1;   // 0 : x, 1 : ordered, i : n(n-1)(n-i)

    int                 back_diff;
    struct f640_tab     *backtrack;

    //
    pthread_mutex_t mutex;
    pthread_cond_t  cond;

    //
    int run;
};
extern struct f640_queue   *f640_make_queue(struct f640_stone *stones, int length, long *foractions, long constraints, int nn_1, int back_diff);
extern int                 f640_enqueue(struct f640_queue *queue, int block, int key, long action);
extern int                 f640_dequeue(struct f640_queue *queue, int block, long foraction);
extern void                f640_backtrack(struct f640_queue *queue, int block, int key);
extern void                f640_dump_queue(struct f640_queue *queue);


struct f641_thread_operations {
    // Processing
    void*               appli;          // in
    void*               ressources;     // from init

    void*               (*init)(void *appli);
    void*               (*updt)(void *appli, void* ressources);
    int                 (*exec)(void *appli, void* ressources, struct f640_stone *stone);
    void                (*free)(void *appli, void* ressources);
};

struct f641_thread_multi_ops {
    struct f641_thread_operations *multi;
};

struct f640_thread {
    char                name[32];
    long                action;
    // In
    struct f640_queue   *queue_in;
    int                 block_dequeue;
    // Out
    struct f640_queue   *queue_out;
    int                 block_enqueue;

    // Logic
    int                 nn_1;   // 0 : raw, 1 : ordered, i : n...(n-i)

    // Old
    int                 (*process)(struct f640_stone *stone);

    // Group
    int                 group_size;
    struct f640_thread  *group;
    void                (*attrib)(struct f641_thread_operations *ops);

    // Processing
    pthread_t   id;
    struct f641_thread_operations ops;
    int loop;

};
extern void f640_make_thread(int nb, long action, struct f640_queue *queue_in, int block_dequeue, struct f640_queue *queue_out, int block_enqueue, int nn_1, int (*process)(struct f640_stone *stone));
extern void *f640_loop(void* th);

extern struct f640_thread *f641_make_group(int nb, long action, struct f640_queue *queue_in, int block_dequeue, struct f640_queue *queue_out, int block_enqueue, int nn_1
        , void (*attrib)(struct f641_thread_operations *ops));

extern int f641_init_thread(struct f640_thread *thread, void *appli);
extern void *f641_loop2(void* th);


#endif /* F640_QUEUES_H_ */

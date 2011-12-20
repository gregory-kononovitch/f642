/*
 * file    : f640_queues.c
 * project : f640
 *
 * Created on: Dec 19, 2011
 * Author and copyright (C) 2011 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */



#include "f640_queues.h"


int f640_make_stone(struct f640_stone *stone, int key) {
    stone->key      = key;
    stone->frame    = 0;
    stone->status   = 0;
    pthread_spin_init(stone->spin, 0);
    stone->private  = NULL;
    return 0;
}

struct f640_queue *f640_make_queue(struct f640_stone *stones, int length, int *foractions, long constraints, int ecart) {
    int i, *a;
    struct f640_queue *queue = calloc(1, sizeof(struct f640_queue));
    if (!queue) {
        printf("ENOMEM for a new queue, returning\n");
        return NULL;
    }
    //
    queue->stones = stones;
    queue->length = length;

    // NEXTS
    queue->nexts = calloc(1, queue->length * sizeof(long));
    for(i = 0 ; i < queue->length ; i++) queue->nexts[i] = i + 1;
    queue->mini = 1;
    queue->maxi = queue->length + 1;

    //
    if (!foractions) {
        queue->outs = 1;
        queue->foractions = NULL;
        queue->queue = calloc(1, queue->length * sizeof(int));
        queue->next_in = calloc(1, sizeof(int));
        queue->next_out = calloc(1, sizeof(int));
    } else {
        i = 0;
        for(a = foractions ; a != NULL ; a++) i++;
        queue->outs = i;
        queue->foractions = calloc(queue->outs, sizeof(int));
        memcpy(queue->foractions, foractions, queue->outs * sizeof(int));
        queue->queue = calloc(queue->outs, sizeof(int*));
        for(i = 0 ; i < queue->outs ; i++) {
            queue->queue[i] = calloc(1, queue->length * sizeof(int));
        }
        queue->next_in = calloc(queue->outs, sizeof(int));
        queue->next_out = calloc(queue->outs, sizeof(int));
    }

    //
    queue->constraints = constraints;

    //
    if (ecart < 1) {
        queue->ecart = 0;
    } else {
        queue->ecart = ecart;

    }

    // BACKS
    queue->backs = calloc(1, queue->length * sizeof(long));
    for(i = 0 ; i < queue->length ; i++) queue->backs[i] = i + 1;
    queue->bmini = 1;
    queue->bmaxi = queue->length + 1;

    //
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init (&queue->cond , NULL);
    //
    return queue;
}

int  f640_dequeue(struct f640_queue *queue, int block, int foraction) {
    int i;
    pthread_mutex_lock(&queue->mutex);
    if (queue->outs > 1) {
        for(i = 0 ; i < queue->outs ; i++) {
            if (queue->foractions[i] == foraction) break;
        }
        if (i == queue->outs) {
            pthread_mutex_unlock(&queue->mutex);
            return -1;
        } else {
            foraction = i;
        }
    } else {
        foraction = 0;
    }
    while(1) {
        if( queue->next_out[foraction] != queue->next_in[foraction] ) {
            int key = -1;
            for( i = queue->next_out[foraction] ; i != queue->next_in[foraction] ; i = (i + 1) % queue->length) {
                int j;
                key = queue->queue[foraction][i];
                // Ecart
                if ( queue->ecart == 1) {
                    if ( queue->stones[key].getNum(&queue->stones[key]) != queue->bmini ) continue;
                }
                // Ok

                // Backtrack
                if (queue->backtrack) {
                    for(j = 0 ; j < queue->length ; j++) {
                        if (queue->backtrack[foraction][j] == -1) {
                            queue->backtrack[foraction][j] = key;
                            break;
                        }
                    }
                } else if (queue->ecart == 1){
                    queue->num++;
                } else {
                    // not possible
                }
                // Reorg
                for(j = i ; j != queue->next_out[foraction] ; j = (j - 1) % queue->length ) {
                    queue->queue[foraction][j] = queue->queue[foraction][(j - 1) % queue->length];
                }
                // Increment
                queue->next_out[foraction] = (queue->next_out[foraction] + 1) % queue->length;
                // Return
                pthread_mutex_unlock(&queue->mutex);
                return key;
            }
        }
        if (block) {
            pthread_cond_wait(&queue->cond, &queue->mutex);
        } else {
            return -1;
        }
    }
}

void f640_enqueue(struct f640_queue *queue, int block, int key, int action) {
    int q, i;
    pthread_mutex_lock(&queue->mutex);
    if ( (queue->constraints & queue->stones[key].status) != queue->constraints ) {
        pthread_mutex_unlock(&queue->mutex);
        return;
    }
    queue->mini = queue->maxi;
    for(i = 0 ; i < queue->length ; i++) {
        if (queue->nexts[i] == queue->stones[key].getNum(&queue->stones[key]) ) {
            queue->nexts[i] = queue->maxi;
            queue->maxi++;
        }
        if (queue->mini > queue->nexts[i]) queue->mini = queue->nexts[i];
    }
    for(q = 0 ; q < queue->outs ; q++) {
        queue->queue[q][queue->next_in[q]] = key;
        queue->next_in[q] = (queue->next_in[q] + 1) % queue->length;
    }
    pthread_cond_broadcast(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);
}

void f640_backtrack(struct f640_queue *queue, int block, int key) {
    int q, i;
    if ( queue->backtrack == NULL) return;

    pthread_mutex_lock(&queue->mutex);
    for(q = 0 ; q < queue->outs ; q++) {
        if ( !queue->stones[key].isDone(&queue->stones[key], queue->foractions[q]) ) break;
    }
    if (q == (queue->outs + 1) ) {
        queue->bmini = queue->bmaxi;
        for(i = 0 ; i < queue->length ; i++) {
            if (queue->backtrack[i] == key) {
                queue->backtrack[i] = -1;
            }
            if (queue->backs[i] == queue->stones[key].getNum(&queue->stones[key])) {
                queue->backs[i] = queue->bmaxi;
                queue->bmaxi++;
            }
            if (queue->bmini > queue->backs[i]) queue->bmini = queue->backs[i];
        }
        pthread_cond_broadcast(&queue->cond);
    }
    pthread_mutex_unlock(&queue->mutex);
}


void *f640_loop(void* prm) {
    int r;
    struct f640_thread *thread = prm;

    while(1) {
        // Dequeue
        int key = f640_dequeue(thread->queue_in, thread->block_dequeue, thread->action);

        // Check
        if ( key < 0) {
            printf("End of in queue, exiting\n");
            break;
        }

        // Processing
        r = thread->process(&thread->queue_in->stones[key]);

        // Backtrack
        if (thread->backtrack) f640_backtrack(thread->queue_in, 0, key, thread->action);

        // Enqueue
        f640_enqueue(thread->queue_out, thread->block_enqueue, key, thread->action);
    }
    pthread_exit(NULL);
}

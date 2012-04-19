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
    int r;
    stone->key      = key;
    stone->frame    = 0;
    stone->status   = 0;
    r = pthread_spin_init(&stone->spin, 0);
    stone->private  = NULL;
    return 0;
}

int f640_make_tab(struct f640_tab *tab, int length) {
    if (!tab) return -1;
    tab->length = length;
    tab->tab = calloc(tab->length, sizeof(int));
    if (!tab->tab) return -1;
    for(length = 0 ; length < tab->length ; length++) tab->tab[length] = -1;
    tab->nexts = calloc(tab->length, sizeof(long));
    if (!tab->nexts) return -1;
    for(length = 0 ; length < tab->length ; length++) tab->nexts[length] = length + 1;
    tab->mini = 1;
    tab->maxi = tab->length + 1;
    return 0;
}

struct f640_queue *f640_make_queue(struct f640_stone *stones, int length, long *foractions
        , long constraints, int nn_1, int back_diff) {
    int i;
    long *a;
    struct f640_queue *queue = calloc(1, sizeof(struct f640_queue));
    if (!queue) {
        printf("ENOMEM for a new queue, returning\n");
        return NULL;
    }
    //
    queue->stones = stones;
    queue->length = length;

    //
    if (!foractions) {
        queue->outs = 1;
        queue->foractions = calloc(1, sizeof(long));
        queue->queues = calloc(1, sizeof(struct f640_tab));
        f640_make_tab(queue->queues, queue->length);
    } else {
        i = 0;
        for(a = foractions ; *a > 0 ; a++) i++;
        queue->outs = i;
        queue->foractions = calloc(queue->outs, sizeof(long));
        memcpy(queue->foractions, foractions, queue->outs * sizeof(long));
        queue->queues = calloc(queue->outs, sizeof(struct f640_tab));
        for(i = 0 ; i < queue->outs ; i++) {
            f640_make_tab(&queue->queues[i], queue->length);
        }
    }
    //
    queue->constraints = constraints;
    queue->nn_1 = nn_1;

    // BACKS
    if (back_diff) {
        queue->back_diff = back_diff;
        queue->backtrack = calloc(1, sizeof(struct f640_tab));
        f640_make_tab(queue->backtrack, queue->length);
    } else {
        queue->back_diff = 0;
    }

    //
    queue->run = 1;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init (&queue->cond , NULL);
    //
    return queue;
}


/*
 *
 */
int f640_add(struct f640_tab *tab, struct f640_stone *stone) {
    int i, j;
    if ( stone->frame && stone->frame < tab->mini ) return 1;       // @@@ -- pb multi&spin, already gone
    for(i = 0 ; i < tab->length ; i++) {
        if (stone->key == tab->tab[i]) return 1;       // secure (spin status + multi enq...) : still there
        if (tab->tab[i] < 0) {                     // Last entry
            tab->tab[i] = stone->key;
            return 0;
        }
        if (stone->stones[tab->tab[i]].frame > stone->frame) break; // between existing
    }
    for(j = tab->length - 1 ; j > i ; j--) tab->tab[j] = tab->tab[j-1];
    tab->tab[i] = stone->key;
    return 0;
}

void f640_del(struct f640_tab *tab, struct f640_stone *stone) {
    int i, j;
    for(i = 0 ; i < tab->length ; i++) {
        if (tab->tab[i] == stone->key) {
            break;
        }
    }
    for(j = i + 1 ; j < tab->length ; j++)
        if ( (tab->tab[j - 1] = tab->tab[j]) < 0 ) break;
    tab->tab[tab->length - 1] = -1;
    //
    tab->mini = tab->maxi;
    for(i = 0 ; i < tab->length ; i++) {
        if (tab->nexts[i] == stone->frame) {
            tab->nexts[i] = tab->maxi;
            tab->maxi++;
        }
        if (tab->mini > tab->nexts[i]) tab->mini = tab->nexts[i];
    }
}

/*
 *
 */
void f640_dump_queue(struct f640_queue *queue) {
    int q, i;
    printf("----------- QUEUE ------------\n");
    for(q = 0 ; q < queue->outs ; q++) {
        printf("Out %lX (%ld < %ld) :", queue->foractions ? queue->foractions[q] : 0, queue->queues[q].mini,queue->queues[q].maxi);
        for(i = 0 ; i < queue->length ; i++) {
            if (queue->queues[q].tab[i] > -1) printf(" %d (%ld)", queue->queues[q].tab[i], queue->stones[queue->queues[q].tab[i]].frame);
            else break;
        }
        printf("\n");
    }
    if (queue->back_diff) {
        printf("Backtrack (%ld < %ld) :", queue->backtrack->mini,queue->backtrack->maxi);
        for(i = 0 ; i < queue->length ; i++) {
            if (queue->backtrack->tab[i] > -1) printf(" %d (%ld)", queue->backtrack->tab[i], queue->stones[queue->backtrack->tab[i]].frame);
            else break;
        }
        printf("\n");
    }
}

/*
 *
 */
int f640_enqueue(struct f640_queue *queue, int block, int key, long action) {
    int q, i = 0;
    pthread_mutex_lock(&queue->mutex);
    pthread_spin_lock(&queue->stones[key].spin);
    if ( (queue->constraints & queue->stones[key].status) != queue->constraints ) {
        pthread_spin_unlock(&queue->stones[key].spin);
        pthread_mutex_unlock(&queue->mutex);
        return 1;
    }
    pthread_spin_unlock(&queue->stones[key].spin);
    for(q = 0 ; q < queue->outs ; q++) {
        i = f640_add(&queue->queues[q], &queue->stones[key]);
    }
    pthread_cond_broadcast(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);
    return i;
}


int f640_debug_dequeue = 0;
int f640_dequeue(struct f640_queue *queue, int block, long foraction) {
    int i;
    if (f640_debug_dequeue) printf("\tDeQueue (blocking %d) for %lX action / %d tabs\n", block, foraction, queue->outs);
    pthread_mutex_lock(&queue->mutex);
    if (queue->outs > 1) {
        for(i = 0 ; i < queue->outs ; i++) {
            if (queue->foractions[i] == foraction) break;
        }
        if (i == queue->outs) {
            pthread_mutex_unlock(&queue->mutex);
            printf("Bad foraction %ld, exiting queue\n", foraction);
            return -1;
        } else {
            foraction = i;
        }
    } else {
        foraction = 0;
    }
    if (f640_debug_dequeue) printf("\tDeQueue tab[%ld / %d]\n", foraction, queue->outs);
    while(1) {
        i = 0;
        if (f640_debug_dequeue) printf("\t\tDeQueue tab[%ld][0] = %d\n", foraction, queue->queues[foraction].tab[i]);
        while( queue->queues[foraction].tab[i] > -1 ) {
            struct f640_stone *st = &queue->stones[queue->queues[foraction].tab[i]];
            if (queue->nn_1 == 1) {
                // Order
                if (st->frame != queue->queues[foraction].mini)
                    goto wait;
                // Speed
                if (queue->back_diff && st->frame - queue->backtrack->mini > queue->back_diff)
                    goto wait;
                // Ok
                f640_del(&queue->queues[foraction], st);
                if (queue->back_diff) f640_add(queue->backtrack, st);
                // return
                pthread_mutex_unlock(&queue->mutex);
                return st->key;
            } else if( queue->nn_1 < 1) {
                // Speed
                if (queue->back_diff && st->frame - queue->backtrack->mini > queue->back_diff)
                    goto wait;
                // Ok
                f640_del(&queue->queues[foraction], st);
                if (queue->back_diff) f640_add(queue->backtrack, st);
                // return
                pthread_mutex_unlock(&queue->mutex);
                return st->key;
            } else if (i == 0) {
                // Speed
                if (queue->back_diff && st->frame - queue->backtrack->mini > queue->back_diff)
                    goto wait;
                // Ok
                f640_del(&queue->queues[foraction], st);
                if (queue->back_diff) f640_add(queue->backtrack, st);
                // return
                pthread_mutex_unlock(&queue->mutex);
                return st->key;
            } else {
                int j, k;
                for(j = 1 ; j < queue->nn_1 ; j++) {
                    for(k = 0 ; k < queue->length ; k++)
                        if (queue->queues[foraction].nexts[k] == (st->frame - j) )
                            break;
                    if (k < queue->length) break;
                }
                if (j != queue->nn_1) goto next;
                // Speed
                if (queue->back_diff && st->frame - queue->backtrack->mini > queue->back_diff)
                    goto wait;
                // Ok
                f640_del(&queue->queues[foraction], st);
                if (queue->back_diff) f640_add(queue->backtrack, st);
                // return
                pthread_mutex_unlock(&queue->mutex);
                return st->key;
            }
next:
            i++;
        }
wait:
        if (block) {
            pthread_cond_wait(&queue->cond, &queue->mutex);
            if (!queue->run) {
                return -1;
            }
        } else {
            return -1;
        }
    }
}


void f640_backtrack(struct f640_queue *queue, int block, int key) {
    int q;
    if ( queue->back_diff < 1) return;

    pthread_mutex_lock(&queue->mutex);
    pthread_spin_lock(&queue->stones[key].spin);
    for(q = 0 ; q < queue->outs ; q++) {        // bad no synchr mngmt (status can change, but ok)
        if ( (queue->stones[key].status & queue->foractions[q]) != queue->foractions[q]) {
            pthread_spin_unlock(&queue->stones[key].spin);
            pthread_mutex_unlock(&queue->mutex);
            return;
        }
    }
    pthread_spin_unlock(&queue->stones[key].spin);
    f640_del(queue->backtrack, &queue->stones[key]);
    pthread_cond_broadcast(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);
}

static int debug_loop = 0;
void *f640_loop(void* th) {
    int r;
    struct f640_thread *thread = th;
    int times = 0;
    struct timeval tve1, tve2, tve3, tvb1, tvb2, tvb3, tvd1, tvd2, tvd3;

    timerclear(&tvd3);
    timerclear(&tvb3);
    timerclear(&tve3);
    if (debug_loop) printf("%s : launched\n", thread->name);
    while(1) {
        // Dequeue
        if (times) gettimeofday(&tvd1, NULL);
        int key = f640_dequeue(thread->queue_in, thread->block_dequeue, thread->action);
        if (times) gettimeofday(&tvd2, NULL);
        if (times) timersub(&tvd2, &tvd1, &tvd1);
        if (times) timeradd(&tvd3, &tvd1, &tvd3);

        // Check
        if ( key < 0) {
            printf("End of in queue for thread %s, exiting\n", thread->name);
            break;
        }
        struct f640_stone *stone = &thread->queue_in->stones[key];
        if (debug_loop) printf("%s : DeQueue %d (%ld)\n", thread->name, key, stone->frame);

        // Processing
        r = thread->process(stone);

        // Flaging
        pthread_spin_lock(&stone->spin);
        stone->status |= thread->action;
        pthread_spin_unlock(&stone->spin);

        // Backtrack
        if (thread->queue_in->back_diff) {
            if (debug_loop) printf("%s : Backtrack %d (%ld)\n", thread->name, key, stone->frame);
            if (times) gettimeofday(&tvb1, NULL);
            f640_backtrack(thread->queue_in, 0, key);
            if (times) gettimeofday(&tvb2, NULL);
            if (times) timersub(&tvb2, &tvb1, &tvb1);
            if (times) timeradd(&tvb3, &tvb1, &tvb3);
        }

        // Enqueue
        if (times) gettimeofday(&tve1, NULL);
        r = f640_enqueue(thread->queue_out, thread->block_enqueue, key, thread->action);
        if (times) gettimeofday(&tve2, NULL);
        if (times) timersub(&tve2, &tve1, &tve1);
        if (times) timeradd(&tve3, &tve1, &tve3);

        if (debug_loop) printf("%s : EnQueue '%lX' %d (%ld) : %s (%lX / %lX)\n", thread->name, thread->action, key, stone->frame, r ? "failed" : "succeed", stone->status, thread->queue_out->constraints);

        if (debug_loop) f640_dump_queue(thread->queue_in);
        if (debug_loop) f640_dump_queue(thread->queue_out);
    }
    pthread_exit(NULL);
}

static int debug_loop2 = 0;
void *f641_loop2(void* th) {
    int r;
    struct f640_thread *thread = th;
    int times = 0;
    struct timeval tve1, tve2, tve3, tvb1, tvb2, tvb3, tvd1, tvd2, tvd3;

    timerclear(&tvd3);
    timerclear(&tvb3);
    timerclear(&tve3);
    if (debug_loop2) printf("%s : launched\n", thread->name);
    while(thread->loop) {
        // Dequeue
        if (times) gettimeofday(&tvd1, NULL);
        int key = f640_dequeue(thread->queue_in, thread->block_dequeue, thread->action);
        if (times) gettimeofday(&tvd2, NULL);
        if (times) timersub(&tvd2, &tvd1, &tvd1);
        if (times) timeradd(&tvd3, &tvd1, &tvd3);

        // Check
        if ( key < 0) {
            printf("\nEnd of in queue for thread %s, exiting", thread->name);
            break;
        }
        struct f640_stone *stone = &thread->queue_in->stones[key];
        if (debug_loop2) printf("%s : DeQueue %d (%ld)\n", thread->name, key, stone->frame);

        // Processing
        if (debug_loop2) printf("%s : Exec %p ; appli %p ; res %p ; stone %p\n", thread->name, thread->ops.exec, thread->ops.appli, thread->ops.ressources, stone);
        r = thread->ops.exec(thread->ops.appli, thread->ops.ressources, stone);

        // Flaging
        pthread_spin_lock(&stone->spin);
        stone->status |= thread->action;
        pthread_spin_unlock(&stone->spin);

        // Backtrack
        if (thread->queue_in->back_diff) {
            if (debug_loop2) printf("%s : Backtrack %d (%ld)\n", thread->name, key, stone->frame);
            if (times) gettimeofday(&tvb1, NULL);
            f640_backtrack(thread->queue_in, 0, key);
            if (times) gettimeofday(&tvb2, NULL);
            if (times) timersub(&tvb2, &tvb1, &tvb1);
            if (times) timeradd(&tvb3, &tvb1, &tvb3);
        }

        // Enqueue
        if (thread->queue_out) {
            if (times) gettimeofday(&tve1, NULL);
            r = f640_enqueue(thread->queue_out, thread->block_enqueue, key, thread->action);
            if (times) gettimeofday(&tve2, NULL);
            if (times) timersub(&tve2, &tve1, &tve1);
            if (times) timeradd(&tve3, &tve1, &tve3);

            if (debug_loop2) printf("%s : EnQueue '%lX' %d (%ld) : %s (%lX / %lX)\n", thread->name, thread->action, key, stone->frame, r ? "failed" : "succeed", stone->status, thread->queue_out->constraints);
        }

//        if (debug_loop2) f640_dump_queue(thread->queue_in);
//        if (debug_loop2) f640_dump_queue(thread->queue_out);
    }

    //
    if (thread->ops.free && thread->ops.ressources) {
        thread->ops.free(thread->ops.appli, thread->ops.ressources);
    }

    //printf("\nEnd of thread %s", thread->name);
    pthread_exit(NULL);
}

void f640_make_thread(int nb, long action, struct f640_queue *queue_in, int block_dequeue, struct f640_queue *queue_out, int block_enqueue, int nn_1, int (*process)(struct f640_stone *stone)) {
    int t;
    for(t = 0 ; t < nb ; t++) {
        struct f640_thread *th = calloc(1, sizeof(struct f640_thread));
        sprintf(th->name, "Thread '%lX' n%d", action, t);
        th->action = action;
        th->queue_in = queue_in;
        th->block_dequeue = block_dequeue;
        th->queue_out = queue_out;
        th->block_enqueue = block_enqueue;
        th->nn_1 = nn_1;
        th->process = process;
        th->loop = 1;

        pthread_t *thread = calloc(1, sizeof(pthread_t));

        // Launch
        pthread_create(thread, NULL, f640_loop, (void *)th);
    }
}

struct f640_thread *f641_make_group(int nb, long action, struct f640_queue *queue_in, int block_dequeue, struct f640_queue *queue_out, int block_enqueue, int nn_1
        , void (*attrib)(struct f641_thread_operations *ops)) {
    int t;

    struct f640_thread *group = calloc(nb, sizeof(struct f640_thread));

    for(t = 0 ; t < nb ; t++) {
        struct f640_thread *th = &group[t];
        th->group = group;
        th->group_size = nb;
        sprintf(th->name, "Thread '%lX' n%d", action, t);
        th->action = action;
        th->queue_in = queue_in;
        th->block_dequeue = block_dequeue;
        th->queue_out = queue_out;
        th->block_enqueue = block_enqueue;
        th->nn_1 = nn_1;

        th->attrib = attrib;
        if (attrib) {
            attrib(&th->ops);
        }

        th->loop = 1;
    }

    return &group[0];
}

int f641_init_thread(struct f640_thread *thread, void *appli) {

    thread->ops.appli = appli;
    if (thread->ops.init) {
        thread->ops.ressources = thread->ops.init(appli);
        if (!thread->ops.ressources) {                      // @@@ return value
            return -1;
        }
    } else {
        thread->ops.ressources = NULL;
    }
    thread->loop = 1;
    return 0;
}

/*
 * file    : f642_lines.c
 * project : f640
 *
 * Created on: Sep 25, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */



#include "f642_x264.h"


queue642 *queue_open642(f642x264 *x264, int nb) {
    int i, r;
    nb = 3;
    queue642 *q = (queue642*)calloc(1, sizeof(queue642));
    //q->yuv = (x264_picture_t**)calloc(nb, sizeof(x264_picture_t*));
    q->index1 = (int*)calloc(nb, sizeof(int));
    q->index2 = (int*)calloc(nb, sizeof(int));
    q->tvs = (struct timeval*)calloc(nb, sizeof(struct timeval));

    q->nb_yuv = nb;
    //
    for(i = 0 ; i < nb ; i++) {
        r = x264_picture_alloc(&q->yuv[i], X264_CSP_I422, x264->width, x264->height);
        q->yuv[i].opaque = x264;
    }
    //
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
    //
    q->next1 = q->last1 = 0, q->full1 = 1;
    q->next2 = q->last2 = 0, q->full2 = 0;
    q->run = 1;
    return q;
}

void queue_close642(queue642 **queue) {
    int i;
    pthread_mutex_lock((*queue)->mutex);
    queue->run = 0;
    while((*queue)->full2 || (*queue)->last2 != (*queue)->next2) {
        pthread_cond_wait(&(*queue)->attr, &(*queue)->mutex);
    }
    (*queue)->run = 0;
    pthread_mutex_unlock((*queue)->mutex);
    void *ret;
    pthread_join((*queue)->thread, &ret);

    pthread_mutex_destroy(&(*queue)->mutex);
    pthread_cond_destroy(&(*queue)->cond);
    for(i = 0 ; i < (*queue)->nb_yuv ; i++) {
        x264_picture_clean(&(*queue)->yuv[i]);
    }
    free((*queue)->tvs);
    free((*queue)->index1);
    free((*queue)->index2);
//    free((*queue)->yuv);
    free(*queue);
    queue = NULL;
}

int dequ1ue642(queue642 *queue) {
    int i;
    pthread_mutex_lock(&queue->mutex);
    while(!queue->full1 && queue->next1 == queue->last1) {
        pthread_cond_wait(&queue->cond, &queue->mutex);
        if (!queue->run) return -1;
    }
    i = queue->index1[queue->next1];
    queue->next1 = (queue->next1 + 1) % queue->nb_yuv;
    queue->full1 = 0;
    pthread_cond_broadcast(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);

    return i;
}

int dequ2ue642(queue642 *queue, struct timeval *tv) {
    int i;
    pthread_mutex_lock(&queue->mutex);
    while(!queue->full2 && queue->next2 == queue->last2) {
        pthread_cond_wait(&queue->cond, &queue->mutex);
        if (!queue->run) return -1;
    }
    i = queue->index1[queue->next2];
    memcpy(tv, &queue->tvs[queue->next2], sizeof(struct timeval));
    queue->next2 = (queue->next2 + 1) % queue->nb_yuv;
    queue->full1 = 0;
    pthread_cond_broadcast(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);

    return i;
}

void enqu1ue642(queue642 *queue, int i) {
    pthread_mutex_lock(&queue->mutex);
    queue->index1[queue->last1] = i;
    queue->last1 = (queue->last1 + 1) % queue->nb_yuv;
    if (queue->next1 == queue->last1) {
        queue->full1 = 1;
    }
    pthread_cond_broadcast(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);
}

void enqu2ue642(queue642 *queue, int i, struct timeval tv) {
    pthread_mutex_lock(&queue->mutex);
    queue->index2[queue->last2] = i;
    memcpy(&queue->tvs[queue->next2], &tv, sizeof(struct timeval));
    queue->last2 = (queue->last2 + 1) % queue->nb_yuv;
    if (queue->next2 == queue->last2) {
        queue->full2 = 1;
    }
    pthread_cond_broadcast(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);
}

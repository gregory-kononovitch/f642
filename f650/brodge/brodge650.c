/*
 * file    : brodge650.c
 * project : f640
 *
 * Created on: Sep 7, 2012 (fork of f650_brodge.c)
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "brodge650.h"

static float lini(float d, float p) {
    if (p != 0) d /= p;
    else d = 0;
    return d > 0 ? d : -d;
}

void brodge_init_src(brodge650 *brodge, bsource650 *src) {
    src->x = .5 * brodge->width;
    src->y = .5 * brodge->height;
    src->i = &lini;
    src->m = 1.f;
    src->r = .75f;
    src->g = 1.f;
    src->b = .25f;
    //
    src->p = 1. / 150;
    src->h = 0.f;
    src->e = 1.;
    // axis
    src->cos = 1.;
    src->sin = 0.;
    //
    src->a = 1.;
    src->b = 1.;
    //
    src->flags = BRDG_SIMPLE;
}

void brodge_turn_src(brodge650 *brodge, bsource650 *src, float cos, float sin) {
    src->cos = cos;
    src->sin = sin;
    src->flags |= BRDG_AXIS;
}

void brodge_scale_src(brodge650 *brodge, bsource650 *src, float a, float b) {
    src->a = a;
    src->b = b;
    src->flags |= BRDG_SCALE;
}

brodge650 *brodge_init(int width, int height, int nb_src) {
    int i;
    brodge650 *brodge = calloc(1, sizeof(brodge650));
    bsource650 **src;
    if (!brodge) return NULL;
    //
    //nb_src = 1;
    // Brodge
    brodge->img = calloc(sizeof(float), 3 * width * height);
    brodge->width  = width;
    brodge->height = height;
    brodge->nb_src = nb_src;
    //
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand((unsigned int)tv.tv_usec);        // @@@ outside
    //
    src = calloc(nb_src, sizeof(void*));
    brodge->sources = src;
    for(i = 0 ; i < nb_src ; i++) {
        vect650 v;
        src[i] = calloc(nb_src, sizeof(bsource650));
        brodge_init_src(brodge, src[i]);
        random650(&v);
        src[i]->x = (.5 + v.x) * width;
        src[i]->y = (.5 + v.y) * height;
        src[i]->i = &lini;
        src[i]->p = 1. / (100 + 0.25 * v.z * width);
        src[i]->m = 1.f;
        src[i]->r = .5 * (1. + v.x);
        src[i]->g = .5 * (1. + v.y);
        src[i]->b = .5 * (1. + v.z);
        //
        src[i]->parm = calloc(1, sizeof(vect650));
        ((vect650*)src[i]->parm)->x = src[i]->x;
        ((vect650*)src[i]->parm)->y = src[i]->y;
        ((vect650*)src[i]->parm)->z = v.z / 100.;
        ((vect650*)src[i]->parm)->t = 0;
    }
    src[0]->flags = BRDG_HYPERBOLE;
    if (brodge->nb_src > 1) src[1]->flags = BRDG_ELLIPSE;
    if (brodge->nb_src > 2) src[2]->flags = BRDG_XLINE;
//    brodge_scale_src(brodge, src[0], 9./16., 1);
//    brodge_turn_src(brodge, src[0], .707, .707);
    //

    //
    brodge->thread = NULL;
    brodge->mutex = (pthread_mutex_t*)calloc(1, sizeof(pthread_mutex_t));
    brodge->cond  = (pthread_cond_t*)calloc(1, sizeof(pthread_cond_t));
    pthread_mutex_init(brodge->mutex, NULL);
    pthread_cond_init (brodge->cond , NULL);

    return brodge;
}

struct brodge_thread {
    brodge650       *brodge;
    bgra650         *bgra;
    pthread_attr_t  attr;
};
struct _prm_ {
    void     *desk;
    void     *zbrd;
    brodge650   *brodge;
    bgra650     *img1;
    bgra650     *img2;
    int         img;
};


static void *brodge_loop(void *prm) {
    struct timeval tv1, tv2;
    struct brodge_thread *bt = (struct brodge_thread*) prm;
    if (!bt) return NULL;
    brodge650 *brodge = bt->brodge;
    if (!brodge) return NULL;
    // ### from gui.c
    bgra650 *img1 = ((struct _prm_*)brodge->prm)->img1;
    bgra650 *img2 = ((struct _prm_*)brodge->prm)->img2;
    int *img = &((struct _prm_*)brodge->prm)->img;
    *img = 2;
    //
    gettimeofday(&tv1, NULL);
    brodge_exec(brodge, img2);
    brodge->callback(brodge->prm);
    brodge->frame++;
    //
    while(brodge->running) {
        FOG("Thread Brodge : frame = %ld for %.3f s", brodge->frame, brodge->time);
        // wait
        gettimeofday(&tv2, NULL);
        timersub(&tv2, &tv1, &tv2);
        brodge->time += tv2.tv_sec + 0.000001 * tv2.tv_usec;
        tv2.tv_sec = brodge->us - 1000000L * tv2.tv_sec - tv2.tv_usec;
        if (tv2.tv_usec > 3000) {
            usleep(tv2.tv_sec);
        }
        // exec
        gettimeofday(&tv1, NULL);
        if (*img == 2) {
            brodge_exec(brodge, img1);
            *img = 1;
        } else {
            brodge_exec(brodge, img2);
            *img = 2;
        }
        if (brodge->callback) brodge->callback(brodge->prm);
        brodge->frame++;
        // pause
//        if (brodge->pause) {
//            pthread_mutex_lock(brodge->mutex);
//            while(brodge->pause) {
//                pthread_cond_wait(brodge->cond, brodge->mutex);
//            }
//            pthread_mutex_unlock(brodge->mutex);
//        }
    }
    brodge->running = 0;
    brodge->pause   = 0;
    if (brodge->thread) free(brodge->thread);
    brodge->thread = NULL;
    free(prm);
    FOG("Brodge loop ended");
    return NULL;
}

int brodge_start(brodge650 *brodge, bgra650 *bgra, long us) {
    if (brodge->thread) {
        return -1;
    }
    //
    brodge->us = us;
    //
    struct brodge_thread *prm = calloc(1, sizeof(struct brodge_thread));
    prm->brodge = brodge;
    prm->bgra   = bgra;
    brodge->thread = calloc(1, sizeof(pthread_t));
    brodge->running = 1;
    brodge->pause = 0;
    //
    pthread_attr_init(&prm->attr);
    pthread_attr_setdetachstate(&prm->attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(brodge->thread, &prm->attr, brodge_loop, (void *)prm);

    return 0;
}


void brodge_stop(brodge650 *brodge) {
    brodge->running = 0;
    brodge->pause = 0;
    if (!brodge->thread) return;
    pthread_mutex_lock(brodge->mutex);
    brodge->running = 0;
    brodge->pause = 0;
    pthread_cond_broadcast(brodge->cond);
    pthread_mutex_unlock(brodge->mutex);
    //
    void *ret;
    pthread_join(*brodge->thread, &ret);
}

void brodge_pause(brodge650 *brodge) {
    if (!brodge->thread) return;
    pthread_mutex_lock(brodge->mutex);
    brodge->pause = 1;
    pthread_mutex_unlock(brodge->mutex);
}


void brodge_rebase(brodge650 *brodge) {
    int i;
    for(i = 0 ; i < brodge->nb_src ; i++) {
        vect650 v;
        random650(&v);
        brodge->sources[i]->x = (.5 + v.x) * brodge->width;
        brodge->sources[i]->y = (.5 + v.y) * brodge->height;
        brodge->sources[i]->i = &lini;
        brodge->sources[i]->p = 1. / (100 + 0.25 * v.z * brodge->width);
        brodge->sources[i]->m = 1.f;
        brodge->sources[i]->r = .5 * (1. + v.x);
        brodge->sources[i]->g = .5 * (1. + v.y);
        brodge->sources[i]->b = .5 * (1. + v.z);
        //
        ((vect650*)brodge->sources[i]->parm)->x = brodge->sources[i]->x;
        ((vect650*)brodge->sources[i]->parm)->y = brodge->sources[i]->y;
        ((vect650*)brodge->sources[i]->parm)->z = v.z / 100.;
        ((vect650*)brodge->sources[i]->parm)->t = 0;
    }
}

void brodge_free(brodge650 **brodge) {
    int i;
    //
    pthread_cond_destroy((*brodge)->cond);
    pthread_mutex_destroy((*brodge)->mutex);
    if ((*brodge)->thread) free((*brodge)->thread);
    //
    for(i = 0 ; i < (*brodge)->nb_src ; i++) {
        if ((*brodge)->sources[i]->parm) free((*brodge)->sources[i]->parm);
        free((*brodge)->sources[i]);
    }
    free((*brodge)->sources);
    free((*brodge)->img);
    free(*brodge);
    *brodge = NULL;
}

int brodge_anim(brodge650 *brodge) {
    int i;
    vect650 alea;

    for(i = 0 ; i < brodge->nb_src ; i++) {
        random650(&alea);
        ((vect650*)brodge->sources[i]->parm)->t += 1.;
        brodge->sources[i]->x = ((vect650*)brodge->sources[i]->parm)->x * cos(6.3 * ((vect650*)brodge->sources[i]->parm)->t * ((vect650*)brodge->sources[i]->parm)->z);
        brodge->sources[i]->y = ((vect650*)brodge->sources[i]->parm)->y * sin(6.3 * ((vect650*)brodge->sources[i]->parm)->t * ((vect650*)brodge->sources[i]->parm)->z);
        //
        brodge->sources[i]->h  = 5. * brodge->sources[i]->e;
//        brodge_turn_src(brodge, brodge->sources[i]
//                  , cos(6.3 * ((vect650*)brodge->sources[i]->parm)->t * ((vect650*)brodge->sources[i]->parm)->z)
//                  , sin(6.3 * ((vect650*)brodge->sources[i]->parm)->t * ((vect650*)brodge->sources[i]->parm)->z)
//        );
        brodge->sources[i]->e += 1;
        //
        random650(&alea);
//        brodge->sources[i].m = 1.f;
        brodge->sources[i]->r *= (1 + 0.05 * alea.x);
        brodge->sources[i]->g *= (1 + 0.05 * alea.y);
        brodge->sources[i]->b *= (1 + 0.05 * alea.z);
        float max;
        if (brodge->sources[i]->r > brodge->sources[i]->g) {
            if (brodge->sources[i]->r > brodge->sources[i]->b) max = brodge->sources[i]->r;
            else max = brodge->sources[i]->b;
        } else {
            if (brodge->sources[i]->g > brodge->sources[i]->b) max = brodge->sources[i]->g;
            else max = brodge->sources[i]->b;
        }
        brodge->sources[i]->r /= max;
        brodge->sources[i]->g /= max;
        brodge->sources[i]->b /= max;
    }

    return 0;
}

int brodge_exec(brodge650 *brodge, bgra650 *img) {
    long l1, l2, l;
    struct timeval tv1, tv2;

    //
    gettimeofday(&tv1, NULL);

    //
    memset(brodge->img, 0, 3 * brodge->width * brodge->height * sizeof(float));

    l1 = ReadTSC();
    l  = brodga650(brodge, img);
    l2 = ReadTSC();
    gettimeofday(&tv2, NULL);
    timersub(&tv2, &tv1, &tv2);

    if (brodge->frame % 30 == 29) {
        printf("Brodge return %ld for %ld µops [ %.3fM ; %ld ; %ld ] : %.1f Hz := %.0f ms\n", l, (l2 - l1)
                , 0.000001 * (l2 - l1) / brodge->nb_src
                , (l2 - l1) / brodge->nb_src / brodge->height
                , (l2 - l1) / brodge->nb_src / brodge->width / brodge->height
                , 1.5e9 / (l2 - l1)
                , .001 * tv2.tv_usec
        );
    }
    //
    return 0;
}

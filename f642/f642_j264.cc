/*
 * file    : f642_j264.cc
 * project : f640
 *
 * Created on: Aug 22, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "f642_x264.hh"

#include <jni.h>
#include "t508_u640_video_F642X264.h"

using namespace t508::f642;


static X264 *toX264(jlong peer) {
  X264 *x264;
  if (!peer) return NULL;
  __builtin_memcpy(&x264, &peer, sizeof(void*));
  return x264;
}

JNIEXPORT jlong JNICALL Java_t508_u640_video_F642X264__1init
(JNIEnv *env, jobject j264, jint jwidth, jint jheight, jint pixin, jfloat jfps, jint jpreset, jint jtune) {
    jlong ptr;
    X264 *peer = NULL;
    switch(pixin) {
        case 1 :
            peer = new X264(jwidth, jheight, PIX_FMT_BGRA, jfps, jpreset, jtune);
            break;
        case 2 :
            peer = new X264(jwidth, jheight, PIX_FMT_BGRA, jfps, jpreset, jtune);
            break;
        case 5 :
            peer = new X264(jwidth, jheight, PIX_FMT_BGR24, jfps, jpreset, jtune);
            break;
    }

    if (!peer) {
      fprintf(stderr,"Class creation pb, peer null.\n");
      return 0;
    }
    __builtin_memcpy(&ptr, &peer, sizeof(void*));

    fprintf(stderr,"jx264 Init: x264 %p | width %d | height %d | fps %f |\n", peer, peer->width, peer->height, peer->fps);
    return ptr;
}

JNIEXPORT void JNICALL Java_t508_u640_video_F642X264__1free
(JNIEnv *env, jobject j264, jlong peer) {
    X264 *x264 = toX264(peer);
    if (x264) {
      fprintf(stderr,"   Freeing X264\n");
      delete x264;
    }
}

JNIEXPORT jint JNICALL Java_t508_u640_video_F642X264__1open
(JNIEnv *env, jobject j264, jlong peer, jstring jpath) {
    X264 *x264 = toX264(peer);
    if (!x264) return -1;
    jboolean b = 0;
    const char *path = env->GetStringUTFChars(jpath, &b);

    return x264->open(path);
}

JNIEXPORT jint JNICALL Java_t508_u640_video_F642X264__1add_1frame__J_3B
(JNIEnv *env, jobject j264, jlong peer, jbyteArray jrgb) {
    X264 *x264 = toX264(peer);
    if (!x264) return -1;
    //
    jboolean isCopy = 0;
    uint8_t *rgb = (uint8_t*) env->GetByteArrayElements(jrgb, &isCopy);
    int r = x264->addFrame(rgb);
    free(rgb);
    return r;
}

JNIEXPORT jint JNICALL Java_t508_u640_video_F642X264__1add_1frame__J_3I
(JNIEnv *env, jobject j164, jlong peer, jintArray jargb) {
    X264 *x264 = toX264(peer);
    if (!x264) return -1;
    //
    jboolean isCopy = 0;
    uint8_t *argb = (uint8_t*) env->GetIntArrayElements(jargb, &isCopy);
    int r = x264->addFrame(argb);
    free(argb);
    return r;
}

JNIEXPORT jint JNICALL Java_t508_u640_video_F642X264__1close
(JNIEnv *env, jobject j264, jlong peer) {
    X264 *x264 = toX264(peer);
    if (!x264) return -1;
    return x264->close();
}

JNIEXPORT jint JNICALL Java_t508_u640_video_F642X264__1set_1qp__JI
(JNIEnv *env, jobject j264, jlong peer, jint jqp) {
    X264 *x264 = toX264(peer);
    if (!x264) return -1;

    return x264->setQP(jqp);
}

JNIEXPORT jint JNICALL Java_t508_u640_video_F642X264__1set_1qp__JIII
(JNIEnv *env, jobject j264, jlong peer, jint jqpmin, jint jqpmax, jint jqpstep) {
    X264 *x264 = toX264(peer);
    if (!x264) return -1;

    return x264->setQP(jqpmin, jqpmax, jqpstep);
}

JNIEXPORT jint JNICALL Java_t508_u640_video_F642X264__1set_1param
(JNIEnv *env, jobject j264, jlong peer, jstring jname, jstring jvalue) {
    jboolean b = 0;
    const char *name = env->GetStringUTFChars(jname, &b);
    const char *value = env->GetStringUTFChars(jvalue, &b);
    X264 *x264 = toX264(peer);
    if (!x264) return -1;

    return x264->setParam(name, value);
}

JNIEXPORT jint JNICALL Java_t508_u640_video_F642X264__1set_1nb_1threads
(JNIEnv *env, jobject j264, jlong peer, jint jnb) {
    X264 *x264 = toX264(peer);
    if (!x264) return -1;

    return x264->setNbThreads(jnb);
}

JNIEXPORT void JNICALL Java_t508_u640_video_F642X264__1set_1log_1level
(JNIEnv *env, jobject j264, jlong peer, jint jlevel) {
    X264 *x264 = toX264(peer);
    if (!x264) return;
    x264->setLogLevel(jlevel);
}

JNIEXPORT void JNICALL Java_t508_u640_video_F642X264__1dump_1config
(JNIEnv *env, jobject j264, jlong peer) {
    X264 *x264 = toX264(peer);
    if (!x264) return;
    x264->dumpConfig();
}


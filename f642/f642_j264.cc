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



JNIEXPORT jlong JNICALL Java_t508_u640_video_F642X264_init
(JNIEnv *env, jobject j264, jint jwidth, jint jheight, jfloat jfps, jstring jpath) {
    return 0;
}

JNIEXPORT void JNICALL Java_t508_u640_video_F642X264_free (JNIEnv *env, jobject j264, jlong peer) {
    X264 *x264 = toX264(peer);
    if (x264) {
      fprintf(stderr,"   Freeing X264\n");
      delete x264;
    }
}

JNIEXPORT jint JNICALL Java_t508_u640_video_F642X264_write_1frame (JNIEnv *env, jobject j264, jlong peer, jbyteArray rgb) {
    X264 *x264 = toX264(peer);
    if (!x264) return -1;

    return 0;
}

JNIEXPORT jint JNICALL Java_t508_u640_video_F642X264_close (JNIEnv *env, jobject j264, jlong peer) {
    X264 *x264 = toX264(peer);
    if (!x264) return -1;

    return 0;
}

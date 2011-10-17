/*
 * p644_jbrodger.cpp
 *
 * Date Oct 16, 2011
 * Author and copyright (C) 2011 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "p644_brodger.h"

#include <jni.h>
#include "u640_CWrap.h"

using namespace p644;


static Brodger *toBrodger(jlong peer) {
  Brodger *brodger;
  if (!peer) return NULL;
  __builtin_memcpy(&brodger, &peer, sizeof(void*));
  return brodger;
}

/*
 * Class:     u640_CWrap
 * Method:    init
 * Signature: (III)J
 */
JNIEXPORT jlong JNICALL Java_u640_CWrap_init(JNIEnv *env, jobject obj, jint width, jint height, jint numOsc) {
    Brodger tmp = Brodger(width, height, numOsc), *peer;
    jlong ret;
    peer = (Brodger*)malloc(sizeof(Brodger));
    if (!peer) {
      printf("Memory pb, peer null.\n");
      return 0;
    }
    __builtin_memcpy(peer, &tmp, sizeof(Brodger));
    __builtin_memcpy(&ret, &peer, sizeof(void*));
    
    printf("CPPInit: brodger %p | oscs %p | img %p | width %d\n", peer, peer->oscs, peer->img, peer->width);
    return ret;
}

/*
 * Class:     u640_CWrap
 * Method:    alloc
 * Signature: (J)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_u640_CWrap_alloc(JNIEnv *env, jobject obj, jlong peer) {
  Brodger *brodger = toBrodger(peer);
  if (!peer) return NULL;
  printf("CPPAlloc: brodger %p | oscs %p | img %p | width %d\n", brodger, brodger->oscs, brodger->img, brodger->width);
  return env->NewDirectByteBuffer((void*)brodger->img, (jlong)(3*brodger->width*brodger->height));
}

/*
 * Class:     u640_CWrap
 * Method:    free
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_u640_CWrap_free(JNIEnv *env, jobject obj, jlong peer) {
  Brodger *brodger = toBrodger(peer);
  if (brodger) {
    printf("   Freeing brodger\n");
    brodger->~Brodger();
  }
}
  
/*
 * Class:     u640_CWrap
 * Method:    brodger
 * Signature: (J[BII)V
 */
JNIEXPORT void JNICALL Java_u640_CWrap_brodger(JNIEnv *env, jobject obj, jlong peer, jbyteArray im, jint width, jint height) {
  uint8_t  *img;
  jboolean isCopy;
  Brodger brodger = Brodger(width, height);

  isCopy = 0;
  img = (uint8_t*) env->GetByteArrayElements(im, &isCopy);
  brodger.brodge(img);
  printf("JNIBrodger done with isCopy = \"%u\"\n", isCopy);
  img[ 0] = 1;
  img[ 1] = 1;
  img[10] = 1;
}


/*
 * Class:     u640_CWrap
 * Method:    process
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_u640_CWrap_process(JNIEnv *env, jobject obj, jlong peer) {
    Brodger *brodger = toBrodger(peer);
    printf("CPPProcess: brodger %p | oscs %p | img %p | width %d\n", brodger, brodger->oscs, brodger->img, brodger->width);
    
    brodger->brodge();
    brodger->img[ 0] = 2;
    brodger->img[ 1] = 2;
    brodger->img[10] = 2;
}

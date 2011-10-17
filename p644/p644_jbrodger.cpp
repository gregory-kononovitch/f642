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
 * Method:    allocArray
 * Signature: (J)[B
 */
JNIEXPORT jbyteArray JNICALL Java_u640_CWrap_allocArray(JNIEnv *env, jobject obj, jlong peer) {
  Brodger *brodger = toBrodger(peer);
  jboolean isCopy;
  jbyteArray ret = env->NewByteArray((jsize)(3*brodger->width*brodger->height) );
  if (!ret) {
    printf("Can't create byte array\n");
    return NULL;
  }
  brodger->img = (uint8_t*)env->GetByteArrayElements(ret, &isCopy);
  printf("CPPBrodger alloc with a byte array (copy %u)\n", isCopy);
  return ret;
}

/*
 * Class:     u640_CWrap
 * Method:    allocBuffer
 * Signature: (J)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_u640_CWrap_allocBuffer(JNIEnv *env, jobject obj, jlong peer) {
  Brodger *brodger = toBrodger(peer);
  if (!brodger) return NULL;
  if (!brodger->img) {
    brodger->img = (uint8_t*) malloc(3*brodger->width*brodger->height);
    if (!brodger->img) {
      printf("Memory pb, no image.\n");
      return NULL;
    }
  }
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
  jboolean isCopy = 0;
  Brodger brodger = Brodger(width, height);

  img = (uint8_t*) env->GetByteArrayElements(im, &isCopy);
  brodger.brodge(img);
  img[ 0] = 1;    img[ 1] = 1;    img[10] = 1;
  env->ReleaseByteArrayElements(im, (jbyte *) img, JNI_COMMIT);
  printf("JNIBrodger done with isCopy = \"%u\"\n", isCopy);
/*
  env->GetByteArrayRegion(im, (jsize)0, (jsize)(3*width*height), (jbyte*)img);
  brodger.brodge(img);
  img[ 0] = 3;    img[ 1] = 3;    img[10] = 3;
  printf("JNIBrodger done with region\n");
*/  
}


/*
 * Class:     u640_CWrap
 * Method:    process
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_u640_CWrap_process__J(JNIEnv *env, jobject obj, jlong peer) {
    Brodger *brodger = toBrodger(peer);
    printf("CPPProcess: brodger %p | oscs %p | img %p | width %d\n", brodger, brodger->oscs, brodger->img, brodger->width);
    
    brodger->brodge();
    brodger->img[ 0] = 2;    brodger->img[ 1] = 2;    brodger->img[10] = 2;
}

/*
 * Class:     u640_CWrap
 * Method:    process
 * Signature: (JLjava/nio/ByteBuffer;)V
 */
JNIEXPORT void JNICALL Java_u640_CWrap_process__JLjava_nio_ByteBuffer_2(JNIEnv * env, jobject obj, jlong peer, jobject buf) {
  uint8_t *img;
  Brodger *brodger = toBrodger(peer);
  
  img = (uint8_t*) env->GetDirectBufferAddress(buf);
  brodger->brodge(img);
  img[ 0] = 4;    img[ 1] = 4;    img[10] = 4;
}

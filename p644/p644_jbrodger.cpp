/*
 * p644_jbrodger.cpp
 *
 * Date Oct 16, 2011
 * Author and copyright (C) 2011 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#include "p644_brodger.h"

#include <jni.h>
#include "u640_CWrap.h"

using namespace p644;


/*
 * Class:     u640_CWrap
 * Method:    brodger
 * Signature: ([BII)V
 */
JNIEXPORT void JNICALL Java_u640_CWrap_brodger (JNIEnv *env, jobject obj
, jbyteArray im, jint width, jint height) {
  uint8_t *img;
  Brodger brodger = Brodger(width, height);

  img = (uint8_t*) env->GetByteArrayElements(im, NULL);
  brodger.brodge(img);
}

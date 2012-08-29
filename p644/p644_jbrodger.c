/*
 * p644_jbrodger.c
 *
 * Date Oct 16, 2011
 * Author and copyright (C) 2011 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#include <stdint.h>

#include <jni.h>
#include "u640_CWrap.h"


typedef struct _point_ point;
typedef struct _osc_ osc;
typedef struct _brodge_ {
  int     width;
  int     height;
  int     num_osc;
      
  uint8_t *img;
  osc     *oscs;
} brodge;

extern int dist(point *p1, point *p2);
extern void coln3(osc *os, point *p, int *colors);
extern int  coln1(osc *os, point *p);
extern void osc_init(osc *os, int col, int x, int y, int pas, int mas, int dec, int dec_typ);
extern void brodger(brodge *b, uint8_t *img);
extern void brodger_init(brodge *b, int w, int h, int num_osc);
extern void brodger_exit(brodge *b);

/*
 * Class:     u640_CWrap
 * Method:    brodger
 * Signature: ([BII)V
 */
JNIEXPORT void JNICALL Java_u640_CWrap_brodger (JNIEnv *env, jobject obj
, jbyteArray im, jint width, jint height) {
  brodge b;
  uint8_t *img;
  brodger_init(&b, width, height, 3);

  img = (uint8_t*) env->GetByteArrayElements(im, NULL);
  brodger(&b, img);

  brodger_exit(&b);
}


/* DO NOT EDIT THIS FILE - it is machine generated */

#include <jni.h>

#ifndef __t508_u640_video_F642X264__
#define __t508_u640_video_F642X264__

#ifdef __cplusplus
extern "C"
{
#endif

JNIEXPORT jlong JNICALL Java_t508_u640_video_F642X264__1init (JNIEnv *env, jobject, jint, jint, jint, jfloat, jint, jint);
JNIEXPORT void JNICALL Java_t508_u640_video_F642X264__1free (JNIEnv *env, jobject, jlong);
JNIEXPORT jint JNICALL Java_t508_u640_video_F642X264__1open (JNIEnv *env, jobject, jlong, jstring, jint);
JNIEXPORT jint JNICALL Java_t508_u640_video_F642X264__1add_1frame__J_3B (JNIEnv *env, jobject, jlong, jbyteArray);
JNIEXPORT jint JNICALL Java_t508_u640_video_F642X264__1add_1frame__J_3I (JNIEnv *env, jobject, jlong, jintArray);
JNIEXPORT jint JNICALL Java_t508_u640_video_F642X264__1close (JNIEnv *env, jobject, jlong);
JNIEXPORT jint JNICALL Java_t508_u640_video_F642X264__1set_1qp__JI (JNIEnv *env, jobject, jlong, jint);
JNIEXPORT jint JNICALL Java_t508_u640_video_F642X264__1set_1qp__JIII (JNIEnv *env, jobject, jlong, jint, jint, jint);
JNIEXPORT jint JNICALL Java_t508_u640_video_F642X264__1set_1param (JNIEnv *env, jobject, jlong, jstring, jstring);
JNIEXPORT jint JNICALL Java_t508_u640_video_F642X264__1set_1nb_1threads (JNIEnv *env, jobject, jlong, jint);
JNIEXPORT void JNICALL Java_t508_u640_video_F642X264__1set_1log_1level (JNIEnv *env, jobject, jlong, jint);
JNIEXPORT void JNICALL Java_t508_u640_video_F642X264__1dump_1config (JNIEnv *env, jobject, jlong);

#undef t508_u640_video_F642X264_RAW_MUXER
#define t508_u640_video_F642X264_RAW_MUXER 0L
#undef t508_u640_video_F642X264_MKV_MUXER
#define t508_u640_video_F642X264_MKV_MUXER 1L
#undef t508_u640_video_F642X264_FLV_MUXER
#define t508_u640_video_F642X264_FLV_MUXER 2L

#ifdef __cplusplus
}
#endif

#endif /* __t508_u640_video_F642X264__ */
//
// Created by catalin on 15.07.2017.
//
#include <jni.h>
#include <cstdlib>

#ifndef RUBIKDETECTORDEMO_RUBIKDETECTORJNI_HPP
#define RUBIKDETECTORDEMO_RUBIKDETECTORJNI_HPP

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jlong JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_createNativeObject(JNIEnv *env,
                                                                      jobject instance,
                                                                      jstring storagePath);

JNIEXPORT void JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeReleaseCubeDetector(JNIEnv *env,
                                                                             jobject instance,
                                                                             jlong cubeDetectorHandle);

JNIEXPORT void JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeSetDebuggable(JNIEnv *env,
                                                                       jobject instance,
                                                                       jlong cubeDetectorHandle,
                                                                       jboolean debuggable);

JNIEXPORT void JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeSetDrawFoundFacelets(JNIEnv *env,
                                                                       jobject instance,
                                                                       jlong cubeDetectorHandle,
                                                                       jboolean shouldDrawFoundFacelets);

JNIEXPORT void JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_findCubeNativeImageData(JNIEnv *env,
                                                                           jobject instance,
                                                                           jlong cubeDetectorHandle,
                                                                           jbyteArray imageByteData);

JNIEXPORT void JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeSetImageDimensions(JNIEnv *env,
                                                                            jobject instance,
                                                                            jlong cubeDetectorHandle,
                                                                            jint width,
                                                                            jint height);

JNIEXPORT jint JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeGetTotalRequiredMemory(JNIEnv *env,
                                                                                jobject instance,
                                                                                jlong cubeDetectorHandle);

JNIEXPORT jint JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeGetRgbaImageOffset(JNIEnv *env,
                                                                            jobject instance,
                                                                            jlong cubeDetectorHandle);

JNIEXPORT jint JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeGetRgbaImageSize(JNIEnv *env,
                                                                          jobject instance,
                                                                          jlong cubeDetectorHandle);

JNIEXPORT jint JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeGetNv21ImageSize(JNIEnv *env,
                                                                          jobject instance,
                                                                          jlong cubeDetectorHandle);

JNIEXPORT jint JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeGetNv21ImageOffset(JNIEnv *env,
                                                                            jobject instance,
                                                                            jlong cubeDetectorHandle);

#ifdef __cplusplus
}
#endif

#endif //RUBIKDETECTORDEMO_RUBIKDETECTORJNI_HPP

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
Java_com_catalinjurjiu_rubikdetector_RubikDetector_findCubeNativeMatAddr(JNIEnv *env,
                                                                         jobject instance,
                                                                         jlong cubeDetectorHandle,
                                                                         jlong addrRgba);
JNIEXPORT jbyteArray JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_findCubeNativeImageData(JNIEnv *env,
                                                                           jobject instance,
                                                                           jlong cubeDetectorHandle,
                                                                           jbyteArray imageByteData,
                                                                           jint width,
                                                                           jint height);

#ifdef __cplusplus
}
#endif

#endif //RUBIKDETECTORDEMO_RUBIKDETECTORJNI_HPP

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

JNIEXPORT void JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_findCubeNative(JNIEnv *env, jobject instance,
                                                                  jlong addrRgba);
JNIEXPORT jbyteArray JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_findCubeNative2(JNIEnv *env, jobject instance,
                                                                   jbyteArray imageData_,
                                                                   jint width, jint height);

#ifdef __cplusplus
}
#endif

#endif //RUBIKDETECTORDEMO_RUBIKDETECTORJNI_HPP

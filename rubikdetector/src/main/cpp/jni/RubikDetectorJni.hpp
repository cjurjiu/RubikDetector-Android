//
// Created by catalin on 15.07.2017.
//
#include <jni.h>

#ifndef RUBIKDETECTORDEMO_RUBIKDETECTORJNI_HPP
#define RUBIKDETECTORDEMO_RUBIKDETECTORJNI_HPP


#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_findCubeNative(JNIEnv *env, jobject instance,
                                                                  jlong addrRgba);
#ifdef __cplusplus
}
#endif

#endif //RUBIKDETECTORDEMO_RUBIKDETECTORJNI_HPP

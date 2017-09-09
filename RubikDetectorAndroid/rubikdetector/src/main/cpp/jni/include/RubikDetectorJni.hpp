//
// Created by catalin on 15.07.2017.
//
#include <jni.h>
#include <cstdlib>
#include "../../rubikdetectorcore/include/rubikdetector/data/processing/RubikFacelet.hpp"

#ifndef RUBIKDETECTORJNI_RUBIKDETECTORJNI_HPP
#define RUBIKDETECTORJNI_RUBIKDETECTORJNI_HPP

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jlong JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeCreateRubikDetector(JNIEnv *env,
                                                                             jobject instance,
                                                                             jint frameWidth,
                                                                             jint frameHeight,
                                                                             jint inputImageFormat,
                                                                             jint drawMode,
                                                                             jint strokeWidth,
                                                                             jboolean fillShape,
                                                                             jstring storagePath_);

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

JNIEXPORT jintArray JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeFindCubeImageData(JNIEnv *env,
                                                                           jobject instance,
                                                                           jlong cubeDetectorHandle,
                                                                           jbyteArray imageByteData);

JNIEXPORT jintArray JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeFindCubeImageDataBuffer(JNIEnv *env,
                                                                                 jobject instance,
                                                                                 jlong cubeDetectorHandle,
                                                                                 jobject imageDataDirectBuffer);
JNIEXPORT void JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeOverrideInputFrameWithResultFrame(
        JNIEnv *env,
        jobject instance,
        jlong cubeDetectorHandle,
        jbyteArray imageByteData);

JNIEXPORT void JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeSetImageProperties(JNIEnv *env,
                                                                            jobject instance,
                                                                            jlong cubeDetectorHandle,
                                                                            jint width,
                                                                            jint height,
                                                                            jint imageFormat);

JNIEXPORT jint JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeGetRequiredMemory(JNIEnv *env,
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

jintArray processResult(const std::vector<std::vector<rbdt::RubikFacelet>> &result, _JNIEnv *env);

#endif //RUBIKDETECTORJNI_RUBIKDETECTORJNI_HPP

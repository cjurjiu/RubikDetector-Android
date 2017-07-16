//
// Created by catalin on 15.07.2017.
//
#include <jni.h>
#include "RubikDetectorJni.hpp"
#include "../detector/CubeDetector.hpp"
#include "OnCubeDetectionJniBridgeListener.hpp"

JNIEXPORT void JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_findCubeNative(JNIEnv *env, jobject instance,
                                                                  jlong addrRgba) {

    CubeDetector &cubeDetector = *new CubeDetector();

    cubeDetector.setOnCubeDetectionResultListener(
            *new OnCubeDetectionJniBridgeListener(env, instance));
    cubeDetector.findCube(*(cv::Mat *) addrRgba);
}


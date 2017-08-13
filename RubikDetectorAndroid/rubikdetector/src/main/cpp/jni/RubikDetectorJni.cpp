//
// Created by catalin on 15.07.2017.
//
#include <jni.h>
#include <vector>
#include "../rubikdetectorcore/helpers/ImageSaver.hpp"
#include "../rubikdetectorcore/detectors/cubedetector/CubeDetector.hpp"
#include "RubikDetectorJni.hpp"
#include "OnCubeDetectionJniBridgeListener.hpp"
#include "../rubikdetectorcore/utils/CrossLog.hpp"
#include <limits>

JNIEXPORT jlong JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_createNativeObject(JNIEnv *env,
                                                                      jobject instance,
                                                                      jstring storagePath) {
    std::shared_ptr<ImageSaver> imageSaver;
    if (storagePath != NULL) {
        const char *cppStoragePath = env->GetStringUTFChars(storagePath, 0);
        imageSaver = std::make_shared<ImageSaver>(cppStoragePath);
        env->ReleaseStringUTFChars(storagePath, cppStoragePath);
    } else {
        imageSaver = nullptr;
    }

    CubeDetector &cubeDetector = *new CubeDetector(imageSaver);
    cubeDetector.setOnCubeDetectionResultListener(
            new OnCubeDetectionJniBridgeListener(env, instance));
    return reinterpret_cast<jlong>(&cubeDetector);

}

JNIEXPORT void JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeReleaseCubeDetector(JNIEnv *env,
                                                                             jobject instance,
                                                                             jlong cubeDetectorHandle) {

    CubeDetector &cubeDetector = *reinterpret_cast<CubeDetector *>(cubeDetectorHandle);
    if (cubeDetector.isDebuggable()) {
        LOG_DEBUG("RUBIK_JNI_PART.cpp", "nativeReleaseCube");
    }
    delete &cubeDetector;
}

JNIEXPORT void JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeSetDebuggable(JNIEnv *env,
                                                                       jobject instance,
                                                                       jlong cubeDetectorHandle,
                                                                       jboolean debuggable) {
    CubeDetector &cubeDetector = *reinterpret_cast<CubeDetector *>(cubeDetectorHandle);
    cubeDetector.setDebuggable(static_cast<bool>(debuggable));
}

JNIEXPORT void JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_findCubeNativeImageData(JNIEnv *env,
                                                                           jobject instance,
                                                                           jlong cubeDetectorHandle,
                                                                           jbyteArray imageByteData,
                                                                           jint width,
                                                                           jint height) {
    CubeDetector &cubeDetector = *reinterpret_cast<CubeDetector *>(cubeDetectorHandle);

    jsize length = env->GetArrayLength(imageByteData);
    jboolean isCopy = 3;
    void *ptr = env->GetPrimitiveArrayCritical(imageByteData, &isCopy);

    if (ptr) {
        uint8_t *ptrAsInt = reinterpret_cast<uint8_t *>(ptr);
        env->ReleasePrimitiveArrayCritical(imageByteData, ptr, JNI_ABORT);
        cubeDetector.findCube(ptrAsInt, length, (int) width, (int) height);
    } else {
        LOG_WARN("RUBIK_JNI_PART.cpp",
                 "Could not obtain image byte array. No processing performed.");
    }
}
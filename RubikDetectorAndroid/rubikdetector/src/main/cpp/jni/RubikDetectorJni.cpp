//
// Created by catalin on 15.07.2017.
//
#include <jni.h>
#include <vector>
#include <assert.h>
#include "../rubikdetectorcore/helpers/ImageSaver.hpp"
#include "../rubikdetectorcore/detectors/cubedetector/CubeDetector.hpp"
#include "RubikDetectorJni.hpp"
#include "OnCubeDetectionJniBridgeListener.hpp"
#include "../rubikdetectorcore/utils/CrossLog.hpp"
#include <limits>

struct Binary {
    using CppType = std::vector<uint8_t>;
    using JniType = jbyteArray;

    using Boxed = Binary;

    static CppType toCpp(JNIEnv *jniEnv, JniType j) {
        assert(j != nullptr);

        std::vector<uint8_t> ret;
        jsize length = jniEnv->GetArrayLength(j);

        if (!length) {
            return ret;
        }

        {
            auto deleter = [jniEnv, j](void *c) {
                if (c) {
                    jniEnv->ReleasePrimitiveArrayCritical(j, c, JNI_ABORT);
                }
            };

            std::unique_ptr<uint8_t, decltype(deleter)> ptr(
                    reinterpret_cast<uint8_t *>(jniEnv->GetPrimitiveArrayCritical(j, nullptr)),
                    deleter
            );

            if (ptr) {
                // Construct and then move-assign. This copies the elements only once,
                // and avoids having to initialize before filling (as with resize())
                ret = std::vector<uint8_t>{ptr.get(), ptr.get() + length};
            } else {
                // Something failed...
            }
        }

        return ret;
    }
};

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

    //TODO check leaks
    CubeDetector &cubeDetector = *new CubeDetector(imageSaver);
    cubeDetector.setOnCubeDetectionResultListener(
            *new OnCubeDetectionJniBridgeListener(env, instance));
    return reinterpret_cast<jlong>(&cubeDetector);

}

JNIEXPORT void JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeReleaseCubeDetector(JNIEnv *env,
                                                                             jobject instance,
                                                                             jlong cubeDetectorHandle) {
    LOG_DEBUG("RUBIK_JNI_PART.cpp", "nativeReleaseCube");
    CubeDetector &cubeDetector = *reinterpret_cast<CubeDetector *>(cubeDetectorHandle);
    LOG_DEBUG("RUBIK_JNI_PART.cpp", "before delete");
    delete &cubeDetector;
    LOG_DEBUG("RUBIK_JNI_PART.cpp", "after delete");
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
Java_com_catalinjurjiu_rubikdetector_RubikDetector_findCubeNativeMatAddr(JNIEnv *env,
                                                                         jobject instance,
                                                                         jlong cubeDetectorHandle,
                                                                         jlong addrRgba) {

    CubeDetector &cubeDetector = *reinterpret_cast<CubeDetector *>(cubeDetectorHandle);
    cubeDetector.findCube(*(cv::Mat *) addrRgba);
}

JNIEXPORT jbyteArray JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_findCubeNativeImageData(JNIEnv *env,
                                                                           jobject instance,
                                                                           jlong cubeDetectorHandle,
                                                                           jbyteArray imageByteData,
                                                                           jint width,
                                                                           jint height) {
    //TODO handle leaks
    CubeDetector &cubeDetector = *reinterpret_cast<CubeDetector *>(cubeDetectorHandle);

    std::vector<uint8_t> image = ::Binary::toCpp(env, imageByteData);
    std::vector<uint8_t> abc = cubeDetector.findCube(image, (int) width, (int) height);
    jbyteArray arr = env->NewByteArray(abc.size());
    env->SetByteArrayRegion(arr, 0, abc.size(), (jbyte *) abc.data());
    return arr;
}
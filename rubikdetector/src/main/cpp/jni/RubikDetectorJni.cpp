//
// Created by catalin on 15.07.2017.
//
#include <jni.h>
#include <vector>
#include <assert.h>
#include "RubikDetectorJni.hpp"
#include "../detectors/cubedetector/CubeDetector.hpp"
#include "OnCubeDetectionJniBridgeListener.hpp"
#include <memory>
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

JNIEXPORT void JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_findCubeNative(JNIEnv *env, jobject instance,
                                                                  jlong addrRgba) {

    CubeDetector &cubeDetector = *new CubeDetector();

    cubeDetector.setOnCubeDetectionResultListener(
            *new OnCubeDetectionJniBridgeListener(env, instance));
    cubeDetector.findCube(*(cv::Mat *) addrRgba);
}

JNIEXPORT jbyteArray JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_findCubeNative2(JNIEnv *env, jobject instance,
                                                                   jbyteArray imageData_,
                                                                   jint width, jint height) {
    CubeDetector &cubeDetector = *new CubeDetector();

    cubeDetector.setOnCubeDetectionResultListener(
            *new OnCubeDetectionJniBridgeListener(env, instance));

    std::vector<uint8_t> image = ::Binary::toCpp(env, imageData_);

    std::vector<uint8_t> abc = cubeDetector.findCube2(image, (int) width, (int) height);

    jbyteArray arr = env->NewByteArray(abc.size());
    env->SetByteArrayRegion(arr, 0, abc.size(), (jbyte *) abc.data());
    return arr;

}
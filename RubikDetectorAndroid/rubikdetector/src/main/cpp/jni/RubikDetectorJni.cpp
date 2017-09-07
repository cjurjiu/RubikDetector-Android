//
// Created by catalin on 15.07.2017.
//
#include <jni.h>
#include <vector>
#include "../rubikdetectorcore/imagesaver/ImageSaver.hpp"
#include "RubikDetectorJni.hpp"
#include "OnCubeDetectionJniBridgeListener.hpp"
#include "../rubikdetectorcore/utils/CrossLog.hpp"
#include "../rubikdetectorcore/utils/Utils.hpp"
#include "../rubikdetectorcore/data/ImageProperties.hpp"
#include "../rubikdetectorcore/detectors/rubikdetector/builder/RubikDetectorBuilder.hpp"

JNIEXPORT jlong JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeCreateRubikDetector(JNIEnv *env,
                                                                             jobject instance,
                                                                             jint frameWidth,
                                                                             jint frameHeight,
                                                                             jint inputImageFormat,
                                                                             jint drawMode,
                                                                             jint strokeWidth,
                                                                             jboolean fillShape,
                                                                             jstring storagePath) {
    std::shared_ptr<ImageSaver> imageSaver;
    if (storagePath != NULL) {
        const char *cppStoragePath = env->GetStringUTFChars(storagePath, 0);
        imageSaver = std::make_shared<ImageSaver>(cppStoragePath);
        env->ReleaseStringUTFChars(storagePath, cppStoragePath);
    } else {
        imageSaver = nullptr;
    }

    RubikDetector *rubikDetector = RubikDetectorBuilder()
            .inputFrameSize((int) frameWidth, (int) frameHeight)
            .inputFrameFormat(utils::imageFormatFromInt(inputImageFormat))
            .drawConfig(DrawConfig(utils::drawModeFromInt((int) drawMode),
                                   (int) strokeWidth,
                                   (bool) fillShape))
            .imageSaver(imageSaver)
            .build();
    return reinterpret_cast<jlong>(rubikDetector);

}

JNIEXPORT void JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeReleaseCubeDetector(JNIEnv *env,
                                                                             jobject instance,
                                                                             jlong cubeDetectorHandle) {

    RubikDetector &cubeDetector = *reinterpret_cast<RubikDetector *>(cubeDetectorHandle);
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
    RubikDetector &cubeDetector = *reinterpret_cast<RubikDetector *>(cubeDetectorHandle);
    cubeDetector.setDebuggable(static_cast<bool>(debuggable));
}

JNIEXPORT void JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeSetDrawFoundFacelets(JNIEnv *env,
                                                                              jobject instance,
                                                                              jlong cubeDetectorHandle,
                                                                              jint drawMode,
                                                                              jint strokeWidth,
                                                                              jboolean fillShape) {
    RubikDetector &cubeDetector = *reinterpret_cast<RubikDetector *>(cubeDetectorHandle);
    cubeDetector.updateDrawConfig(DrawConfig(utils::drawModeFromInt((int) drawMode),
                                             (int) strokeWidth,
                                             (bool) fillShape));
}

JNIEXPORT jintArray  JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeFindCubeImageData(JNIEnv *env,
                                                                           jobject instance,
                                                                           jlong cubeDetectorHandle,
                                                                           jbyteArray imageByteData) {
    RubikDetector &cubeDetector = *reinterpret_cast<RubikDetector *>(cubeDetectorHandle);

    jboolean isCopy = 3; //some arbitrary value
    void *ptr = env->GetPrimitiveArrayCritical(imageByteData, &isCopy);

    if (ptr) {
        uint8_t *ptrAsInt = reinterpret_cast<uint8_t *>(ptr);
        env->ReleasePrimitiveArrayCritical(imageByteData, ptr, JNI_ABORT);
        std::vector<std::vector<RubikFacelet>> result = cubeDetector.process(ptrAsInt);
        return processResult(result, env);
    } else {
        LOG_WARN("RUBIK_JNI_PART.cpp",
                 "Could not obtain image byte array. No processing performed.");
        return NULL;
    }
}

JNIEXPORT jintArray JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeFindCubeImageDataBuffer(JNIEnv *env,
                                                                                 jobject instance,
                                                                                 jlong cubeDetectorHandle,
                                                                                 jobject imageDataDirectBuffer) {
    RubikDetector &cubeDetector = *reinterpret_cast<RubikDetector *>(cubeDetectorHandle);

    void *ptr = env->GetDirectBufferAddress(imageDataDirectBuffer);
    if (ptr) {
        uint8_t *ptrAsInt = reinterpret_cast<uint8_t *>(ptr);
        std::vector<std::vector<RubikFacelet>> result = cubeDetector.process(ptrAsInt);
        return processResult(result, env);
    } else {
        LOG_WARN("RUBIK_JNI_PART.cpp",
                 "Could not obtain image byte array. No processing performed.");
        return NULL;
    }
}


JNIEXPORT void JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeOverrideInputFrameWithResultFrame(
        JNIEnv *env,
        jobject instance,
        jlong cubeDetectorHandle,
        jbyteArray imageByteData) {

    RubikDetector &cubeDetector = *reinterpret_cast<RubikDetector *>(cubeDetectorHandle);

    jboolean isCopy = 3;
    void *ptr = env->GetPrimitiveArrayCritical(imageByteData, &isCopy);
    if (ptr) {
        uint8_t *ptrAsInt = reinterpret_cast<uint8_t *>(ptr);
        env->ReleasePrimitiveArrayCritical(imageByteData, ptr, JNI_ABORT);
        cubeDetector.overrideInputFrameWithResultFrame(ptrAsInt);
    } else {
        LOG_WARN("RUBIK_JNI_PART.cpp",
                 "Could not obtain image byte array. No processing performed.");
    }
}

JNIEXPORT void JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeSetImageProperties(JNIEnv *env,
                                                                            jobject instance,
                                                                            jlong cubeDetectorHandle,
                                                                            jint width,
                                                                            jint height,
                                                                            jint imageFormat) {
    RubikDetector &cubeDetector = *reinterpret_cast<RubikDetector *>(cubeDetectorHandle);
    cubeDetector.updateImageProperties(ImageProperties((int) width, (int) height,
                                                       utils::imageFormatFromInt(imageFormat)));
}

JNIEXPORT jint JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeGetRequiredMemory(JNIEnv *env,
                                                                           jobject instance,
                                                                           jlong cubeDetectorHandle) {
    RubikDetector &cubeDetector = *reinterpret_cast<RubikDetector *>(cubeDetectorHandle);
    return cubeDetector.getRequiredMemory();
}

JNIEXPORT jint JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeGetRgbaImageOffset(JNIEnv *env,
                                                                            jobject instance,
                                                                            jlong cubeDetectorHandle) {
    RubikDetector &cubeDetector = *reinterpret_cast<RubikDetector *>(cubeDetectorHandle);
    return cubeDetector.getOutputFrameBufferOffset();
}

JNIEXPORT jint JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeGetRgbaImageSize(JNIEnv *env,
                                                                          jobject instance,
                                                                          jlong cubeDetectorHandle) {
    RubikDetector &cubeDetector = *reinterpret_cast<RubikDetector *>(cubeDetectorHandle);
    return cubeDetector.getOutputFrameByteCount();
}

JNIEXPORT jint JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeGetNv21ImageSize(JNIEnv *env,
                                                                          jobject instance,
                                                                          jlong cubeDetectorHandle) {
    RubikDetector &cubeDetector = *reinterpret_cast<RubikDetector *>(cubeDetectorHandle);
    return cubeDetector.getInputFrameByteCount();
}

JNIEXPORT jint JNICALL
Java_com_catalinjurjiu_rubikdetector_RubikDetector_nativeGetNv21ImageOffset(JNIEnv *env,
                                                                            jobject instance,
                                                                            jlong cubeDetectorHandle) {
    RubikDetector &cubeDetector = *reinterpret_cast<RubikDetector *>(cubeDetectorHandle);
    return cubeDetector.getInputFrameBufferOffset();
}

jintArray processResult(const std::vector<std::vector<RubikFacelet>> &result, _JNIEnv *env) {
    if (result.size() != 0) {
        size_t data_size = 9 * 6;
        jint flattenedResult[data_size];
        jint *currentPos = flattenedResult;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                *(currentPos++) = utils::asInt(result[i][j].color);
                *(currentPos++) = (int) (result[i][j].center.x * 100000);
                *(currentPos++) = (int) (result[i][j].center.y * 100000);
                *(currentPos++) = (int) (result[i][j].width * 100000);
                *(currentPos++) = (int) (result[i][j].height * 100000);
                *(currentPos++) = (int) (result[i][j].angle * 100000);
            }
        }

        jintArray retArray = env->NewIntArray(data_size);
        env->SetIntArrayRegion(retArray, 0, data_size, (jint *) flattenedResult);
        return retArray;
    } else {
        return NULL;
    }
}
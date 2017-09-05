//
// Created by catalin on 15.07.2017.
//

#include <stdlib.h>
#include "OnCubeDetectionJniBridgeListener.hpp"
#include "../rubikdetectorcore/utils/CrossLog.hpp"
#include "../rubikdetectorcore/data/RubikFacelet.hpp"

OnCubeDetectionJniBridgeListener::OnCubeDetectionJniBridgeListener(JNIEnv *jniEnv,
                                                                   jobject jobjc) {
    LOG_DEBUG("RubikJniPart.cpp", "OnCubeDetectionJniBridgeListener c-tor.");

    jniEnv->GetJavaVM(&javaVM);
    globalJobj = jniEnv->NewGlobalRef(jobjc);
    jclass localClass = jniEnv->FindClass("com/catalinjurjiu/rubikdetector/RubikDetector");
    javaCallbackMethodId = jniEnv->GetMethodID(localClass, "onFacetColorsDetected", "([I)V");

    LOG_DEBUG("RubikJniPart.cpp",
              "OnCubeDetectionJniBridgeListener c-tor end. exceptionCheck: %d",
              jniEnv->ExceptionCheck() == JNI_TRUE);
}

OnCubeDetectionJniBridgeListener::~OnCubeDetectionJniBridgeListener() {
    LOG_DEBUG("RubikJniPart.cpp",
              "OnCubeDetectionJniBridgeListener destructor.");
    JNIEnv *currentEnvironment;
    int status = javaVM->GetEnv((void **) &currentEnvironment, JNI_VERSION_1_6);
    if (status == JNI_OK) {
        currentEnvironment->DeleteGlobalRef(globalJobj);
    } else {
        LOG_WARN("RubikJniPart.cpp",
                 "Error when trying to obtain the JniEnvironment. Error code: %d. See jni.h for value meaning(e.g. JNI_EDETACHED equals -2).",
                 status);
    }
}

void OnCubeDetectionJniBridgeListener::onCubeDetectionResult(
        const std::vector<std::vector<RubikFacelet>> &result) const {
    LOG_DEBUG("RubikJniPart.cpp",
              "OnCubeDetectionJniBridgeListener -> notifying java side of result.");
    JNIEnv *currentEnvironment;
    int status = javaVM->GetEnv((void **) &currentEnvironment, JNI_VERSION_1_6);
    if (status == JNI_OK) {
        //flatten the 2D color array, in order to send it over JNI
        size_t data_size = 9 * 6;
        jint flattenedResult[data_size];
        jint *currentPos = flattenedResult;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                *(currentPos++) = result[i][j].color;
                *(currentPos++) = (int) (result[i][j].center.x * 100000);
                *(currentPos++) = (int) (result[i][j].center.y * 100000);
                *(currentPos++) = (int) (result[i][j].width * 100000);
                *(currentPos++) = (int) (result[i][j].height * 100000);
                *(currentPos++) = (int) (result[i][j].angle * 100000);
            }
        }

        jintArray retArray = currentEnvironment->NewIntArray(data_size);
        currentEnvironment->SetIntArrayRegion(retArray, 0, data_size, (jint *) flattenedResult);
        currentEnvironment->CallVoidMethod(globalJobj, javaCallbackMethodId, retArray);
    } else {
        LOG_WARN("RubikJniPart.cpp",
                 "Error when trying to obtain the JniEnvironment. Error code: %d. See jni.h for value meaning(e.g. JNI_EDETACHED equals -2).",
                 status);
    }
}

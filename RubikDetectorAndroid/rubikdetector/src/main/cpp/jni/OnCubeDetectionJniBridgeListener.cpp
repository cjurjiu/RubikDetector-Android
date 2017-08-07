//
// Created by catalin on 15.07.2017.
//

#include <stdlib.h>
#include "OnCubeDetectionJniBridgeListener.hpp"
#include "../rubikdetectorcore/utils/CrossLog.hpp"

OnCubeDetectionJniBridgeListener::OnCubeDetectionJniBridgeListener(JNIEnv *jniEnv, jobject jobjc) {
    LOG_DEBUG("CATALNER", "OnCubeDetectionJniBridgeListener c-tor!");

    jniEnv->GetJavaVM(&javaVM);
    globalJobj = jniEnv->NewGlobalRef(jobjc);
    jclass localClass = jniEnv->FindClass("com/catalinjurjiu/rubikdetector/RubikDetector");
    javaCallbackMethodId = jniEnv->GetMethodID(localClass, "onFacetColorsDetected", "([I)V");
    LOG_DEBUG("CATALNER", "OnCubeDetectionJniBridgeListener c-tor end! exceptionCheck: %d",
              jniEnv->ExceptionCheck() == JNI_TRUE);
}

OnCubeDetectionJniBridgeListener::~OnCubeDetectionJniBridgeListener() {
    JNIEnv *currentEnvironment;
    int getEnvStat = javaVM->GetEnv((void **) &currentEnvironment, JNI_VERSION_1_6);
    if (getEnvStat == JNI_OK) {
        LOG_DEBUG("CATALNER", "ATTACHED");
        LOG_DEBUG("CATALNER", "before DeleteGlobalRef!");
        currentEnvironment->DeleteGlobalRef(globalJobj);
        LOG_DEBUG("CATALNER", "after DeleteGlobalRef!");
    } else if (getEnvStat == JNI_EDETACHED) {
        LOG_DEBUG("CATALNER", "JNI_EDETACHED, do nothing.");
    } else if (getEnvStat == JNI_EVERSION) {
        LOG_DEBUG("CATALNER", "ERROR JNI VERSION, do nothing!");
    }
}

void OnCubeDetectionJniBridgeListener::onCubeDetectionResult(
        const std::vector<std::vector<int>> result) const {
    JNIEnv *currentEnvironment;
    int getEnvStat = javaVM->GetEnv((void **) &currentEnvironment, JNI_VERSION_1_6);
    if (getEnvStat == JNI_OK) {
        LOG_DEBUG("CATALNER", "ATTACHED");

        //flatten the 2D color array, in order to send it over JNI
        jint flattenedResult[9];
        size_t data_size = 9;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                flattenedResult[i * 3 + j] = (jint) result[i][j];
            }
        }

        jintArray retArray = currentEnvironment->NewIntArray(data_size);
        currentEnvironment->SetIntArrayRegion(retArray, 0, data_size, (jint *) flattenedResult);
        LOG_DEBUG("CATALNER", "before call void method!");
        currentEnvironment->CallVoidMethod(globalJobj, javaCallbackMethodId, retArray);
        LOG_DEBUG("CATALNER", "after call void method!");

    } else if (getEnvStat == JNI_EDETACHED) {
        LOG_DEBUG("CATALNER", "JNI_EDETACHED, do nothing.");
    } else if (getEnvStat == JNI_EVERSION) {
        LOG_DEBUG("CATALNER", "ERROR JNI VERSION, do nothing!");
    }
}


//
// Created by catalin on 15.07.2017.
//

#include <stdlib.h>
#include "OnCubeDetectionJniBridgeListener.hpp"


OnCubeDetectionJniBridgeListener::OnCubeDetectionJniBridgeListener(JNIEnv *jniEnv, jobject jobjc) {
    env = jniEnv;
    jobj = jobjc;
}

OnCubeDetectionJniBridgeListener::~OnCubeDetectionJniBridgeListener() {
    env = 0;
    jobj = 0;
}

void OnCubeDetectionJniBridgeListener::onCubeDetectionResult(
        const std::vector<std::vector<int>> result) {
    //flatten the 2D color array, in order to send it over JNI
    jint flattenedResult[9];
    size_t data_size = 9;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            flattenedResult[i * 3 + j] = (jint) result[i][j];
        }
    }

    jclass localClass = env->FindClass("com/catalinjurjiu/rubikdetector/RubikDetector");
    jclass clazz = reinterpret_cast<jclass>(env->NewGlobalRef(localClass));
    jmethodID messageMe = env->GetMethodID(clazz, "onFacetColorsDetected", "([I)V");
    jintArray retArray = env->NewIntArray(data_size);
    env->SetIntArrayRegion(retArray, 0, data_size, (jint *) flattenedResult);
    env->CallVoidMethod(jobj, messageMe, retArray);
}


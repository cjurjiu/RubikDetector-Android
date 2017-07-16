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

void OnCubeDetectionJniBridgeListener::onCubeDetectionResult(const int (&result)[9]) {
    //setup the data to be sent to the java part
    jint buffer[9];
    size_t data_size = 9;
    for (int i = 0; i < data_size; i++) {
        buffer[i] = (jint) result[i]; // put whatever logic you want to populate the values here.
    }

    jclass localClass = env->FindClass(
            "com/catalinjurjiu/rubikdetector/RubikDetector");
    jclass clazz = reinterpret_cast<jclass>(env->NewGlobalRef(localClass));
    jmethodID messageMe = env->GetMethodID(clazz, "onFacetColorsDetected", "([I)V");
    jintArray retArray = env->NewIntArray(data_size);
    env->SetIntArrayRegion(retArray, 0, data_size, (jint *) buffer);
    env->CallVoidMethod(jobj, messageMe, retArray);
}


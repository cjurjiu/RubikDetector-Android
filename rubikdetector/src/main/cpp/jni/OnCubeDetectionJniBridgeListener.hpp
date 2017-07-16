//
// Created by catalin on 15.07.2017.
//

#ifndef RUBIKDETECTORDEMO_ONCUBEDETECTIONJNIBRIDGELISTENER_HPP
#define RUBIKDETECTORDEMO_ONCUBEDETECTIONJNIBRIDGELISTENER_HPP


#include <jni.h>
#include "../detector/OnCubeDetectionResultListener.hpp"

class OnCubeDetectionJniBridgeListener : public OnCubeDetectionResultListener {

public:
    OnCubeDetectionJniBridgeListener(JNIEnv *jniEnv, jobject jobjc);

    ~OnCubeDetectionJniBridgeListener();

    void onCubeDetectionResult(const int (&result)[3][3]);

private:
    JNIEnv *env;
    jobject jobj;
};


#endif //RUBIKDETECTORDEMO_ONCUBEDETECTIONJNIBRIDGELISTENER_HPP

//
// Created by catalin on 15.07.2017.
//

#ifndef RUBIKDETECTORDEMO_ONCUBEDETECTIONJNIBRIDGELISTENER_HPP
#define RUBIKDETECTORDEMO_ONCUBEDETECTIONJNIBRIDGELISTENER_HPP


#include <jni.h>
#include <vector>
#include "../detectors/cubedetector/OnCubeDetectionResultListener.hpp"

class OnCubeDetectionJniBridgeListener : public OnCubeDetectionResultListener {

public:
    OnCubeDetectionJniBridgeListener(JNIEnv *jniEnv, jobject jobjc);

    ~OnCubeDetectionJniBridgeListener();

    void onCubeDetectionResult(const std::vector<std::vector<int>> result);

private:
    JNIEnv *env;
    jobject jobj;
};


#endif //RUBIKDETECTORDEMO_ONCUBEDETECTIONJNIBRIDGELISTENER_HPP

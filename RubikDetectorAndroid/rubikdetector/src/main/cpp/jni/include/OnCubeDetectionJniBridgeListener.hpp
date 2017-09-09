//
// Created by catalin on 15.07.2017.
//

#ifndef RUBIKDETECTORJNI_ONCUBEDETECTIONJNIBRIDGELISTENER_HPP
#define RUBIKDETECTORJNI_ONCUBEDETECTIONJNIBRIDGELISTENER_HPP


#include <jni.h>
#include <vector>
#include "../../rubikdetectorcore/include/rubikdetector/detectors/faceletsdetector/public/OnCubeDetectionResultListener.hpp"

class RubikFacelet;

class OnCubeDetectionJniBridgeListener : public OnCubeDetectionResultListener {

public:
    OnCubeDetectionJniBridgeListener(JNIEnv *jniEnv, jobject jobjc);

    ~OnCubeDetectionJniBridgeListener();

    void onCubeDetectionResult(const std::vector<std::vector<RubikFacelet>> &result) const override;

private:
    JavaVM *javaVM;
    jobject globalJobj;
    jmethodID javaCallbackMethodId;
};


#endif //RUBIKDETECTORJNI_ONCUBEDETECTIONJNIBRIDGELISTENER_HPP
//
// Created by catalin on 12.07.2017.
//

#include "CubeDetector.hpp"
#include "CubeDetectorBehavior.hpp"

CubeDetector::CubeDetector() : CubeDetector(nullptr) {}

CubeDetector::CubeDetector(ImageSaver *imageSaver)
        : behavior(new CubeDetectorBehavior(imageSaver)) {}


CubeDetector::~CubeDetector() {
    delete behavior;
}

void CubeDetector::setOnCubeDetectionResultListener(OnCubeDetectionResultListener &listener) {
    behavior->setOnCubeDetectionResultListener(listener);
}

void CubeDetector::findCube(cv::Mat &mat) {
    behavior->findCube(mat);
}

std::vector<uchar>
CubeDetector::findCube(const std::vector<uint8_t> &imageData, int width, int height) {
    return behavior->findCube(imageData, width, height);
}

void CubeDetector::setDebuggable(const bool debuggable) {
    behavior->setDebuggable(debuggable);
}

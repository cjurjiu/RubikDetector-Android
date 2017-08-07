//
// Created by catalin on 12.07.2017.
//

#include "CubeDetector.hpp"
#include "CubeDetectorBehavior.hpp"
#include "../../utils/CrossLog.hpp"

CubeDetector::CubeDetector() : CubeDetector(nullptr) {}

CubeDetector::CubeDetector(const std::shared_ptr<ImageSaver> imageSaver)
        : behavior(std::unique_ptr<CubeDetectorBehavior>(new CubeDetectorBehavior(imageSaver))) {}

CubeDetector::~CubeDetector() {
    if (behavior->isDebuggable()) {
        LOG_DEBUG("RubikJniPart.cpp", "CubeDetector - destructor.");
    }
}

void CubeDetector::setOnCubeDetectionResultListener(OnCubeDetectionResultListener *listener) {
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

bool CubeDetector::isDebuggable() {
    return behavior->isDebuggable();
}

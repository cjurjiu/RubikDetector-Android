//
// Created by catalin on 12.07.2017.
//

#include "CubeDetector.hpp"
#include "CubeDetectorBehavior.hpp"

CubeDetector::CubeDetector() {
    behavior = new CubeDetectorBehavior();
}

CubeDetector::~CubeDetector() {
    delete &behavior;
}

void CubeDetector::setOnCubeDetectionResultListener(OnCubeDetectionResultListener &listener) {
    behavior->setOnCubeDetectionResultListener(listener);
}

void CubeDetector::findCube(cv::Mat &mat) {
    behavior->findCube(mat);
}
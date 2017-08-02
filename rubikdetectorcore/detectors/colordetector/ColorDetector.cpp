//
// Created by catalin on 26.07.2017.
//

#include "ColorDetector.hpp"
#include "ColorDetectorBehavior.hpp"

ColorDetector::ColorDetector() : ColorDetector(nullptr) {}

ColorDetector::ColorDetector(ImageSaver *imageSaver) : colorDetectorBehavior(
        new ColorDetectorBehavior(imageSaver)) {}

ColorDetector::~ColorDetector() {
    delete colorDetectorBehavior;
}

int ColorDetector::detectColor(const cv::Mat &image, const float whiteRatio,
                               const int frameNr, const int regionId) {
    return colorDetectorBehavior->detectColor(image, whiteRatio, frameNr, regionId);
}

void ColorDetector::setDebuggable(const bool debuggable) const {
    colorDetectorBehavior->setDebuggable(debuggable);
}

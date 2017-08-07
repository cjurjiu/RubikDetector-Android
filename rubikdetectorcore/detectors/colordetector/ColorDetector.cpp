//
// Created by catalin on 26.07.2017.
//

#include "ColorDetector.hpp"
#include "ColorDetectorBehavior.hpp"
#include "../../utils/CrossLog.hpp"

ColorDetector::ColorDetector() : ColorDetector(nullptr) {}

ColorDetector::ColorDetector(std::shared_ptr<ImageSaver> imageSaver) : colorDetectorBehavior(
        std::unique_ptr<ColorDetectorBehavior>(new ColorDetectorBehavior(imageSaver))) {}

ColorDetector::~ColorDetector() {
    if (colorDetectorBehavior->isDebuggable()) {
        LOG_DEBUG("RubikJniPart.cpp", "ColorDetector - destructor.");
    }
}

int ColorDetector::detectColor(const cv::Mat &image, const float whiteRatio,
                               const int frameNr, const int regionId) {
    return colorDetectorBehavior->detectColor(image, whiteRatio, frameNr, regionId);
}

void ColorDetector::setDebuggable(const bool debuggable) const {
    colorDetectorBehavior->setDebuggable(debuggable);
}

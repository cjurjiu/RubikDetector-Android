//
// Created by catalin on 12.07.2017.
//

#include "../../../include/rubikdetector/detectors/faceletsdetector/SimpleFaceletsDetector.hpp"
#include "../../../include/rubikdetector/detectors/faceletsdetector/internal/SimpleFaceletsDetectorImpl.hpp"
#include "../../../include/rubikdetector/utils/CrossLog.hpp"

namespace rbdt {

SimpleFaceletsDetector::SimpleFaceletsDetector() : SimpleFaceletsDetector(nullptr) {}

SimpleFaceletsDetector::SimpleFaceletsDetector(const std::shared_ptr<ImageSaver> imageSaver)
        : behavior(std::unique_ptr<SimpleFaceletsDetectorImpl>(
        new SimpleFaceletsDetectorImpl(imageSaver))) {}

SimpleFaceletsDetector::~SimpleFaceletsDetector() {
    if (isDebuggable()) {
        LOG_DEBUG("RubikJniPart.cpp", "SimpleFaceletsDetector - destructor.");
    }
}

std::vector<std::vector<RubikFacelet>> SimpleFaceletsDetector::detect(cv::Mat &frameGray,
                                                                            const int frameNumber) {
    return behavior->detect(frameGray, frameNumber);
}

void SimpleFaceletsDetector::onFrameSizeSelected(int width, int height) {
    behavior->onFrameSizeSelected(width, height);
}

void SimpleFaceletsDetector::setDebuggable(const bool debuggable) {
    behavior->setDebuggable(debuggable);
}

bool SimpleFaceletsDetector::isDebuggable() const {
    return behavior->isDebuggable();
}

} //end namespace rbdt
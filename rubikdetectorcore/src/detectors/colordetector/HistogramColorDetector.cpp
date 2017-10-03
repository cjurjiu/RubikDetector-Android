//
// Created by catalin on 26.07.2017.
//

#include "../../../include/rubikdetector/detectors/colordetector/HistogramColorDetector.hpp"
#include "../../../include/rubikdetector/detectors/colordetector/internal/HistogramColorDetectorImpl.hpp"
#include "../../../include/rubikdetector/utils/CrossLog.hpp"

namespace rbdt {

HistogramColorDetector::HistogramColorDetector() : HistogramColorDetector(nullptr) {}

HistogramColorDetector::HistogramColorDetector(std::shared_ptr<ImageSaver> imageSaver)
        : colorDetectorImpl(
        std::unique_ptr<HistogramColorDetectorImpl>(
                new HistogramColorDetectorImpl(imageSaver))) {}

HistogramColorDetector::~HistogramColorDetector() {
    if (isDebuggable()) {
        LOG_DEBUG("RubikJniPart.cpp", "HistogramColorDetector - destructor.");
    }
}

RubikFacelet::Color HistogramColorDetector::detectColor(const cv::Mat &image,
                                                        const float whiteRatio,
                                                        const int regionInfo,
                                                        const int frameNr) {
    return colorDetectorImpl->detectColor(image, whiteRatio, regionInfo, frameNr);
}

void HistogramColorDetector::setDebuggable(const bool debuggable) {
    colorDetectorImpl->setDebuggable(debuggable);
}

bool HistogramColorDetector::isDebuggable() const {
    return colorDetectorImpl->isDebuggable();
}

} //end namespace rbdt
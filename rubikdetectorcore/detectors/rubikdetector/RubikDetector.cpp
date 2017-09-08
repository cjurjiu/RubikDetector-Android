//
// Created by catalin on 12.07.2017.
//

#include "RubikDetector.hpp"
#include "RubikDetectorImpl.hpp"
#include "../../utils/CrossLog.hpp"

RubikDetector::RubikDetector(const ImageProperties imageProperties,
                             std::unique_ptr<FaceletsDetector> faceletsDetector,
                             std::unique_ptr<ColorDetector> colorDetector,
                             std::unique_ptr<FaceletsDrawController> faceletsDrawController,
                             std::shared_ptr<ImageSaver> imageSaver)
        : behavior(std::unique_ptr<RubikDetectorImpl>(
        new RubikDetectorImpl(imageProperties,
                              std::move(faceletsDetector),
                              std::move(colorDetector),
                              std::move(faceletsDrawController),
                              imageSaver))) {}

RubikDetector::~RubikDetector() {
    if (isDebuggable()) {
        LOG_DEBUG("RubikJniPart.cpp", "RubikDetector - destructor.");
    }
}

std::vector<std::vector<RubikFacelet>> RubikDetector::process(const uint8_t *imageData) {
    return behavior->process(imageData);
}

void RubikDetector::updateImageProperties(const ImageProperties &imageProperties) {
    behavior->updateImageProperties(imageProperties);
}

void RubikDetector::overrideInputFrameWithResultFrame(const uint8_t *imageData) {
    behavior->overrideInputFrameWithResultFrame(imageData);
}

void RubikDetector::setDebuggable(const bool debuggable) {
    behavior->setDebuggable(debuggable);
}

bool RubikDetector::isDebuggable() const {
    return behavior->isDebuggable();
}

int RubikDetector::getRequiredMemory() {
    return behavior->getRequiredMemory();
}

int RubikDetector::getOutputFrameBufferOffset() {
    return behavior->getOutputFrameBufferOffset();
}

int RubikDetector::getOutputFrameByteCount() {
    return behavior->getOutputFrameByteCount();
}

int RubikDetector::getInputFrameByteCount() {
    return behavior->getInputFrameByteCount();
}

int RubikDetector::getInputFrameBufferOffset() {
    return behavior->getInputFrameBufferOffset();
}

void RubikDetector::updateDrawConfig(DrawConfig drawConfig) {
    behavior->updateDrawConfig(drawConfig);
}

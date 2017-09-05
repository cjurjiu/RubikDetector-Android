//
// Created by catalin on 12.07.2017.
//

#include "SimpleFaceletsDetector.hpp"
#include "SimpleFaceletsDetectorBehavior.hpp"
#include "../../utils/CrossLog.hpp"

SimpleFaceletsDetector::SimpleFaceletsDetector() : SimpleFaceletsDetector(nullptr) {}

SimpleFaceletsDetector::SimpleFaceletsDetector(const std::shared_ptr<ImageSaver> imageSaver)
        : behavior(std::unique_ptr<SimpleFaceletsDetectorBehavior>(
        new SimpleFaceletsDetectorBehavior(imageSaver))) {}

SimpleFaceletsDetector::~SimpleFaceletsDetector() {
    if (behavior->isDebuggable()) {
        LOG_DEBUG("RubikJniPart.cpp", "CubeDetector - destructor.");
    }
}

std::vector<std::vector<RubikFacelet>> SimpleFaceletsDetector::findCube(const uint8_t *imageData) {
    return behavior->findCube(imageData);
}

void SimpleFaceletsDetector::findCubeAsync(const uint8_t *imageData) {
    behavior->findCubeAsync(imageData);
}

void SimpleFaceletsDetector::setImageProperties(int width, int height, ImageFormat imageFormat) {
    behavior->setImageProperties(width, height, imageFormat);
}

void SimpleFaceletsDetector::overrideInputFrameWithResultFrame(const uint8_t *imageData) {
    behavior->overrideInputFrameWithResultFrame(imageData);
}

void
SimpleFaceletsDetector::setOnCubeDetectionResultListener(OnCubeDetectionResultListener *listener) {
    behavior->setOnCubeDetectionResultListener(listener);
}

void SimpleFaceletsDetector::setDebuggable(const bool debuggable) {
    behavior->setDebuggable(debuggable);
}

bool SimpleFaceletsDetector::isDebuggable() const {
    return behavior->isDebuggable();
}

int SimpleFaceletsDetector::getRequiredMemory() {
    return behavior->getRequiredMemory();
}

int SimpleFaceletsDetector::getOutputFrameBufferOffset() {
    return behavior->getOutputFrameBufferOffset();
}

int SimpleFaceletsDetector::getOutputFrameByteCount() {
    return behavior->getOutputFrameByteCount();
}

int SimpleFaceletsDetector::getInputFrameByteCount() {
    return behavior->getInputFrameByteCount();
}

int SimpleFaceletsDetector::getInputFrameBufferOffset() {
    return behavior->getInputFrameBufferOffset();
}

void SimpleFaceletsDetector::setShouldDrawFoundFacelets(bool shouldDrawFoundFacelets) {
    behavior->setShouldDrawFoundFacelets(shouldDrawFoundFacelets);
}
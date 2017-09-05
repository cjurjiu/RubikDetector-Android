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
        LOG_DEBUG("CATAMEM", "CubeDetector - destructor.");
        LOG_DEBUG("RubikJniPart.cpp", "CubeDetector - destructor.");
    }
}

std::vector<std::vector<RubikFacelet>> CubeDetector::findCube(const uint8_t *imageData) {
    return behavior->findCube(imageData);
}

void CubeDetector::findCubeAsync(const uint8_t *imageData) {
    behavior->findCubeAsync(imageData);
}

void CubeDetector::setImageProperties(int width, int height, int colorSpace) {
    behavior->setImageProperties(width, height, colorSpace);
}

void CubeDetector::setOnCubeDetectionResultListener(OnCubeDetectionResultListener *listener) {
    behavior->setOnCubeDetectionResultListener(listener);
}

void CubeDetector::setDebuggable(const bool debuggable) {
    behavior->setDebuggable(debuggable);
}

bool CubeDetector::isDebuggable() {
    return behavior->isDebuggable();
}

int CubeDetector::getTotalRequiredMemory() {
    return behavior->getTotalRequiredMemory();
}

int CubeDetector::getRgbaImageOffset() {
    return behavior->getRgbaImageOffset();
}

int CubeDetector::getRgbaImageSize() {
    return behavior->getRgbaImageSize();
}

int CubeDetector::getNv21ImageSize() {
    return behavior->getNv21ImageSize();
}

int CubeDetector::getNv21ImageOffset() {
    return behavior->getNv21ImageOffset();
}

void CubeDetector::setShouldDrawFoundFacelets(bool shouldDrawFoundFacelets) {
    behavior->setShouldDrawFoundFacelets(shouldDrawFoundFacelets);
}

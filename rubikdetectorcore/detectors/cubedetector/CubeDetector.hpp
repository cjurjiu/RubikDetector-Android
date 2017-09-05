//
// Created by catalin on 12.07.2017.
//

#ifndef RUBIKDETECTORDEMO_CUBEDETECTOR_HPP
#define RUBIKDETECTORDEMO_CUBEDETECTOR_HPP

#include <vector>
#include <cstdint>
#include <memory>
#include "../../data/RubikFacelet.hpp"

class OnCubeDetectionResultListener;

class CubeDetectorBehavior;

class ImageSaver;

static enum CubeColors {
    RED, ORANGE, YELLOW, GREEN, BLUE, WHITE
};

static enum ColorSpace {
    YUV_NV21, YUV_NV12, YUV_I420, YUV_YV12
};

class CubeDetector {
public:
    CubeDetector();

    CubeDetector(std::shared_ptr<ImageSaver> imageSaver);

    ~CubeDetector();

    std::vector<std::vector<RubikFacelet>> findCube(const uint8_t *imageData);

    void findCubeAsync(const uint8_t *imageData);

    void setOnCubeDetectionResultListener(OnCubeDetectionResultListener *listener);

    void setImageProperties(int width, int height, int colorSpace = YUV_NV21);

    void setShouldDrawFoundFacelets(bool shouldDrawFoundFacelets);

    void setDebuggable(const bool debuggable);

    bool isDebuggable();

    int getTotalRequiredMemory();

    int getRgbaImageOffset();

    int getRgbaImageSize();

    int getNv21ImageSize();

    int getNv21ImageOffset();

private:
    std::unique_ptr<CubeDetectorBehavior> behavior;
};

#endif //RUBIKDETECTORDEMO_CUBEDETECTOR_HPP

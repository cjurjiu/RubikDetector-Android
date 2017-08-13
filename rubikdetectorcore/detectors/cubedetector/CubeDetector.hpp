//
// Created by catalin on 12.07.2017.
//

#ifndef RUBIKDETECTORDEMO_CUBEDETECTOR_HPP
#define RUBIKDETECTORDEMO_CUBEDETECTOR_HPP

#include <vector>
#include <cstdint>
#include <memory>

namespace cv {
    class Mat;
}
class OnCubeDetectionResultListener;

class CubeDetectorBehavior;

class ImageSaver;

static const int RED = 0;
static const int ORANGE = 1;
static const int YELLOW = 2;
static const int GREEN = 3;
static const int BLUE = 4;
static const int WHITE = 5;

class CubeDetector {
public:
    CubeDetector();

    CubeDetector(std::shared_ptr<ImageSaver> imageSaver);

    ~CubeDetector();

    void
    findCube(const uint8_t *imageData, const int dataLength, int width, int height);

    void setOnCubeDetectionResultListener(OnCubeDetectionResultListener *listener);

    void setDebuggable(const bool debuggable);

    bool isDebuggable();

private:
    std::unique_ptr<CubeDetectorBehavior> behavior;

};

#endif //RUBIKDETECTORDEMO_CUBEDETECTOR_HPP

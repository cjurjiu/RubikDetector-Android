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

    void setOnCubeDetectionResultListener(const OnCubeDetectionResultListener &listener);

    void findCube(cv::Mat &mat);

    std::vector<unsigned char>
    findCube(const std::vector<uint8_t> &imageData, int width, int height);

    void setDebuggable(const bool debuggable);

private:
    CubeDetectorBehavior *behavior;
};

#endif //RUBIKDETECTORDEMO_CUBEDETECTOR_HPP

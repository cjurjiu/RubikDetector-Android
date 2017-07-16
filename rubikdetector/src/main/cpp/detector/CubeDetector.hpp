//
// Created by catalin on 12.07.2017.
//

#ifndef RUBIKDETECTORDEMO_CUBEDETECTOR_HPP
#define RUBIKDETECTORDEMO_CUBEDETECTOR_HPP

namespace cv {
    class Mat;
}
class OnCubeDetectionResultListener;

class CubeDetectorBehavior;

class CubeDetector {
public:
    CubeDetector();

    ~CubeDetector();

    void setOnCubeDetectionResultListener(OnCubeDetectionResultListener &listener);

    void findCube(cv::Mat &mat);

private:
    CubeDetectorBehavior *behavior;
};

#endif //RUBIKDETECTORDEMO_CUBEDETECTOR_HPP

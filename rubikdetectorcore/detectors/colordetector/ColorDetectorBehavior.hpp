//
// Created by catalin on 26.07.2017.
//

#ifndef RUBIKDETECTORDEMO_COLORDETECTORBEHAVIOR_HPP
#define RUBIKDETECTORDEMO_COLORDETECTORBEHAVIOR_HPP


#include <vector>
#include "../../helpers/ImageSaver.hpp"

namespace cv {
    class Mat;
}

class ColorDetectorBehavior {

public:
    ColorDetectorBehavior();

    ColorDetectorBehavior(ImageSaver *imageSaver);

    ~ColorDetectorBehavior();

    int detectColor(const cv::Mat &image, const float whiteRatio = 0.5f,
                    const int frameNr = -1, const int regionId = -1);

    void setDebuggable(bool debuggable);

private:
    int saturationThreshold = 100;

    bool debuggable = false;

    ImageSaver *imageSaver;

    void printOwnHistogram(const int hist[], const int histogramSize,
                           const int frameNumber, const int regionId) const;

    void computeSaturationHistogram(const std::vector<cv::Mat> &hsvChannels,
                                    int *saturationHistogram,
                                    int &nrNonWhiteNonGrayPixels) const;
};


#endif //RUBIKDETECTORDEMO_COLORDETECTORBEHAVIOR_HPP

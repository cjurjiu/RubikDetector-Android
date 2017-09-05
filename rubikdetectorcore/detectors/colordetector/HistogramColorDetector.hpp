//
// Created by catalin on 26.07.2017.
//

#ifndef RUBIKDETECTORDEMO_COLORDETECTOR_HPP
#define RUBIKDETECTORDEMO_COLORDETECTOR_HPP

#include <memory>
#include "ColorDetector.hpp"

namespace cv {
    class Mat;
}

class HistogramColorDetectorBehavior;

class ImageSaver;

class HistogramColorDetector : public ColorDetector {
public:
    HistogramColorDetector();

    HistogramColorDetector(std::shared_ptr<ImageSaver> imageSaver);

    virtual ~HistogramColorDetector();

    RubikFacelet::Color detectColor(const cv::Mat &image,
                                    const float whiteRatio = 0.5,
                                    const int regionInfo = -1,
                                    const int frameNr = -1) override;

    void setDebuggable(const bool debuggable) override;

    bool isDebuggable() const override;

private:
    std::unique_ptr<HistogramColorDetectorBehavior> colorDetectorBehavior;
};


#endif //RUBIKDETECTORDEMO_COLORDETECTOR_HPP

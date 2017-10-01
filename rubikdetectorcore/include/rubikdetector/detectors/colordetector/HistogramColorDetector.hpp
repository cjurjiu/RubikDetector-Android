//
// Created by catalin on 26.07.2017.
//

#ifndef RUBIKDETECTOR_HISTOGRAMCOLORDETECTOR_HPP
#define RUBIKDETECTOR_HISTOGRAMCOLORDETECTOR_HPP

#include <memory>
#include "RubikColorDetector.hpp"

namespace cv {
class Mat;
}

namespace rbdt {

class HistogramColorDetectorImpl;

class ImageSaver;

class HistogramColorDetector : public RubikColorDetector {
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
    std::unique_ptr<HistogramColorDetectorImpl> colorDetectorBehavior;
};

} //end namespace rbdt
#endif //RUBIKDETECTOR_HISTOGRAMCOLORDETECTOR_HPP

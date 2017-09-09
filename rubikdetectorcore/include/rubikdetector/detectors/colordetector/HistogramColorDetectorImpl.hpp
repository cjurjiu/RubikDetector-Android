//
// Created by catalin on 26.07.2017.
//

#ifndef RUBIKDETECTOR_HISTOGRAMCOLORDETECTORIMPL_HPP
#define RUBIKDETECTOR_HISTOGRAMCOLORDETECTORIMPL_HPP

#include <vector>
#include <memory>
#include "../../imagesaver/public/ImageSaver.hpp"
#include "../../data/processing/public/RubikFacelet.hpp"
#include "public/ColorDetector.hpp"

namespace cv {
class Mat;
}
namespace rbdt {

class HistogramColorDetectorImpl : ColorDetector {

public:
    HistogramColorDetectorImpl();

    HistogramColorDetectorImpl(std::shared_ptr<ImageSaver> imageSaver);

    virtual ~HistogramColorDetectorImpl();

    RubikFacelet::Color detectColor(const cv::Mat &image,
                                    const float whiteRatio = 0.5,
                                    const int regionInfo = -1,
                                    const int frameNr = -1) override;

    void setDebuggable(bool debuggable) override;

    bool isDebuggable() const override;

private:

    void printOwnHistogram(const int hist[], const int histogramSize,
                           const int frameNumber, const int regionId) const;

    void computeSaturationHistogram(const std::vector<cv::Mat> &hsvChannels,
                                    int *saturationHistogram,
                                    int &nrNonWhiteNonGrayPixels) const;

    static constexpr int HUE = 0;

    static constexpr int SATURATION = 1;

    static constexpr int VALUE = 2;

    static constexpr int MIN_HSV_VALUE_NON_GRAY = 80;

    static constexpr int MIN_HSV_VALUE_FOR_WHITE = 160;

    static constexpr int SATURATION_HISTOGRAM_SIZE = 256;

    static constexpr int HUE_HISTOGRAM_SIZE = 180;

    static constexpr int SATURATION_THRESHOLD = 100;

    bool debuggable = false;

    std::shared_ptr<ImageSaver> imageSaver;
};

} //namespace rbdt
#endif //RUBIKDETECTOR_HISTOGRAMCOLORDETECTORIMPL_HPP

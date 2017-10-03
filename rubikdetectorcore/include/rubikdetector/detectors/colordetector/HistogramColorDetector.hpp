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

/**
 * @copybrief RubikColorDetector
 * The input image is expected to be passed as a cv::Mat parameter to RubikColorDetector::detectColor().
 *
 * Concrete implementation of a RubikColorDetector which creates a histogram of the input frame. The histogram is then used as a source of evidence
 * for each possible RubikFacelet::Color.
 *
 * The selected RubikFacelet::Color will be the one with most evidence.
 *
 * @note The cv::Mat passed to HistogramColorDetector::detectColor() needs to use the HSV color space.
 *
 * Enabling the debug mode on this component makes it print information relevant for debugging. If a non-null ImageSaver is also provided through the
 * appropriate constructor, then mid-processing histograms & images are saved for debugging purposes, when debugging is turned on. However, performance is drastically
 * reduced when the images are saved, since writing to disk is typically very slow.
 */
class HistogramColorDetector : public RubikColorDetector {
public:
    /**
     * Default constructor.
     * @return a HistogramColorDetector.
     */
    HistogramColorDetector();

    /**
     * Constructs a detector capable of saving histograms & various image data to disk, when in debug mode.
     *
     * The ImageSaver provided here will not be used (therefore, will not impact performance) when debugging is off.
     * @param imageSaver an ImageSaver which can be used to print additional image data, for debugging purposes.
     * @return a HistogramColorDetector.
     */
    HistogramColorDetector(std::shared_ptr<ImageSaver> imageSaver);

    /**
     * Virtual destructor. Used just for logging when the memory is freed, if this component is debuggable.
     */
    virtual ~HistogramColorDetector();

    /**
     * @copybrief RubikColorDetector::detectColor()
     *
     * The <b><i>inputFrame</i></b> needs to be a cv::Mat using the HSV color space.
     *
     * @copydetails RubikColorDetector::detectColor()
     */
    RubikFacelet::Color detectColor(const cv::Mat &image,
                                    const float whiteRatio = 0.5,
                                    const int regionInfo = -1,
                                    const int frameNr = -1) override;

    void setDebuggable(const bool debuggable) override;

    bool isDebuggable() const override;

private:
    /**
     * Pointer to private implementation (PIMPL Pattern)
     */
    std::unique_ptr<HistogramColorDetectorImpl> colorDetectorImpl;
};

} //end namespace rbdt
#endif //RUBIKDETECTOR_HISTOGRAMCOLORDETECTOR_HPP

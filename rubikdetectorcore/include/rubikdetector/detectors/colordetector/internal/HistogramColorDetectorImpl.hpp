//
// Created by catalin on 26.07.2017.
//

#ifndef RUBIKDETECTOR_HISTOGRAMCOLORDETECTORIMPL_HPP
#define RUBIKDETECTOR_HISTOGRAMCOLORDETECTORIMPL_HPP

#include <vector>
#include <memory>
#include "../../../imagesaver/ImageSaver.hpp"
#include "../../../data/processing/RubikFacelet.hpp"
#include "../RubikColorDetector.hpp"

namespace cv {
class Mat;
}
namespace rbdt {
/**
 * Class to which a HistogramColorDetector delegates its work to.
 *
 * This is effectivelly the <i>private implementation</i> of a HistogramColorDetector's behavior.
 *
 * @note Copied from HistogramColorDetector:
 *
 * @copydoc HistogramColorDetector
 *
 * @note end copy.
 *
 * @warning Do not use HistogramColorDetectorImpl directly, do not expose this in the API. Use the HistogramColorDetector instead.
 *
 * @see HistogramColorDetector
 */
class HistogramColorDetectorImpl : RubikColorDetector {

public:
    /**
     * @note Copied from HistogramColorDetector:
     *
     * @copybrief HistogramColorDetector::HistogramColorDetector
     *
     * @note end copy.
     *
     * @return HistogramColorDetectorImpl
     */
    HistogramColorDetectorImpl();

    /**
     * @note Copied from HistogramColorDetector:
     *
     * @copybrief HistogramColorDetector::HistogramColorDetector(std::shared_ptr<ImageSaver> imageSaver)
     *
     * The ImageSaver provided here will not be used (therefore, will not impact performance) when debugging is off.
     * @param imageSaver an ImageSaver which can be used to print additional image data, for debugging purposes.
     *
     * @note end copy.
     *
     * @return HistogramColorDetectorImpl
     */
    HistogramColorDetectorImpl(std::shared_ptr<ImageSaver> imageSaver);

    /**
     * @note Copied from HistogramColorDetector:
     *
     * @copybrief HistogramColorDetector::~HistogramColorDetector
     *
     * @note end copy.
     */
    virtual ~HistogramColorDetectorImpl();

    /**
     * @copydoc HistogramColorDetector::detectColor()
     */
    RubikFacelet::Color detectColor(const cv::Mat &image,
                                    const float whiteRatio = 0.5,
                                    const int regionInfo = -1,
                                    const int frameNr = -1) override;

    void setDebuggable(bool debuggable) override;

    bool isDebuggable() const override;

private:

    /**
     * Print as a histogram the values present in the array received as a parameter.
     *
     * @param [in] hist int array representing the histogram values
     * @param [in] histogramSize integer representing the number of elements in the histogram array
     * @param [in] frameNumber the current frame number. added to the name of the saved file in order to be able to uniquelly identify the file
     * @param [in] regionId a unique identifier for this image, in the current frame. This Value is added to the name of the saved file in order
     * to be able to uniquelly identify this histogram, among others in the same frame.
     */
    void printOwnHistogram(const int hist[], const int histogramSize,
                           const int frameNumber, const int regionId) const;

    /**
     * Creates a histogram for the staturation. Only pixels that satisfy the below conditions will be counted:
     *   - pixels with a saturation value below HistogramColorDetectorImpl::SATURATION_THRESHOLD (i.e. with not enough color)
     *   - pixels with a value above HistogramColorDetectorImpl::MIN_HSV_VALUE_FOR_WHITE (i.e. with enough light to be white,
     *   or a very light gray, but gray or dark gray)
     *
     * All other pixels are regarded as being either colored pixels, or grayscale pixels, and not added to the histogram.
     *
     * Non-grayscale pixels, are also counted & returned as the <b>nrNonGrayPixels</b> in/out parameter.
     *
     * @param [in] hsvChannels a std::vector of 3 elements. Each needs to represent a channel of the original HSV frame in which detection is performed.
     * The Hue component is expected to be index 0, Saturation at index 1 & Value at index 2.
     * @param [in/out] saturationHistogram the array in which the histogram will be written
     * @param [in/out] nrNonGrayPixels the number of non-grayscale pixels.
     */
    void computeSaturationHistogram(const std::vector<cv::Mat> &hsvChannels,
                                    int *saturationHistogram,
                                    int &nrNonGrayPixels) const;

    static constexpr int HUE = 0;

    static constexpr int SATURATION = 1;

    static constexpr int VALUE = 2;

    static constexpr int MIN_HSV_VALUE_NON_GRAY = 80;

    static constexpr int MIN_HSV_VALUE_FOR_WHITE = 160;

    static constexpr int SATURATION_THRESHOLD = 100;

    static constexpr int SATURATION_HISTOGRAM_SIZE = 256;

    static constexpr int HUE_HISTOGRAM_SIZE = 180;

    static constexpr int BUCKET_ORANGE_MIN_THRESHOLD = 4;

    static constexpr int BUCKET_ORANGE_MAX_THRESHOLD = 18;

    static constexpr int BUCKET_YELLOW_MIN_THRESHOLD = 18;

    static constexpr int BUCKET_YELLOW_MAX_THRESHOLD = 39;

    static constexpr int BUCKET_GREEN_MIN_THRESHOLD = 39;

    static constexpr int BUCKET_GREEN_MAX_THRESHOLD = 76;

    static constexpr int BUCKET_BLUE_MIN_THRESHOLD = 76;

    static constexpr int BUCKET_BLUE_MAX_THRESHOLD = 136;

    static constexpr int BUCKET_RED_MIN_THRESHOLD_1 = 171;

    static constexpr int BUCKET_RED_MAX_THRESHOLD_1 = 179;

    static constexpr int BUCKET_RED_MIN_THRESHOLD_2 = 0;

    static constexpr int BUCKET_RED_MAX_THRESHOLD_2 = 4;

    bool debuggable = false;

    std::shared_ptr<ImageSaver> imageSaver;
};

} //namespace rbdt
#endif //RUBIKDETECTOR_HISTOGRAMCOLORDETECTORIMPL_HPP

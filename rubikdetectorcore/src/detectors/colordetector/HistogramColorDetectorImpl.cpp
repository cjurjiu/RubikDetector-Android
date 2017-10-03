//
// Created by catalin on 26.07.2017.
//

#include "../../../include/rubikdetector/detectors/colordetector/internal/HistogramColorDetectorImpl.hpp"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "../../../include/rubikdetector/data/processing/internal/HueColorEvidence.hpp"
#include "../../../include/rubikdetector/utils/CrossLog.hpp"
#include "../../../include/rubikdetector/utils/Utils.hpp"

namespace rbdt {

HistogramColorDetectorImpl::HistogramColorDetectorImpl() : HistogramColorDetectorImpl(
        nullptr) {

}

HistogramColorDetectorImpl::HistogramColorDetectorImpl(
        std::shared_ptr<ImageSaver> imageSaver) : imageSaver(
        imageSaver) {

}

HistogramColorDetectorImpl::~HistogramColorDetectorImpl() {
    if (debuggable) {
        LOG_DEBUG("RubikJniPart.cpp", "HistogramColorDetectorBehavior - destructor.");
    }
}

RubikFacelet::Color HistogramColorDetectorImpl::detectColor(const cv::Mat &image,
                                                            const float whiteRatio,
                                                            const int regionInfo,
                                                            const int frameNr) {

    std::vector<cv::Mat> hsvChannels;
    //split the HSV into 3 other Mat, one for each color channel
    split(image, hsvChannels);

    int saturationHistogram[SATURATION_HISTOGRAM_SIZE];
    int nrNonGrayPixels = 0;
    computeSaturationHistogram(hsvChannels, saturationHistogram, nrNonGrayPixels);

    int nrWhitePixels = 0;
    for (int i = 0; i <= SATURATION_THRESHOLD; i++) {
        nrWhitePixels += saturationHistogram[i];
    }

    float whitePixelRatio = (float) nrWhitePixels / nrNonGrayPixels;

    if (whitePixelRatio > whiteRatio) {
        //if the majority of the saturation values are in the "almost white" range of the saturation domain, then assume color is white
        if (debuggable && imageSaver != nullptr) {
            //print saturation histogram, if in debug mode
            printOwnHistogram(saturationHistogram, SATURATION_HISTOGRAM_SIZE, frameNr, regionInfo);
            cv::cvtColor(image, image, cv::COLOR_HSV2BGR);
            imageSaver->saveImage(image, frameNr, regionInfo);
        }
        return RubikFacelet::Color::WHITE;
    } else {
        uchar pixelHsvHue, pixelHsvValue;
        int hueHistogram[HUE_HISTOGRAM_SIZE];
        for (int i = 0; i < HUE_HISTOGRAM_SIZE; i++) {
            //ensure all values are 0
            hueHistogram[i] = 0;
        }
        for (int i = 0; i < image.rows; i++) {
            for (int j = 0; j < image.cols; j++) {
                pixelHsvHue = hsvChannels[HUE].at<uchar>(i, j);
                pixelHsvValue = hsvChannels[VALUE].at<uchar>(i, j);
                if (pixelHsvValue > MIN_HSV_VALUE_NON_GRAY) {
                    //if the pixel has just enough color to not be considered gray scale, add it to the histogram
                    hueHistogram[pixelHsvHue]++;
                }
            }
        }

        //create 5 color buckets(white is excluded)
        HueColorEvidence colorEvidence[5] = {HueColorEvidence(RubikFacelet::Color::RED),
                                             HueColorEvidence(RubikFacelet::Color::ORANGE),
                                             HueColorEvidence(RubikFacelet::Color::YELLOW),
                                             HueColorEvidence(RubikFacelet::Color::GREEN),
                                             HueColorEvidence(RubikFacelet::Color::BLUE)};

        //partition the values in the 5 color buckets 
        //add the value at each histogram position as evidence to a particular color bucket
        for (int i = 0; i < HUE_HISTOGRAM_SIZE; i++) {
            if (i >= BUCKET_ORANGE_MIN_THRESHOLD && i <= BUCKET_ORANGE_MAX_THRESHOLD) {
                colorEvidence[rbdt::asInt(RubikFacelet::Color::ORANGE)]
                        .evidence += hueHistogram[i];
            } else if (i > BUCKET_YELLOW_MIN_THRESHOLD && i <= BUCKET_YELLOW_MAX_THRESHOLD) {
                colorEvidence[rbdt::asInt(RubikFacelet::Color::YELLOW)]
                        .evidence += hueHistogram[i];
            } else if (i > BUCKET_GREEN_MIN_THRESHOLD && i <= BUCKET_GREEN_MAX_THRESHOLD) {
                colorEvidence[rbdt::asInt(RubikFacelet::Color::GREEN)]
                        .evidence += hueHistogram[i];
            } else if (i > BUCKET_BLUE_MIN_THRESHOLD && i <= BUCKET_BLUE_MAX_THRESHOLD) {
                colorEvidence[rbdt::asInt(RubikFacelet::Color::BLUE)]
                        .evidence += hueHistogram[i];
            } else if ((i >= BUCKET_RED_MIN_THRESHOLD_1 && i <= BUCKET_RED_MAX_THRESHOLD_1) ||
                       (i >= BUCKET_RED_MIN_THRESHOLD_2 && i < BUCKET_RED_MAX_THRESHOLD_2)) {
                colorEvidence[rbdt::asInt(RubikFacelet::Color::RED)]
                        .evidence += hueHistogram[i];
            }
        }

        //sort the colors in descending evidence order
        std::sort(colorEvidence, colorEvidence + 5,
                  [](const HueColorEvidence &firstItem, const HueColorEvidence &secondItem) {
                      return firstItem.evidence > secondItem.evidence;
                  });

        if (debuggable && imageSaver != nullptr) {
            printOwnHistogram(hueHistogram, HUE_HISTOGRAM_SIZE, frameNr, regionInfo);
            cv::cvtColor(image, image, cv::COLOR_HSV2BGR);
            imageSaver->saveImage(image, frameNr, regionInfo);
        }

        //return the color with most evidence,i.e. the first in the sorted evidence array
        return colorEvidence[0].color;
    }
}

void HistogramColorDetectorImpl::setDebuggable(bool debuggable) {
    HistogramColorDetectorImpl::debuggable = debuggable;
}

bool HistogramColorDetectorImpl::isDebuggable() const {
    return debuggable;
}

void HistogramColorDetectorImpl::printOwnHistogram(const int hist[], const int histogramSize,
                                                   const int frameNumber,
                                                   const int regionId) const {
    if (imageSaver == nullptr) {
        //do nothing
        return;
    }
    int hist_w = histogramSize * 4;
    int hist_h = 2300;
    cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(255, 255, 255));

    /// Draw for each channel
    for (int i = 0; i < histogramSize; i++) {
        int height = hist[i];
        if (height > 0) {
            line(histImage, cv::Point(i * 4 + 2, hist_h),
                 cv::Point(i * 4 + 2, hist_h - height), cv::Scalar(255, 0, 0), 4, 0,
                 0);
        }
    }
    imageSaver->saveImage(histImage, frameNumber, regionId);
}

void HistogramColorDetectorImpl::computeSaturationHistogram(const std::vector<cv::Mat> &hsvChannels,
                                                            int *saturationHistogram,
                                                            int &nrNonGrayPixels) const {
    uchar pixelHsvSaturation, pixelHsvValue;
    for (int i = 0; i < SATURATION_HISTOGRAM_SIZE; i++) {
        saturationHistogram[i] = 0;
    }

    for (int i = 0; i < hsvChannels[SATURATION].rows; i++) {
        for (int j = 0; j < hsvChannels[SATURATION].cols; j++) {
            pixelHsvSaturation = hsvChannels[SATURATION].at<uchar>(i, j);
            pixelHsvValue = hsvChannels[VALUE].at<uchar>(i, j);
            if (pixelHsvSaturation <= SATURATION_THRESHOLD &&
                pixelHsvValue > MIN_HSV_VALUE_FOR_WHITE) {
                saturationHistogram[pixelHsvSaturation]++;
            }
            if (pixelHsvValue > MIN_HSV_VALUE_NON_GRAY) {
                nrNonGrayPixels++;
            }
        }
    }
}

}//namespace rbdt
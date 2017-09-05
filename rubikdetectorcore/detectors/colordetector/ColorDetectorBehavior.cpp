//
// Created by catalin on 26.07.2017.
//

#include "ColorDetectorBehavior.hpp"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "../cubedetector/CubeDetector.hpp"
#include "../../data/HueColorEvidence.hpp"
#include "../../utils/CrossLog.hpp"

#ifndef  HUE
#define HUE 0
#endif

#ifndef  SATURATION
#define SATURATION 1
#endif

#ifndef  VALUE
#define VALUE 2
#endif

#ifndef MIN_HSV_VALUE_NON_GRAY
#define MIN_HSV_VALUE_NON_GRAY 80
#endif

#ifndef MIN_HSV_VALUE_FOR_WHITE
#define MIN_HSV_VALUE_FOR_WHITE 160
#endif

#ifndef SATURATION_HISTOGRAM_SIZE
#define SATURATION_HISTOGRAM_SIZE 256
#endif

#ifndef HUE_HISTOGRAM_SIZE
#define HUE_HISTOGRAM_SIZE 180
#endif

ColorDetectorBehavior::ColorDetectorBehavior() : ColorDetectorBehavior(nullptr) {

}

ColorDetectorBehavior::ColorDetectorBehavior(std::shared_ptr<ImageSaver> imageSaver) : imageSaver(
        imageSaver) {

}

ColorDetectorBehavior::~ColorDetectorBehavior() {
    if (debuggable) {
        LOG_DEBUG("RubikJniPart.cpp", "ColorDetectorBehavior - destructor.");
    }
}

int ColorDetectorBehavior::detectColor(const cv::Mat &image, const float whiteRatio,
                                       const int frameNr, const int regionId) {

    std::vector<cv::Mat> hsvChannels;
    split(image, hsvChannels);

    int saturationHistogram[256];
    int nrNonWhiteNonGrayPixels = 0;
    computeSaturationHistogram(hsvChannels, saturationHistogram, nrNonWhiteNonGrayPixels);

    int nrWhitePixels = 0;
    for (int i = 0; i <= saturationThreshold; i++) {
        nrWhitePixels += saturationHistogram[i];
    }

    float whitePixelRatio = (float) nrWhitePixels / nrNonWhiteNonGrayPixels;

    if (whitePixelRatio > whiteRatio) {
        //if the majority of the saturation values are in the "almost white" range of the saturation domain, then assume color is white
        if (debuggable && imageSaver != nullptr) {
            //print saturation histogram, if in debug mode
            printOwnHistogram(saturationHistogram, 256, frameNr, regionId);
            cv::cvtColor(image, image, cv::COLOR_HSV2BGR);
            imageSaver->saveImage(image, frameNr, regionId);
        }
        return CubeColors::WHITE;
    } else {
        uchar pixelHsvHue, pixelHsvValue;
        int hueHistogram[HUE_HISTOGRAM_SIZE];
        for (int i = 0; i < HUE_HISTOGRAM_SIZE; i++) {
            hueHistogram[i] = 0;
        }
        for (int i = 0; i < image.rows; i++) {
            for (int j = 0; j < image.cols; j++) {
                pixelHsvHue = hsvChannels[HUE].at<uchar>(i, j);
                pixelHsvValue = hsvChannels[VALUE].at<uchar>(i, j);
                if (pixelHsvValue > MIN_HSV_VALUE_NON_GRAY) {
                    hueHistogram[pixelHsvHue]++;
                }
            }
        }

        HueColorEvidence colorEvidence[5] = {HueColorEvidence(CubeColors::RED),
                                             HueColorEvidence(CubeColors::ORANGE),
                                             HueColorEvidence(CubeColors::YELLOW),
                                             HueColorEvidence(CubeColors::GREEN),
                                             HueColorEvidence(CubeColors::BLUE)};

        for (int i = 0; i < HUE_HISTOGRAM_SIZE; i++) {
            if (i >= 4 && i <= 18) {
                colorEvidence[CubeColors::ORANGE].evidence += hueHistogram[i];
            } else if (i > 18 && i <= 39) {
                colorEvidence[CubeColors::YELLOW].evidence += hueHistogram[i];
            } else if (i > 39 && i <= 76) {
                colorEvidence[CubeColors::GREEN].evidence += hueHistogram[i];
            } else if (i > 76 && i <= 136) {
                colorEvidence[CubeColors::BLUE].evidence += hueHistogram[i];
            } else if ((i >= 171 && i <= 179) || (i >= 0 && i < 4)) {
                colorEvidence[CubeColors::RED].evidence += hueHistogram[i];
            }
        }

        std::sort(colorEvidence, colorEvidence + 5,
                  [](const HueColorEvidence &firstItem, const HueColorEvidence &secondItem) {
                      return firstItem.evidence > secondItem.evidence;
                  });
        if (debuggable && imageSaver != nullptr) {
            printOwnHistogram(hueHistogram, 180, frameNr, regionId);
            cv::cvtColor(image, image, cv::COLOR_HSV2BGR);
            imageSaver->saveImage(image, frameNr, regionId);
        }
        //return the color with most evidence
        return colorEvidence[0].color;
    }
}

void ColorDetectorBehavior::setDebuggable(bool debuggable) {
    ColorDetectorBehavior::debuggable = debuggable;
}

bool ColorDetectorBehavior::isDebuggable() {
    return debuggable;
}

/**
Print the histogram and also specify the row & column of the printed sticker
*/
void ColorDetectorBehavior::printOwnHistogram(const int hist[], const int histogramSize,
                                              const int frameNumber, const int regionId) const {
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

void ColorDetectorBehavior::computeSaturationHistogram(const std::vector<cv::Mat> &hsvChannels,
                                                       int *saturationHistogram,
                                                       int &nrNonWhiteNonGrayPixels) const {
    uchar pixelHsvSaturation, pixelHsvValue;
    for (int i = 0; i < SATURATION_HISTOGRAM_SIZE; i++) {
        saturationHistogram[i] = 0;
    }

    for (int i = 0; i < hsvChannels[SATURATION].rows; i++) {
        for (int j = 0; j < hsvChannels[SATURATION].cols; j++) {
            pixelHsvSaturation = hsvChannels[SATURATION].at<uchar>(i, j);
            pixelHsvValue = hsvChannels[VALUE].at<uchar>(i, j);
            if (pixelHsvSaturation <= saturationThreshold &&
                pixelHsvValue > MIN_HSV_VALUE_FOR_WHITE) {
                saturationHistogram[pixelHsvSaturation]++;
            }
            if (pixelHsvValue > MIN_HSV_VALUE_NON_GRAY) {
                nrNonWhiteNonGrayPixels++;
            }
        }
    }
}

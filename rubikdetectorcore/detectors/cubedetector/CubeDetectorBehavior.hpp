//
// Created by catalin on 12.07.2017.
//

#ifndef RUBIKDETECTORDEMO_CUBEDETECTORBEHAVIOR_HPP
#define RUBIKDETECTORDEMO_CUBEDETECTORBEHAVIOR_HPP

#include <opencv2/core/core.hpp>
#include "../../data/Circle.hpp"
#include "../../data/HueColorEvidence.hpp"
#include "../colordetector/ColorDetector.hpp"
#include "../../helpers/ImageSaver.hpp"
#include <iostream>
#include <memory>


#ifndef  X1
#define X1 0
#endif

#ifndef  Y1
#define Y1 1
#endif

#ifndef  X2
#define X2 2
#endif

#ifndef  Y2
#define Y2 3
#endif

#ifndef  HUE
#define HUE 0
#endif

#ifndef  SATURATION
#define SATURATION 1
#endif

#ifndef  VALUE
#define VALUE 2
#endif

#ifndef  CANNY_DETECTION
#define CANNY_DETECTION 1
#endif

#ifndef  CANNY_DETECTION_DEMO
#define CANNY_DETECTION_DEMO 3
#endif

class OnCubeDetectionResultListener;

class CubeDetectorBehavior {
public:
    CubeDetectorBehavior();

    CubeDetectorBehavior(std::shared_ptr<ImageSaver> imageSaver);

    ~CubeDetectorBehavior();

    void setOnCubeDetectionResultListener(OnCubeDetectionResultListener *listener);

    void
    findCube(const uint8_t *imageData, const int dataLength, int width, int height);

    void setDebuggable(const bool isDebuggable);

    bool isDebuggable();

private:
    std::unique_ptr<OnCubeDetectionResultListener> onCubeDetectionResultListener;

    std::unique_ptr<ColorDetector> colorDetector;

    std::shared_ptr<ImageSaver> imageSaver;

    int frameNumber = 0;

    int frameRateSum = 0;

    bool debuggable = false;

    int getSmallestMargin(Circle referenceCircle, std::vector<Circle> validCircles);

    int getPositionInVector(int i, int j);

    cv::Scalar getColorAsScalar(int color);

    void performCannyProcessing(cv::Mat &frameRgba, cv::Mat &frameGray);

    void saveDebugData(const cv::Mat &currentFrame,
                       const std::vector<cv::RotatedRect> &filteredRectangles,
                       const Circle &referenceCircle, const std::vector<Circle> &potentialFacelets,
                       const std::vector<Circle> &estimatedFacelets,
                       const std::vector<std::vector<int>> colors);

    std::vector<std::vector<cv::Point>> detectContours(const cv::Mat &frameGray) const;

    void filterContours(const cv::Mat &currentFrame,
                        const std::vector<std::vector<cv::Point>> &contours,
                        std::vector<cv::RotatedRect> &possibleFacelets,
                        std::vector<Circle> &possibleFaceletsInnerCircles) const;

    std::vector<Circle>
    findPotentialFacelets(const Circle &referenceCircle,
                          const std::vector<Circle> &innerCircles,
                          int referenceCircleIndex) const;

    std::vector<Circle>
    estimateRemainingFaceletsPos(const Circle &referenceCircle, int diameterWithMargin) const;

    std::vector<std::vector<Circle>>
    matchEstimatedWithPotentialFacelets(const std::vector<Circle> &potentialFacelets,
                                        const std::vector<Circle> &estimatedFacelets);

    bool verifyIfCubeFound(const std::vector<std::vector<Circle>> &cubeFacet) const;

    void fillMissingFacelets(const std::vector<Circle> &facelets,
                             std::vector<std::vector<Circle>> &vector);

    std::vector<std::vector<int>>
    detectFacetColors(const cv::Mat &currentFrame,
                      const std::vector<std::vector<Circle>> facetModel);

    void
    drawFoundFacelets(cv::Mat &currentFrame, const std::vector<std::vector<Circle>> &facetModel,
                      const std::vector<std::vector<int>> &colors);
};

#endif //RUBIKDETECTORDEMO_CUBEDETECTORBEHAVIOR_HPP

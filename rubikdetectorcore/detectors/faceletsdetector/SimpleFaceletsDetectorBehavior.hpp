//
// Created by catalin on 12.07.2017.
//

#ifndef RUBIKDETECTORDEMO_CUBEDETECTORBEHAVIOR_HPP
#define RUBIKDETECTORDEMO_CUBEDETECTORBEHAVIOR_HPP

#include <opencv2/core/core.hpp>
#include "../../data/Circle.hpp"
#include "../../data/HueColorEvidence.hpp"
#include "../colordetector/HistogramColorDetector.hpp"
#include "../../imagesaver/ImageSaver.hpp"
#include "FaceletsDetector.hpp"
#include <iostream>
#include <memory>

class OnCubeDetectionResultListener;

class RubikFacelet;

class SimpleFaceletsDetectorBehavior : FaceletsDetector {
public:
    SimpleFaceletsDetectorBehavior();

    SimpleFaceletsDetectorBehavior(std::shared_ptr<ImageSaver> imageSaver);

    virtual ~SimpleFaceletsDetectorBehavior();

    std::vector<std::vector<RubikFacelet>>
    findFacelets(cv::Mat &frameRgba, cv::Mat &frameGray, const int frameNumber = 0) override;

    void onFrameSizeSelected(int processingWidth, int processingHeight) override;

    void setDebuggable(const bool isDebuggable) override;

    bool isDebuggable() const override;

private:

    static constexpr float MIN_VALID_SHAPE_TO_IMAGE_AREA_RATIO = 0.0025f;

    static constexpr float MIN_VALID_SHAPE_TO_IMAGE_SIDE_SIZE_RATIO = 0.25f;

    static constexpr int CIRCLE_DISTANCE_BUFFER = 2 * 10;

    static constexpr int BLUR_KERNEL_SIZE = 5;

    static constexpr int CANNY_LOW_THRESHOLD = 80;

    static constexpr int CANNY_THRESHOLD_RATIO = 3;

    static constexpr int CANNY_APERTURE_SIZE = 5;

    std::shared_ptr<ImageSaver> imageSaver;

    bool debuggable = false;

    int maxShapeSideSize;
    int minValidShapeArea;

    std::vector<std::vector<cv::Point>> detectContours(const cv::Mat &frameGray) const;

    void filterContours(const cv::Mat &currentFrame,
                        const std::vector<std::vector<cv::Point>> &contours,
                        std::vector<cv::RotatedRect> &possibleFacelets,
                        std::vector<Circle> &possibleFaceletsInnerCircles) const;

    float getSmallestMargin(Circle referenceCircle, std::vector<Circle> validCircles);

    std::vector<Circle> findPotentialFacelets(const Circle &referenceCircle,
                                              const std::vector<Circle> &innerCircles,
                                              int referenceCircleIndex) const;

    std::vector<Circle> estimateRemainingFaceletsPositions(const Circle &referenceCircle,
                                                           float margin) const;

    std::vector<std::vector<Circle>> matchEstimatedWithPotentialFacelets(
            const std::vector<Circle> &potentialFacelets,
            const std::vector<Circle> &estimatedFacelets);

    bool verifyIfCubeFound(const std::vector<std::vector<Circle>> &cubeFacet) const;

    void fillMissingFacelets(const std::vector<Circle> &facelets,
                             std::vector<std::vector<Circle>> &vector);

    std::vector<std::vector<RubikFacelet>>
    createResult(const std::vector<std::vector<Circle>> &model);

    void saveWholeFrame(const cv::Mat &currentFrame, int frameNr) const;

    cv::Mat saveFilteredRectangles(const cv::Mat &currentFrame,
                                   const std::vector<cv::RotatedRect> &filteredRectangles,
                                   int frameNr) const;

    void saveDebugData(const cv::Mat &frame,
                       const std::vector<cv::RotatedRect> &filteredRectangles,
                       const Circle &referenceCircle,
                       const std::vector<Circle> &potentialFacelets,
                       const std::vector<Circle> &estimatedFacelets,
                       const std::vector<std::vector<RubikFacelet::Color>> colors,
                       const int frameNumber);

    void drawRectangleToMat(const cv::Mat &currentFrame, const cv::RotatedRect &rotatedRect,
                            const cv::Scalar color = cv::Scalar(0, 255, 0)) const;

};

#endif //RUBIKDETECTORDEMO_CUBEDETECTORBEHAVIOR_HPP

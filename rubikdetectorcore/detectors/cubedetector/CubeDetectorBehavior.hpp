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

class RubikFacelet;

class CubeDetectorBehavior {
public:
    CubeDetectorBehavior();

    CubeDetectorBehavior(std::shared_ptr<ImageSaver> imageSaver);

    ~CubeDetectorBehavior();

    std::vector<std::vector<RubikFacelet>> findCube(const uint8_t *imageData);

    void
    findCubeAsync(const uint8_t *imageData);

    void setOnCubeDetectionResultListener(OnCubeDetectionResultListener *listener);

    void setImageProperties(int width, int height, int colorSpace);

    void setShouldDrawFoundFacelets(bool shouldDrawFoundFacelets);

    void setDebuggable(const bool isDebuggable);

    bool isDebuggable();

    int getTotalRequiredMemory();

    int getRgbaImageOffset();

    int getRgbaImageSize();

    int getNv21ImageSize();

    int getNv21ImageOffset();

private:

    static constexpr int DEFAULT_DIMENSION = 320;

    static constexpr float MIN_VALID_SHAPE_TO_IMAGE_AREA_RATIO = 0.0025f;

    static constexpr float MIN_VALID_SHAPE_TO_IMAGE_SIDE_SIZE_RATIO = 0.25f;

    static constexpr int BLUR_KERNEL_SIZE = 5;

    static constexpr int CANNY_LOW_THRESHOLD = 80;

    static constexpr int CANNY_THRESHOLD_RATIO = 3;

    static constexpr int CANNY_APERTURE_SIZE = 5;

    std::unique_ptr<OnCubeDetectionResultListener> onCubeDetectionResultListener;

    std::unique_ptr<ColorDetector> colorDetector;

    std::shared_ptr<ImageSaver> imageSaver;

    int frameNumber = 0;

    int frameRateSum = 0;

    bool debuggable = false;

    bool shouldDrawFoundFacelets = false;

    int imageHeight;
    int imageWidth;
    int totalRequiredMemory;
    int rgbaImageOffset;
    int rgbaImageSize;
    int nv21ImageSize;
    float upscalingRatio;
    int maxShapeSideSize;
    int minValidShapeArea;
    int processingWidth;
    int processingHeight;
    int largestDimension;
    float downscalingRatio;
    bool needsResize;
    int nv21ImageOffset;
    int processingRgbaImageOffset;
    int processingRgbaImageSize;
    int processingGrayImageOffset;
    int processingGrayImageSize;
    int cvColorConversionCode;

    std::vector<std::vector<RubikFacelet>> findCubeInternal(const uint8_t *imageData);

    std::vector<std::vector<RubikFacelet>> findFaceletsInFrame(cv::Mat &frameRgba,
                                                               cv::Mat &frameGray,
                                                               cv::Mat &resultFrame);

    std::vector<std::vector<cv::Point>> detectContours(const cv::Mat &frameGray) const;

    void filterContours(const cv::Mat &currentFrame,
                        const std::vector<std::vector<cv::Point>> &contours,
                        std::vector<cv::RotatedRect> &possibleFacelets,
                        std::vector<Circle> &possibleFaceletsInnerCircles) const;

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

    std::vector<std::vector<int>> detectFacetColors(const cv::Mat &currentFrame,
                                                    const std::vector<std::vector<Circle>> facetModel);

    std::vector<std::vector<RubikFacelet>> createResult(const std::vector<std::vector<int>> &colors,
                                                        const std::vector<std::vector<Circle>> &model);

    void drawFoundFaceletsCircles(cv::Mat &procRgbaFrame,
                                  std::vector<std::vector<RubikFacelet>> &facetModel);

    void drawFoundFaceletsRectangles(cv::Mat &procRgbaFrame,
                                     std::vector<std::vector<RubikFacelet>> &facetModel);

    void saveWholeFrame(const cv::Mat &currentFrame, int frameNr) const;

    cv::Mat saveFilteredRectangles(const cv::Mat &currentFrame,
                                   const std::vector<cv::RotatedRect> &filteredRectangles,
                                   int frameNr) const;

    void saveDebugData(const cv::Mat &frame,
                       const std::vector<cv::RotatedRect> &filteredRectangles,
                       const Circle &referenceCircle,
                       const std::vector<Circle> &potentialFacelets,
                       const std::vector<Circle> &estimatedFacelets,
                       const std::vector<std::vector<int>> colors);

    void drawRectangleToMat(const cv::Mat &currentFrame, const cv::RotatedRect &rotatedRect,
                            const cv::Scalar color = cv::Scalar(0, 255, 0)) const;

    float getSmallestMargin(Circle referenceCircle, std::vector<Circle> validCircles);

};

#endif //RUBIKDETECTORDEMO_CUBEDETECTORBEHAVIOR_HPP

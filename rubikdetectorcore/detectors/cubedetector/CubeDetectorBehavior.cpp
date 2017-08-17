//// Created by catalin on 12.07.2017.
//

#include <math.h>
#include "CubeDetectorBehavior.hpp"
#include "CubeDetector.hpp"
#include "OnCubeDetectionResultListener.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "../../utils/Utils.hpp"
#include "../../utils/CrossLog.hpp"

/* return current time in milliseconds */
static double getCurrentTimeMillis(void);

CubeDetectorBehavior::CubeDetectorBehavior() : CubeDetectorBehavior(nullptr) {
    //empty default constructor
}

CubeDetectorBehavior::CubeDetectorBehavior(std::shared_ptr<ImageSaver> imageSaver) :
        imageSaver(imageSaver),
        colorDetector(std::unique_ptr<ColorDetector>(new ColorDetector(imageSaver))),
        morphOpStructuringElement(cv::getStructuringElement(CV_SHAPE_CROSS, cv::Size(3, 3))) {}

CubeDetectorBehavior::~CubeDetectorBehavior() {
    if (debuggable) {
        LOG_DEBUG("RubikJniPart.cpp", "CubeDetectorBehavior - destructor.");
    }
}

void CubeDetectorBehavior::setImageDimensions(int width, int height) {
    imageWidth = width;
    imageHeight = height;
    totalRequiredMemory = width * height * 4 + width * (height + height / 2);
    rgbaImageOffset = width * (height + height / 2);
    rgbaImageSize = width * height * 4;
    nv21ImageSize = width * (height + height / 2);
    largestDimension = width > height ? width : height;
    if (largestDimension > DEFAULT_DIMENSION) {
        upscalingRatio = (float) largestDimension / DEFAULT_DIMENSION;
        downscalingRatio = (float) DEFAULT_DIMENSION / largestDimension;
        if (largestDimension == imageHeight) {
            processingHeight = round(imageHeight * downscalingRatio);
            processingWidth = round(imageHeight * downscalingRatio *
                                    ((float) imageWidth / imageHeight));
        } else {
            processingWidth = round(imageWidth * downscalingRatio);
            processingHeight = round(imageWidth * downscalingRatio *
                                     ((float) imageHeight / imageWidth));
        }
    } else {
        upscalingRatio = 1.0f;
        downscalingRatio = 1.0f;
        processingHeight = imageHeight;
        processingWidth = imageWidth;
    }
    minValidShapeArea = (int) (processingWidth * processingHeight *
                               MIN_VALID_SHAPE_TO_IMAGE_AREA_RATIO);
    maxShapeSideSize = (int) (std::max(processingWidth, processingHeight) *
                              MIN_VALID_SHAPE_TO_IMAGE_SIDE_SIZE_RATIO);
}

void
CubeDetectorBehavior::findCube(cv::Mat &rgbaMat) {
    cv::Mat greyMat;
    cv::cvtColor(rgbaMat, greyMat, CV_RGBA2GRAY);
    cv::Mat resultFrame = rgbaMat.clone();
    performCannyProcessing(rgbaMat, greyMat, resultFrame);
}

void
CubeDetectorBehavior::findCube(const uint8_t *imageData, const int dataLength) {

    cv::Mat frameNV21(imageHeight + imageHeight / 2, imageWidth, CV_8UC1,
                      (uchar *) imageData);

    cv::Mat frameRgba(imageHeight, imageWidth, CV_8UC4,
                      (uchar *) imageData + rgbaImageOffset);

//
//    LOG_DEBUG("RubikMemoryInfo",
//              "frameGrey: chanels: %d,\n"
//                      "width: %d,\n"
//                      "height: %d,\n"
//                      "depth: %d,\n"
//                      "dataStart: %p,\n"
//                      "dataEnd: %p,\n"
//                      "dataLimit: %p,\n"
//                      "sizeWEnd: %d\n"
//                      "sizeWLimit: %d\n"
//                      "computedSize: %d\n"
//                      "imageData: %p",
//              frameGrey.channels(),
//              frameGrey.cols,
//              frameGrey.rows,
//              frameGrey.depth(),
//              (void *) frameGrey.datastart,
//              (void *) frameGrey.dataend,
//              (void *) frameGrey.datalimit,
//              frameGrey.dataend - frameGrey.datastart,
//              frameGrey.datalimit - frameGrey.datastart,
//              frameGrey.total() * frameGrey.elemSize(),
//              imageData);
//
//    LOG_DEBUG("RubikMemoryInfo",
//              "frameNV21: chanels: %d,\n"
//                      "width: %d,\n"
//                      "height: %d,\n"
//                      "depth: %d,\n"
//                      "dataStart: %p,\n"
//                      "dataEnd: %p,\n"
//                      "dataLimit: %p,\n"
//                      "sizeWEnd: %d\n"
//                      "sizeWLimit: %d\n"
//                      "computedSize: %d\n"
//                      "imageData: %p",
//              frameNV21.channels(),
//              frameNV21.cols,
//              frameNV21.rows,
//              frameNV21.depth(),
//              (void *) frameNV21.datastart,
//              (void *) frameNV21.dataend,
//              (void *) frameNV21.datalimit,
//              frameNV21.dataend - frameNV21.datastart,
//              frameNV21.datalimit - frameNV21.datastart,
//              frameNV21.total() * frameNV21.elemSize(),
//              imageData);
//
//    LOG_DEBUG("RubikMemoryInfo",
//              "frameRgba pseudo RGBA: chanels: %d,\n"
//                      "width: %d,\n"
//                      "height: %d,\n"
//                      "depth: %d,\n"
//                      "dataStart: %p,\n"
//                      "dataEnd: %p,\n"
//                      "dataLimit: %p,\n"
//                      "sizeWEnd: %d\n"
//                      "sizeWLimit: %d\n"
//                      "computedSize: %d",
//              frameRgba.channels(),
//              frameRgba.cols,
//              frameRgba.rows,
//              frameRgba.depth(),
//              (void *) frameRgba.datastart,
//              (void *) frameRgba.dataend,
//              (void *) frameRgba.datalimit,
//              frameRgba.dataend - frameRgba.datastart,
//              frameRgba.datalimit - frameRgba.datastart,
//              frameRgba.total() * frameRgba.elemSize());

    cv::cvtColor(frameNV21, frameRgba, cv::COLOR_YUV2RGBA_NV21);

    cv::Mat procRgbaFrame;
    cv::Mat frameGrey;
    if (imageHeight != processingHeight || imageWidth != processingWidth) {
        cv::resize(frameRgba, procRgbaFrame, cv::Size(processingWidth, processingHeight));
        cv::cvtColor(procRgbaFrame, frameGrey, CV_RGBA2GRAY);
    } else {
        procRgbaFrame = cv::Mat(frameRgba);
        frameGrey = frameNV21(cv::Rect(0, 0, imageWidth, imageHeight));
    }

    LOG_DEBUG("RubikMemoryInfo",
              "afterResize. RGBA: width: %d,\n"
                      "height: %d,\n"
                      "depth: %d,\n"
                      "dataStart: %p,\n"
                      "dataEnd: %p,\n"
                      "dataLimit: %p,\n"
                      "computedSize: %d\n"
                      "GRAY: width: %d,\n"
                      "height: %d,\n"
                      "depth: %d,\n"
                      "dataStart: %p,\n"
                      "dataEnd: %p,\n"
                      "dataLimit: %p,\n"
                      "computedSize: %d",
              procRgbaFrame.cols,
              procRgbaFrame.rows,
              procRgbaFrame.depth(),
              (void *) procRgbaFrame.datastart,
              (void *) procRgbaFrame.dataend,
              (void *) procRgbaFrame.datalimit,
              procRgbaFrame.total() * procRgbaFrame.elemSize(),
              frameGrey.cols,
              frameGrey.rows,
              frameGrey.depth(),
              (void *) frameGrey.datastart,
              (void *) frameGrey.dataend,
              (void *) frameGrey.datalimit,
              frameGrey.total() * frameGrey.elemSize());

//    LOG_DEBUG("RubikMemoryInfo",
//              "frameRgba real RGBA: chanels: %d,\n"
//                      "width: %d,\n"
//                      "height: %d,\n"
//                      "depth: %d,\n"
//                      "dataStart: %p,\n"
//                      "dataEnd: %p,\n"
//                      "dataLimit: %p,\n"
//                      "sizeWEnd: %d\n"
//                      "sizeWLimit: %d\n"
//                      "computedSize: %d",
//              frameRgba.channels(),
//              frameRgba.cols,
//              frameRgba.rows,
//              frameRgba.depth(),
//              (void *) frameRgba.datastart,
//              (void *) frameRgba.dataend,
//              (void *) frameRgba.datalimit,
//              frameRgba.dataend - frameRgba.datastart,
//              frameRgba.datalimit - frameRgba.datastart,
//              frameRgba.total() * frameRgba.elemSize());

    performCannyProcessing(procRgbaFrame, frameGrey, frameRgba);
}

void
CubeDetectorBehavior::setOnCubeDetectionResultListener(OnCubeDetectionResultListener *listener) {
    onCubeDetectionResultListener = std::unique_ptr<OnCubeDetectionResultListener>(listener);
}

void CubeDetectorBehavior::setDebuggable(const bool isDebuggable) {
    if (debuggable) {
        LOG_DEBUG("RUBIK_JNI_PART.cpp", "setDebuggable. current:%d, new: %d, frameNumber: %d",
                  debuggable, isDebuggable, frameNumber);
    }
//    frameNumber++;
    debuggable = isDebuggable;
    colorDetector->setDebuggable(isDebuggable);
}

bool CubeDetectorBehavior::isDebuggable() {
    return debuggable;
}

int
CubeDetectorBehavior::getSmallestMargin(Circle referenceCircle, std::vector<Circle> validCircles) {
    int margin = 32768;
    for (int i = 0; i < validCircles.size(); i++) {
        Circle testedCircle = validCircles[i];
        if (referenceCircle.contains(testedCircle.center) ||
            (testedCircle.center.x >
             referenceCircle.center.x + 3 * referenceCircle.radius + 2 * 10) ||
            (testedCircle.center.y >
             referenceCircle.center.y + 3 * referenceCircle.radius + 2 * 10)) {
            //if the center of the current rectangle/circle is  within the current reference, or is either too far from
            //the current rectangle (either to the left or to the right), then just skip it
            //TODO replace magic number 2*10(pixels)
            continue;
        }
        int currentMargin =
                (int) utils::pointsDistance(referenceCircle.center, testedCircle.center) -
                referenceCircle.radius - testedCircle.radius;
        if (currentMargin < margin && currentMargin > 0) {
            margin = currentMargin;
        }
    }

    return (margin == 32768 || margin > referenceCircle.radius) ? 10 : margin;
}

int CubeDetectorBehavior::getPositionInVector(int i, int j) {
    return i * 3 + j - 1;
}

cv::Scalar CubeDetectorBehavior::getColorAsScalar(int color) {
    switch (color) {
        case WHITE:
            return cv::Scalar(255, 255, 255, 255);
        case ORANGE:
            return cv::Scalar(255, 127, 0, 255);
        case YELLOW:
            return cv::Scalar(255, 255, 0, 255);
        case GREEN:
            return cv::Scalar(20, 240, 20, 255);
        case BLUE:
            return cv::Scalar(0, 0, 200, 255);
        case RED:
            return cv::Scalar(255, 0, 0, 255);
        default:
            return cv::Scalar(-1, -1, -1, 255);
    }
}

void
CubeDetectorBehavior::performCannyProcessing(cv::Mat &frameRgba, cv::Mat &frameGray,
                                             cv::Mat &resultFrame) {
    double startTime = getCurrentTimeMillis();
    std::vector<std::vector<cv::Point>> contours = detectContours(frameGray);

    std::vector<cv::RotatedRect> filteredRectangles;
    std::vector<Circle> filteredRectanglesInnerCircles;

    filterContours(frameRgba, contours, filteredRectangles, filteredRectanglesInnerCircles);
    Circle referenceCircle;
    for (int i = 0; i < filteredRectanglesInnerCircles.size(); i++) {
        referenceCircle = filteredRectanglesInnerCircles[i];
        //test each and every rectangle
        std::vector<Circle> potentialFacelets = findPotentialFacelets(referenceCircle,
                                                                      filteredRectanglesInnerCircles,
                                                                      i);
        if (potentialFacelets.size() < 3) {
            //if we have at most 2 other rectangles that have an area similar to the initial one, just continue, can't be a cube facet
            continue;
        }

        int margin = getSmallestMargin(referenceCircle, potentialFacelets);
        int diameterWithMargin = referenceCircle.radius * 2 + margin;
        std::vector<Circle> estimatedFacelets = estimateRemainingFaceletsPos(referenceCircle,
                                                                             diameterWithMargin);

        std::vector<std::vector<Circle>> facetModel = matchEstimatedWithPotentialFacelets(
                potentialFacelets, estimatedFacelets);
        facetModel[0][0] = referenceCircle;
        bool cubeFound = verifyIfCubeFound(facetModel);
        if (imageSaver != nullptr) {
            saveWholeFrame(frameRgba);
            saveFilteredRectangles(frameRgba, filteredRectangles);
        }
        if (cubeFound) {
            fillMissingFacelets(estimatedFacelets, facetModel);
            std::vector<std::vector<int>> colors = detectFacetColors(frameRgba, facetModel);
            if (shouldDrawFoundFacelets) {
                //we draw facelets in the result frame, not in the processing frame
                drawFoundFacelets(resultFrame, facetModel, colors);
            }
            if (debuggable) {
                saveDebugData(frameRgba,
                              filteredRectangles,
                              referenceCircle,
                              potentialFacelets,
                              estimatedFacelets,
                              colors);
            }
            onCubeDetectionResultListener->onCubeDetectionResult(colors);
            //break from iteration, cube was found in current frame. no need to continue
            break;
        }
    }

    //BASICALLY DONE
    double endTime = getCurrentTimeMillis();
    double delta = endTime - startTime;
    double fps = 1000 / delta;
    frameRateSum += fps;
    frameNumber++;
    if (debuggable) {
        float frameRateAverage = (float) frameRateSum / frameNumber;
        LOG_DEBUG("RUBIK_JNI_PART.cpp",
                  "frameNumber: %d, frameRate current frame: %.2f, frameRateAverage: %.2f, frameRate: %.2f",
                  frameNumber, fps, frameRateAverage, fps);
    }
    //end
}

void CubeDetectorBehavior::drawFoundFacelets(cv::Mat &procRgbaFrame,
                                             const std::vector<std::vector<Circle>> &facetModel,
                                             const std::vector<std::vector<int>> &colors) {
    for (int j = 0; j < 3; j++) {
        for (int k = 0; k < 3; k++) {
            Circle detectedCircle = facetModel[j][k];
//            utils::drawCircle(currentFrame, detectedCircle, getColorAsScalar(colors[j][k]),
//                              scalingRatio);

            cv::circle(procRgbaFrame,
                       cv::Point(round(detectedCircle.center.x *
                                       ((float) imageWidth / processingWidth)),
                                 round(detectedCircle.center.y *
                                       ((float) imageHeight / processingHeight))),
                       detectedCircle.radius * upscalingRatio,
                       getColorAsScalar(colors[j][k]),
                       (int) round(2 * upscalingRatio),
                       CV_AA, 0);
        }
    }
}

bool CubeDetectorBehavior::verifyIfCubeFound(
        const std::vector<std::vector<Circle>> &cubeFacet) const {
    bool cubeFound = false;
    //verify if valid cube model
    //top left element is always considered to be 1. if any of these is 0, then it means no sticker was detected on the specific row
    int topRow = 100 * !cubeFacet[0][0].isEmpty() +
                 10 * !cubeFacet[0][1].isEmpty() +
                 !cubeFacet[0][2].isEmpty();
    int middleRow = 100 * !cubeFacet[1][0].isEmpty() +
                    10 * !cubeFacet[1][1].isEmpty() +
                    !cubeFacet[1][2].isEmpty();
    int bottomRow = 100 * !cubeFacet[2][0].isEmpty() +
                    10 * !cubeFacet[2][1].isEmpty() +
                    !cubeFacet[2][2].isEmpty();

    switch (topRow) {
        case 101:
        case 111:
            //these two cases can be treated in the same manner
            if (middleRow != 0 && bottomRow != 100 &&
                bottomRow != 10 &&
                bottomRow != 1 &&
                bottomRow != 0) {
                cubeFound = true;
            }
            break;
        case 110:
            if ((middleRow == 1 || middleRow == 11 || middleRow == 101 || middleRow == 111) &&
                bottomRow != 100 &&
                bottomRow != 10 &&
                bottomRow != 1 &&
                bottomRow != 0) {
                cubeFound = true;
            } else if ((middleRow == 10 || middleRow == 100 || middleRow == 110) &&
                       (bottomRow == 11 || bottomRow == 101 || bottomRow == 111)) {
                cubeFound = true;
            }
            break;
        default:
            break;
    }
    return cubeFound;
}

std::vector<std::vector<Circle>>
CubeDetectorBehavior::matchEstimatedWithPotentialFacelets(
        const std::vector<Circle> &potentialFacelets,
        const std::vector<Circle> &estimatedFacelets) {

    std::vector<std::vector<Circle>> facetModel(3, std::vector<Circle>(3));

    for (int i = 0; i < 8; i++) {
        Circle testedCircle = estimatedFacelets[i];
        for (int j = 0; j < potentialFacelets.size(); j++) {
            if (potentialFacelets[j].contains(testedCircle.center)) {

                float R = std::max(testedCircle.radius, potentialFacelets[j].radius);
                float r = std::min(testedCircle.radius, potentialFacelets[j].radius);
                float d = utils::pointsDistance(testedCircle.center, potentialFacelets[j].center);

                float part1 = r * r * std::acos((d * d + r * r - R * R) / (2 * d * r));
                float part2 = R * R * std::acos((d * d + R * R - r * r) / (2 * d * R));
                float part3 =
                        0.5f * std::sqrt((-d + r + R) * (d + r - R) * (d - r + R) * (d + r + R));
                float intersectionArea = part1 + part2 - part3;

                float areasRatio = potentialFacelets[j].area / intersectionArea;
                if (areasRatio < 1.45f) {
                    int auxI = (i + 1) / 3;
                    int auxJ = (i + 1) % 3;
                    facetModel[auxI][auxJ] = potentialFacelets[j];
                    //already found a matching rectangle, we are not interested in others. just continue
                    continue;
                }
            }
        }
    }
    return facetModel;
}

std::vector<Circle>
CubeDetectorBehavior::estimateRemainingFaceletsPos(const Circle &referenceCircle,
                                                   int diameterWithMargin) const {//draw the remaining rectangles
    std::vector<Circle> newCircles;

    float angle = referenceCircle.angle;
    float xOffset = diameterWithMargin * std::cos(angle);
    float yOffset = diameterWithMargin * std::sin(angle);
    float xOffset2 = diameterWithMargin * (float) cos(angle + CV_PI / 2);
    float yOffset2 = diameterWithMargin * (float) sin(angle + CV_PI / 2);
    Circle secondRowFirstCircle = Circle(cv::Point2f(referenceCircle.center.x + xOffset2,
                                                     referenceCircle.center.y + yOffset2),
                                         referenceCircle.radius, referenceCircle.angle);
    Circle thirdRowFirstCircle = Circle(cv::Point2f(referenceCircle.center.x + 2 * xOffset2,
                                                    referenceCircle.center.y + 2 * yOffset2),
                                        referenceCircle.radius, referenceCircle.angle);

    newCircles.push_back(Circle(cv::Point2f(referenceCircle.center.x + xOffset,
                                            referenceCircle.center.y + yOffset),
                                referenceCircle.radius, referenceCircle.angle));
    newCircles.push_back(Circle(cv::Point2f(referenceCircle.center.x + 2 * xOffset,
                                            referenceCircle.center.y + 2 * yOffset),
                                referenceCircle.radius, referenceCircle.angle));
    newCircles.push_back(secondRowFirstCircle);
    newCircles.push_back(Circle(cv::Point2f(secondRowFirstCircle.center.x + xOffset,
                                            secondRowFirstCircle.center.y + yOffset),
                                referenceCircle.radius, referenceCircle.angle));
    newCircles.push_back(Circle(cv::Point2f(secondRowFirstCircle.center.x + 2 * xOffset,
                                            secondRowFirstCircle.center.y + 2 * yOffset),
                                referenceCircle.radius, referenceCircle.angle));
    newCircles.push_back(thirdRowFirstCircle);
    newCircles.push_back(Circle(cv::Point2f(thirdRowFirstCircle.center.x + xOffset,
                                            thirdRowFirstCircle.center.y + yOffset),
                                referenceCircle.radius, referenceCircle.angle));
    newCircles.push_back(Circle(cv::Point2f(thirdRowFirstCircle.center.x + 2 * xOffset,
                                            thirdRowFirstCircle.center.y + 2 * yOffset),
                                referenceCircle.radius, referenceCircle.angle));
    return newCircles;
}

std::vector<Circle> CubeDetectorBehavior::findPotentialFacelets(const Circle &referenceCircle,
                                                                const std::vector<Circle> &innerCircles,
                                                                int referenceCircleIndex) const {
    //only have rectangles that have an area similar to the initial one
    Circle testedCircle;
    std::vector<Circle> foundCircles;
    for (int j = 0; j < innerCircles.size(); j++) {
        if (referenceCircleIndex != j) {
            testedCircle = innerCircles[j];
        } else {
            continue;
        }
        float maxArea = (float) std::max(referenceCircle.area, testedCircle.area);
        int minArea = std::min(referenceCircle.area, testedCircle.area);
        float ratio = maxArea / minArea;
        float angleDifference = (float) std::abs((referenceCircle.angle - testedCircle.angle) *
                                                 180 / CV_PI);
        if (ratio < 1.35f && angleDifference < 15.0f) {
            foundCircles.push_back(testedCircle);
        }
    }
    return foundCircles;
}

void CubeDetectorBehavior::filterContours(const cv::Mat &currentFrame,
                                          const std::vector<std::vector<cv::Point>> &contours,
                                          std::vector<cv::RotatedRect> &possibleFacelets,
                                          std::vector<Circle> &possibleFaceletsInnerCircles) const {
//    cv::Mat drawing = (cv::Mat) cv::Mat::zeros(currentFrame.size(), CV_8UC3);
    /// Approximate contours to polygons + get bounding rects and circles
    for (int i = 0; i < contours.size(); i++) {
        //approximate contour to a polygon, save it.
        cv::RotatedRect currentRect = cv::minAreaRect(contours[i]);
//        drawRectangleToMat(drawing, currentRect);
        //calculate aspect aspectRatio
        float maxDimensionValue = (float) std::max(currentRect.size.height, currentRect.size.width);
        float minDimensionValue = std::min(currentRect.size.height, currentRect.size.width);
        float aspectRatio = maxDimensionValue / minDimensionValue;
        //verify contour aspect ratio, size and area.
        if (aspectRatio < 1.2f &&
            currentRect.size.area() > minValidShapeArea &&
            currentRect.size.height < maxShapeSideSize &&
            currentRect.size.width < maxShapeSideSize) {
            //if based on the above conditions it is considered as being a valid contour,
            //then save it for later processing
            possibleFacelets.push_back(currentRect);
            possibleFaceletsInnerCircles.push_back(Circle(currentRect));
        }
    }
//    utils::quickSaveImage(drawing, "/storage/emulated/0/RubikResults", frameNumber, 1811);
}

std::vector<std::vector<cv::Point>>
CubeDetectorBehavior::detectContours(const cv::Mat &frameGray) const {
//    utils::quickSaveImage(frameGray, "/storage/emulated/0/RubikResults", frameNumber, 1800);

    /// Reduce noise with a kernel
    cv::blur(frameGray, frameGray, cv::Size(BLUR_KERNEL_SIZE, BLUR_KERNEL_SIZE));
    // Canny detector
    cv::Canny(frameGray, frameGray, CANNY_LOW_THRESHOLD,
              CANNY_LOW_THRESHOLD * CANNY_THRESHOLD_RATIO, CANNY_APERTURE_SIZE, true);
//    cv::morphologyEx(frameGray, frameGray, CV_MOP_CLOSE, morphOpStructuringElement,
//                     cv::Point(-1, -1), MORPH_OP_ITERATIONS);
//    utils::quickSaveImage(frameGray, "/storage/emulated/0/RubikResults", frameNumber, 1801);
    std::vector<cv::Vec4i> hierarchy;
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(frameGray, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE,
                     cv::Point(0, 0));
//    LOG_DEBUG("CATA", "nr contours: %d", contours.size());
    return contours;
}

void CubeDetectorBehavior::saveDebugData(const cv::Mat &currentFrame,
                                         const std::vector<cv::RotatedRect> &filteredRectangles,
                                         const Circle &referenceCircle,
                                         const std::vector<Circle> &potentialFacelets,
                                         const std::vector<Circle> &estimatedFacelets,
                                         const std::vector<std::vector<int>> colors) {
    ///BEGIN PRINT
    if (imageSaver != nullptr) {
        ///save whole frame
        saveWholeFrame(currentFrame);

        ///save filtered rectangles
        cv::Mat drawing = saveFilteredRectangles(currentFrame, filteredRectangles);

        ///save potential facelets
        drawing = cv::Mat::zeros(currentFrame.size(), CV_8UC3);
        utils::drawCircles(drawing, potentialFacelets, cv::Scalar(255, 0, 0), upscalingRatio);
        utils::drawCircle(drawing, referenceCircle, cv::Scalar(0, 0, 255), upscalingRatio);
        imageSaver->saveImage(drawing, frameNumber, "potential_facelets");

        ///save estimated facelets
        drawing = cv::Mat::zeros(currentFrame.size(), CV_8UC3);
        utils::drawCircles(drawing, estimatedFacelets, cv::Scalar(0, 255, 80), upscalingRatio);
        utils::drawCircle(drawing, referenceCircle, cv::Scalar(0, 0, 255), upscalingRatio);
        imageSaver->saveImage(drawing, frameNumber, "estimated_facelets");

        ///save potential & estimated facelets which match
        ///recompute the incomplete facelet model, in order to print only the facelets which passed through all the filtering steps
        ///it's a waste indeed to compute them once more...but then again, this is for debugging so..
        std::vector<std::vector<Circle>> faceletIncompleteModel = matchEstimatedWithPotentialFacelets(
                potentialFacelets, estimatedFacelets);

        drawing = cv::Mat::zeros(currentFrame.size(), CV_8UC3);
        utils::drawCircles(drawing, estimatedFacelets, cv::Scalar(0, 255, 80), upscalingRatio);
        utils::drawCircles(drawing, faceletIncompleteModel, cv::Scalar(255, 0, 0), upscalingRatio,
                           3,
                           true);
        utils::drawCircle(drawing, referenceCircle, cv::Scalar(0, 0, 255), upscalingRatio);
        utils::drawCircle(drawing, referenceCircle, cv::Scalar(255, 0, 0), upscalingRatio, 3, true);
        imageSaver->saveImage(drawing, frameNumber, "matched_pot_est_facelets");

        ///save just the facelets which matched with the estimated ones
        drawing = cv::Mat::zeros(currentFrame.size(), CV_8UC3);
        utils::drawCircles(drawing, faceletIncompleteModel, cv::Scalar(0, 255, 0), upscalingRatio,
                           3);
        utils::drawCircle(drawing, referenceCircle, cv::Scalar(0, 0, 255), upscalingRatio);
        imageSaver->saveImage(drawing, frameNumber, "matched_potential_facelets");
    }

    LOG_DEBUG("RUBIK_JNI_PART.cpp",
              "COLORS: [1]:{ %d %d %d } [2]:{ %d %d %d } [3]:{ %d %d %d }",
              colors[0][0], colors[0][1], colors[0][2], colors[1][0],
              colors[1][1], colors[1][2], colors[2][0], colors[2][1],
              colors[2][2]);
//    end debug data saving
}

void CubeDetectorBehavior::saveWholeFrame(const cv::Mat &currentFrame) const {
    cv::Mat bgr;
    cvtColor(currentFrame, bgr, CV_RGBA2BGR);
    imageSaver->saveImage(bgr, frameNumber, "full_frame");
}

cv::Mat CubeDetectorBehavior::saveFilteredRectangles(const cv::Mat &currentFrame,
                                                     const std::vector<cv::RotatedRect> &filteredRectangles) const {
    cv::Mat drawing = (cv::Mat) cv::Mat::zeros(currentFrame.size(), CV_8UC3);
    for (int i = 0; i < filteredRectangles.size(); i++) {
        // rotated rectangle
        cv::Point2f rect_points[4];
        filteredRectangles[i].points(rect_points);
        for (int j = 0; j < 4; j++)
            line(drawing, rect_points[j], rect_points[(j + 1) % 4],
                 cv::Scalar(0, 255, 0), 1, CV_AA);
    }
    imageSaver->saveImage(drawing, frameNumber, "filtered_rectangles");
    return drawing;
}


void CubeDetectorBehavior::drawRectangleToMat(const cv::Mat &currentFrame,
                                              const cv::RotatedRect &rotatedRect,
                                              const cv::Scalar color) const {
    // rotated rectangle
    cv::Point2f rect_points[4];
    rotatedRect.points(rect_points);
    for (int j = 0; j < 4; j++)
        line(currentFrame, rect_points[j], rect_points[(j + 1) % 4],
             color, 1, CV_AA);
}

void CubeDetectorBehavior::fillMissingFacelets(const std::vector<Circle> &estimatedFacelets,
                                               std::vector<std::vector<Circle>> &facetModel) {
    for (int j = 0; j < 3; j++) {
        for (int k = 0; k < 3; k++) {
            if (facetModel[j][k].isEmpty()) {
                facetModel[j][k] =
                        estimatedFacelets[getPositionInVector(j, k)];
            }
        }
    }
}

std::vector<std::vector<int>>
CubeDetectorBehavior::detectFacetColors(const cv::Mat &currentFrame,
                                        const std::vector<std::vector<Circle>> facetModel) {
    std::vector<std::vector<int>> colors(3, std::vector<int>(3));
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            Circle detectedCircle = facetModel[i][j];
            if (0 <= (detectedCircle.center.x - detectedCircle.radius) &&
                0 <= (2 * detectedCircle.radius) &&
                (detectedCircle.center.x + detectedCircle.radius) <= currentFrame.cols &&
                0 <= (detectedCircle.center.y - detectedCircle.radius) &&
                (detectedCircle.center.y + detectedCircle.radius) <= currentFrame.rows) {
                cv::Rect roi = cv::Rect(
                        cv::Point(detectedCircle.center.x - detectedCircle.radius,
                                  detectedCircle.center.y - detectedCircle.radius),
                        cv::Point(detectedCircle.center.x + detectedCircle.radius,
                                  detectedCircle.center.y + detectedCircle.radius)
                );

                cv::Mat stickerRoiHSV;
                // Convert the image to HSV
                cv::cvtColor(currentFrame(roi), stickerRoiHSV, CV_RGB2HSV);

                float whiteMinRatio;
                if (i == 1 && j == 1) {
                    whiteMinRatio = 0.44f;
                } else {
                    whiteMinRatio = 0.5f;
                }
                colors[i][j] = colorDetector->detectColor(stickerRoiHSV, whiteMinRatio,
                                                          frameNumber,
                                                          i * 10 + j);
            } else {
                colors[i][j] = WHITE;
                if (debuggable) {
                    LOG_DEBUG("RUBIK_JNI_PART.cpp",
                              "frameNumberOld: %d FOUND INVALID RECT WHEN DETECTING COLORS",
                              frameNumber);
                }
            }
        }
    }
    return colors;
}

int CubeDetectorBehavior::getTotalRequiredMemory() {
    return totalRequiredMemory;
}

int CubeDetectorBehavior::getRgbaImageOffset() {
    return rgbaImageOffset;
}

int CubeDetectorBehavior::getRgbaImageSize() {
    return rgbaImageSize;
}

int CubeDetectorBehavior::getNv21ImageSize() {
    return nv21ImageSize;
}

int CubeDetectorBehavior::getNv21ImageOffset() {
    return rgbaImageOffset;
}

void CubeDetectorBehavior::setShouldDrawFoundFacelets(bool shouldDrawFoundFacelets) {
    CubeDetectorBehavior::shouldDrawFoundFacelets = shouldDrawFoundFacelets;
}

/* return current time in milliseconds */
static double getCurrentTimeMillis(void) {
    struct timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    return 1000.0 * res.tv_sec + (double) res.tv_nsec / 1e6;
}
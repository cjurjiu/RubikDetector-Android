//// Created by catalin on 12.07.2017.
//

#include <math.h>
#include "SimpleFaceletsDetectorBehavior.hpp"
#include "SimpleFaceletsDetector.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "../../utils/Utils.hpp"
#include "../../utils/CrossLog.hpp"

/**##### PUBLIC API #####**/
SimpleFaceletsDetectorBehavior::SimpleFaceletsDetectorBehavior() :
        SimpleFaceletsDetectorBehavior(nullptr) {
    //empty default constructor
}

SimpleFaceletsDetectorBehavior::SimpleFaceletsDetectorBehavior(
        std::shared_ptr<ImageSaver> imageSaver) :
        imageSaver(imageSaver) {
}

SimpleFaceletsDetectorBehavior::~SimpleFaceletsDetectorBehavior() {
    if (debuggable) {
        LOG_DEBUG("RubikJniPart.cpp", "SimpleFaceletsDetectorBehavior - destructor.");
    }
}

std::vector<std::vector<RubikFacelet>>
SimpleFaceletsDetectorBehavior::findFacelets(cv::Mat &frameRgba, cv::Mat &frameGray,
                                             const int frameNumber) {
    if (debuggable) {
        LOG_DEBUG("RubikJniPart.cpp",
                  "SimpleFaceletsDetectorBehavior - findFacelets. isDebuggable:%d", debuggable);
    }

    std::vector<std::vector<RubikFacelet>> facelets(0);

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
            if (debuggable) {
                LOG_DEBUG("RubikJniPart.cpp",
                          "SimpleFaceletsDetectorBehavior - didn't find at least 2 other rectangles similar to reference.");
            }
            //if we have at most 2 other rectangles that have an area similar to the initial one, just continue, can't be a cube facet
            continue;
        }

        float margin = getSmallestMargin(referenceCircle, potentialFacelets);
        std::vector<Circle> estimatedFacelets = estimateRemainingFaceletsPositions(referenceCircle,
                                                                                   margin);

        std::vector<std::vector<Circle>> facetModel = matchEstimatedWithPotentialFacelets(
                potentialFacelets, estimatedFacelets);
        facetModel[0][0] = referenceCircle;
        bool cubeFound = verifyIfCubeFound(facetModel);
        if (debuggable && imageSaver != nullptr) {
            saveWholeFrame(frameRgba, frameNumber * 10 + i + 1);
            cv::Mat drawing = saveFilteredRectangles(frameRgba, filteredRectangles,
                                                     frameNumber * 10 + i + 1);

            utils::drawCircles(drawing, estimatedFacelets, cv::Scalar(0, 255, 80));
            utils::drawCircles(drawing, facetModel, cv::Scalar(255, 0, 0));
            utils::drawCircle(drawing, referenceCircle, cv::Scalar(255, 0, 0));
            utils::drawCircle(drawing, referenceCircle, cv::Scalar(0, 0, 255), 1, 2, true);
            imageSaver->saveImage(drawing, frameNumber * 10 + i + 1, "matched_pot_est_facelets1");

            ///save just the facelets which matched with the estimated ones
            drawing = cv::Mat::zeros(frameRgba.size(), CV_8UC3);
            utils::drawCircles(drawing, facetModel, cv::Scalar(0, 255, 80));
            utils::drawCircle(drawing, referenceCircle, cv::Scalar(0, 0, 255), 1, 2, true);
            imageSaver->saveImage(drawing, frameNumber * 10 + i + 1, "matched_potential_facelets1");

        }
        if (cubeFound) {
            if (debuggable) {
                LOG_DEBUG("RubikJniPart.cpp",
                          "SimpleFaceletsDetectorBehavior - found cube!");
            }
            fillMissingFacelets(estimatedFacelets, facetModel);
            //dummy colors for now
            std::vector<std::vector<RubikFacelet::Color>> colors(3,
                                                                 std::vector<RubikFacelet::Color>(
                                                                         3));

            if (debuggable) {
                LOG_DEBUG("RubikJniPart.cpp",
                          "SimpleFaceletsDetectorBehavior - creating the result from internal model");
            }
            //create result
            facelets = createResult(facetModel);

            saveDebugData(frameRgba,
                          filteredRectangles,
                          referenceCircle,
                          potentialFacelets,
                          estimatedFacelets,
                          colors,
                          frameNumber);
            //break from iteration, cube was found in current frame. no need to continue
            break;
        } else {
            if (debuggable) {
                LOG_DEBUG("RubikJniPart.cpp",
                          "SimpleFaceletsDetectorBehavior - cube not found. cubeFound: %d",
                          cubeFound);
            }
        }
    }

    if (debuggable) {
        LOG_DEBUG("RubikJniPart.cpp",
                  "SimpleFaceletsDetectorBehavior - end, returning facelets of size: %d",
                  facelets.size());
    }
    return facelets;
    //end
}

void
SimpleFaceletsDetectorBehavior::onFrameSizeSelected(int processingWidth, int processingHeight) {
    minValidShapeArea = (int) (processingWidth * processingHeight *
                               MIN_VALID_SHAPE_TO_IMAGE_AREA_RATIO);
    maxShapeSideSize = (int) (std::max(processingWidth, processingHeight) *
                              MIN_VALID_SHAPE_TO_IMAGE_SIDE_SIZE_RATIO);
}

void SimpleFaceletsDetectorBehavior::setDebuggable(const bool isDebuggable) {
    debuggable = isDebuggable;
}

bool SimpleFaceletsDetectorBehavior::isDebuggable() const {
    return debuggable;
}

std::vector<std::vector<cv::Point>> SimpleFaceletsDetectorBehavior::detectContours(
        const cv::Mat &frameGray) const {
    /// Reduce noise with a kernel
    cv::blur(frameGray, frameGray, cv::Size(BLUR_KERNEL_SIZE, BLUR_KERNEL_SIZE));
    // Canny detector
    cv::Canny(frameGray, frameGray, CANNY_LOW_THRESHOLD,
              CANNY_LOW_THRESHOLD * CANNY_THRESHOLD_RATIO, CANNY_APERTURE_SIZE, true);
    std::vector<cv::Vec4i> hierarchy;
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(frameGray, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE,
                     cv::Point(0, 0));
    return contours;
}

void SimpleFaceletsDetectorBehavior::filterContours(const cv::Mat &currentFrame,
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


float SimpleFaceletsDetectorBehavior::getSmallestMargin(Circle referenceCircle,
                                                        std::vector<Circle> validCircles) {
    float margin = 320.0f;
    for (int i = 0; i < validCircles.size(); i++) {
        Circle testedCircle = validCircles[i];
        if (referenceCircle.contains(testedCircle.center) ||
            (testedCircle.center.x >
             referenceCircle.center.x + 3 * referenceCircle.radius + CIRCLE_DISTANCE_BUFFER) ||
            (testedCircle.center.y >
             referenceCircle.center.y + 3 * referenceCircle.radius + CIRCLE_DISTANCE_BUFFER)) {
            //if the center of the current rectangle/circle is  within the current reference, or is either too far from
            //the current rectangle (either to the left or to the right), then just skip it
            //TODO replace magic number 2*10(pixels)
            continue;
        }
        float currentMargin = utils::pointsDistance(referenceCircle.center, testedCircle.center) -
                              referenceCircle.radius - testedCircle.radius;
        if (currentMargin < margin && currentMargin > 0) {
            margin = currentMargin;
        }
    }

    return (margin >= 320.0f || margin >= referenceCircle.radius) ? 10.f : margin;
}

std::vector<Circle>
SimpleFaceletsDetectorBehavior::findPotentialFacelets(const Circle &referenceCircle,
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

std::vector<Circle> SimpleFaceletsDetectorBehavior::estimateRemainingFaceletsPositions(
        const Circle &referenceCircle,
        float margin) const {
    //draw the remaining rectangles
    std::vector<Circle> newCircles;
    float diameterWithMargin = referenceCircle.radius * 2 + margin;

    float angle = referenceCircle.angle;
    float xOffset = diameterWithMargin * std::cos(angle);
    float yOffset = diameterWithMargin * std::sin(angle);
    float xOffset2 = diameterWithMargin * (float) std::cos(angle + M_PI_2);
    float yOffset2 = diameterWithMargin * (float) std::sin(angle + M_PI_2);
    //create first circles in 2nd and 3rd rows respectively
    Circle secondRowFirstCircle = Circle(referenceCircle,
                                         cv::Point2f(xOffset2, yOffset2));
    Circle thirdRowFirstCircle = Circle(referenceCircle,
                                        cv::Point2f(2 * xOffset2, 2 * yOffset2));

    //complete first row
    newCircles.push_back(Circle(referenceCircle,
                                cv::Point2f(xOffset, yOffset)));
    newCircles.push_back(Circle(referenceCircle,
                                cv::Point2f(2 * xOffset, 2 * yOffset)));

    //push first circle in second row
    newCircles.push_back(secondRowFirstCircle);
    //complete second row
    newCircles.push_back(Circle(secondRowFirstCircle,
                                cv::Point2f(xOffset, yOffset)));
    newCircles.push_back(Circle(secondRowFirstCircle,
                                cv::Point2f(2 * xOffset, 2 * yOffset)));

    //push first circle in third row
    newCircles.push_back(thirdRowFirstCircle);
    //complete third row
    newCircles.push_back(Circle(thirdRowFirstCircle,
                                cv::Point2f(xOffset, yOffset)));
    newCircles.push_back(Circle(thirdRowFirstCircle,
                                cv::Point2f(2 * xOffset, 2 * yOffset)));
    return newCircles;
}

std::vector<std::vector<Circle>>
SimpleFaceletsDetectorBehavior::matchEstimatedWithPotentialFacelets(
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

bool SimpleFaceletsDetectorBehavior::verifyIfCubeFound(
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
                (bottomRow == 111 || bottomRow == 11 || bottomRow == 101 || bottomRow == 110)) {
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

void
SimpleFaceletsDetectorBehavior::fillMissingFacelets(const std::vector<Circle> &estimatedFacelets,
                                                    std::vector<std::vector<Circle>> &facetModel) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (facetModel[i][j].isEmpty()) {
                facetModel[i][j] =
                        estimatedFacelets[i * 3 + j - 1];
            }
        }
    }
}

std::vector<std::vector<RubikFacelet>> SimpleFaceletsDetectorBehavior::createResult(
        const std::vector<std::vector<Circle>> &faceModel) {

    std::vector<std::vector<RubikFacelet>> result(3, std::vector<RubikFacelet>(3));
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result[i][j] = RubikFacelet(Point2d(faceModel[i][j].center.x,
                                                faceModel[i][j].center.y),
                                        faceModel[i][j].originalRectWidth,
                                        faceModel[i][j].originalRectHeight,
                                        faceModel[i][j].angle);
        }
    }
    return result;
}

void
SimpleFaceletsDetectorBehavior::saveWholeFrame(const cv::Mat &currentFrame, int frameNr) const {
    cv::Mat bgr;
    cvtColor(currentFrame, bgr, CV_RGBA2BGR);
    imageSaver->saveImage(bgr, frameNr, "full_frame");
}

cv::Mat SimpleFaceletsDetectorBehavior::saveFilteredRectangles(const cv::Mat &currentFrame,
                                                               const std::vector<cv::RotatedRect> &filteredRectangles,
                                                               int frameNr) const {
    cv::Mat drawing = (cv::Mat) cv::Mat::zeros(currentFrame.size(), CV_8UC3);
    for (int i = 0; i < filteredRectangles.size(); i++) {
        // rotated rectangle
        cv::Point2f rect_points[4];
        filteredRectangles[i].points(rect_points);
        for (int j = 0; j < 4; j++)
            line(drawing, rect_points[j], rect_points[(j + 1) % 4],
                 cv::Scalar(0, 255, 0), 1, CV_AA);
    }
    imageSaver->saveImage(drawing, frameNr, "filtered_rectangles");
    return drawing;
}

void SimpleFaceletsDetectorBehavior::saveDebugData(const cv::Mat &frame,
                                                   const std::vector<cv::RotatedRect> &filteredRectangles,
                                                   const Circle &referenceCircle,
                                                   const std::vector<Circle> &potentialFacelets,
                                                   const std::vector<Circle> &estimatedFacelets,
                                                   const std::vector<std::vector<RubikFacelet::Color>> colors,
                                                   const int frameNumber) {
    if (!debuggable) {
        return;
    }
    LOG_DEBUG("RUBIK_JNI_PART.cpp",
              "SimpleFaceletsDetectorBehavior - savingDebugData. imageSaver!=null: %d", imageSaver !=
                                                                                      nullptr);
    ///BEGIN PRINT
    if (imageSaver != nullptr) {
        ///save whole frame
        saveWholeFrame(frame, frameNumber * 10);

        ///save filtered rectangles
        cv::Mat drawing = saveFilteredRectangles(frame, filteredRectangles, frameNumber * 10);

        ///save potential facelets
        drawing = cv::Mat::zeros(frame.size(), CV_8UC3);
        utils::drawCircles(drawing, potentialFacelets, cv::Scalar(255, 0, 0));
        utils::drawCircle(drawing, referenceCircle, cv::Scalar(0, 0, 255));
        imageSaver->saveImage(drawing, frameNumber * 10, "potential_facelets");

        ///save estimated facelets
        drawing = cv::Mat::zeros(frame.size(), CV_8UC3);
        utils::drawCircles(drawing, estimatedFacelets, cv::Scalar(0, 255, 80));
        utils::drawCircle(drawing, referenceCircle, cv::Scalar(0, 0, 255));
        imageSaver->saveImage(drawing, frameNumber * 10, "estimated_facelets");

        ///save potential & estimated facelets which match
        ///recompute the incomplete facelet model, in order to print only the facelets which passed through all the filtering steps
        ///it's a waste indeed to compute them once more...but then again, this is for debugging so..
        std::vector<std::vector<Circle>> faceletIncompleteModel = matchEstimatedWithPotentialFacelets(
                potentialFacelets, estimatedFacelets);

        drawing = cv::Mat::zeros(frame.size(), CV_8UC3);
        utils::drawCircles(drawing, estimatedFacelets, cv::Scalar(0, 255, 80));
        utils::drawCircles(drawing, faceletIncompleteModel, cv::Scalar(255, 0, 0));
        utils::drawCircle(drawing, referenceCircle, cv::Scalar(255, 0, 0));
        utils::drawCircle(drawing, referenceCircle, cv::Scalar(0, 0, 255), 1, 2, true);
        imageSaver->saveImage(drawing, frameNumber * 10, "matched_pot_est_facelets");

        ///save just the facelets which matched with the estimated ones
        drawing = cv::Mat::zeros(frame.size(), CV_8UC3);
        utils::drawCircles(drawing, faceletIncompleteModel, cv::Scalar(0, 255, 80));
        utils::drawCircle(drawing, referenceCircle, cv::Scalar(0, 0, 255), 1, 2, true);
        imageSaver->saveImage(drawing, frameNumber * 10, "matched_potential_facelets");
    }

    LOG_DEBUG("RUBIK_JNI_PART.cpp",
              "COLORS: [1]:{ %d %d %d } [2]:{ %d %d %d } [3]:{ %d %d %d }",
              utils::asInt(colors[0][0]),
              utils::asInt(colors[0][1]),
              utils::asInt(colors[0][2]),
              utils::asInt(colors[1][0]),
              utils::asInt(colors[1][1]),
              utils::asInt(colors[1][2]),
              utils::asInt(colors[2][0]),
              utils::asInt(colors[2][1]),
              utils::asInt(colors[2][2]));
//    end debug data saving
}

void SimpleFaceletsDetectorBehavior::drawRectangleToMat(const cv::Mat &currentFrame,
                                                        const cv::RotatedRect &rotatedRect,
                                                        const cv::Scalar color) const {
    // rotated rectangle
    cv::Point2f rect_points[4];
    rotatedRect.points(rect_points);
    for (int j = 0; j < 4; j++)
        line(currentFrame, rect_points[j], rect_points[(j + 1) % 4],
             color, 1, CV_AA);
}
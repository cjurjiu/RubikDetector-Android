//// Created by catalin on 12.07.2017.
//

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
        colorDetector(std::unique_ptr<ColorDetector>(new ColorDetector(imageSaver))) {}

CubeDetectorBehavior::~CubeDetectorBehavior() {
    if (debuggable) {
        LOG_DEBUG("RubikJniPart.cpp", "CubeDetectorBehavior - destructor.");
    }
}

void
CubeDetectorBehavior::setOnCubeDetectionResultListener(OnCubeDetectionResultListener *listener) {
    onCubeDetectionResultListener = std::unique_ptr<OnCubeDetectionResultListener>(listener);
}

void CubeDetectorBehavior::findCube(cv::Mat &mat) {
    performCannyProcessing(mat);
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
            //TODO replace magic number 2*10
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
            return cv::Scalar(255, 255, 255);
        case ORANGE:
            return cv::Scalar(255, 127, 0);
        case YELLOW:
            return cv::Scalar(255, 255, 0);
        case GREEN:
            return cv::Scalar(20, 240, 20);
        case BLUE:
            return cv::Scalar(0, 0, 200);
        case RED:
            return cv::Scalar(255, 0, 0);
        default:
            return cv::Scalar(-1, -1, -1);
    }
}

void CubeDetectorBehavior::performCannyProcessing(cv::Mat &currentFrame) {
    double startTime = getCurrentTimeMillis();
    std::vector<std::vector<cv::Point>> contours = detectContours(currentFrame);

    std::vector<cv::RotatedRect> filteredRectangles;
    std::vector<Circle> filteredRectanglesInnerCircles;

    filterContours(currentFrame, contours, filteredRectangles, filteredRectanglesInnerCircles);

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
        //TODO should be margin * 2?
        int diameterWithMargin = referenceCircle.radius * 2 + margin;
        std::vector<Circle> estimatedFacelets = estimateRemainingFaceletsPos(referenceCircle,
                                                                             diameterWithMargin);

        std::vector<std::vector<Circle>> facetModel = matchEstimatedWithPotentialFacelets(
                potentialFacelets, estimatedFacelets);
        facetModel[0][0] = referenceCircle;
        bool cubeFound = verifyIfCubeFound(facetModel);

        if (cubeFound) {
            fillMissingFacelets(estimatedFacelets, facetModel);
            std::vector<std::vector<int>> colors = detectFacetColors(currentFrame, facetModel);
            drawFoundFacelets(currentFrame, facetModel, colors);
            if (debuggable) {
                saveDebugData(currentFrame,
                              filteredRectangles,
                              referenceCircle,
                              potentialFacelets,
                              estimatedFacelets,
                              colors);
            }
            LOG_DEBUG("CATALNER", "notifying listener!");
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

void CubeDetectorBehavior::drawFoundFacelets(cv::Mat &currentFrame,
                                             const std::vector<std::vector<Circle>> &facetModel,
                                             const std::vector<std::vector<int>> &colors) {
    for (int j = 0; j < 3; j++) {
        for (int k = 0; k < 3; k++) {
            Circle detectedCircle = facetModel[j][k];
            //draw the found circles on screen
            circle(currentFrame, detectedCircle.center, detectedCircle.radius,
                   getColorAsScalar(colors[j][k]), 1, CV_AA, 0);
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
    std::vector<std::vector<cv::Point> > contoursPoly(contours.size());
    int totalImageArea = currentFrame.cols * currentFrame.rows;
    /// Approximate contours to polygons + get bounding rects and circles
    for (int i = 0; i < contours.size(); i++) {
        //approximate contour to a polygon, save it.
        cv::approxPolyDP(cv::Mat(contours[i]), contoursPoly[i], 3, true);
        //computer the minimum containing rectangle of the current contour
        cv::RotatedRect currentRect = cv::minAreaRect(cv::Mat(contoursPoly[i]));
        //calculate aspect aspectRatio
        float maxDimensionValue = (float) std::max(currentRect.size.height, currentRect.size.width);
        float minDimensionValue = std::min(currentRect.size.height, currentRect.size.width);
        float aspectRatio = maxDimensionValue / minDimensionValue;
        //verify contour aspect ratio, size and area.
        if (aspectRatio < 1.2f &&
            currentRect.size.area() > 0.0025f * totalImageArea &&
            currentRect.size.height < currentFrame.rows * 0.25f &&
            currentRect.size.width < currentFrame.cols * 0.25f) {
            //if based on the above conditions it is considered as being a valid contour,
            //then save it for later processing
            possibleFacelets.push_back(currentRect);
            possibleFaceletsInnerCircles.push_back(Circle(currentRect));
        }
    }
}

std::vector<std::vector<cv::Point>>
CubeDetectorBehavior::detectContours(const cv::Mat &currentFrame) const {
    std::vector<std::vector<cv::Point> > contours;
    cv::Mat processingFrame;
    int lowThreshold = 10;
    int thresholdRatio = 2;
    int kernel_size = 3;

    // Convert the image to grayscale
    cvtColor(currentFrame, processingFrame, CV_RGB2GRAY);
    /// Reduce noise with a kernel 5x5
    GaussianBlur(processingFrame, processingFrame, cv::Size(5, 5), 0, 0);
    // Canny detector
    Canny(processingFrame, processingFrame, lowThreshold, lowThreshold * thresholdRatio,
          kernel_size);
    //BOUNDARY
    std::vector<cv::Vec4i> hierarchy;
    /// Find contours
    findContours(processingFrame, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE,
                 cv::Point(0, 0));
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
        cv::cvtColor(currentFrame, currentFrame, CV_RGB2BGR);
        imageSaver->saveImage(currentFrame, frameNumber, "full_frame");
        cv::cvtColor(currentFrame, currentFrame, CV_BGR2RGB);

        ///save filtered rectangles
        cv::Mat drawing = (cv::Mat) cv::Mat::zeros(currentFrame.size(), CV_8UC3);
        for (int i = 0; i < filteredRectangles.size(); i++) {
            // rotated rectangle
            cv::Point2f rect_points[4];
            filteredRectangles[i].points(rect_points);
            for (int j = 0; j < 4; j++)
                cv::line(drawing, rect_points[j], rect_points[(j + 1) % 4],
                         cv::Scalar(0, 255, 0), 1, CV_AA);
        }
        imageSaver->saveImage(drawing, frameNumber, "filtered_rectangles");

        ///save potential facelets
        drawing = cv::Mat::zeros(currentFrame.size(), CV_8UC3);
        utils::drawCircles(drawing, potentialFacelets, cv::Scalar(255, 0, 0));
        utils::drawCircle(drawing, referenceCircle, cv::Scalar(0, 0, 255));
        imageSaver->saveImage(drawing, frameNumber, "potential_facelets");

        ///save estimated facelets
        drawing = cv::Mat::zeros(currentFrame.size(), CV_8UC3);
        utils::drawCircles(drawing, estimatedFacelets, cv::Scalar(0, 255, 80));
        utils::drawCircle(drawing, referenceCircle, cv::Scalar(0, 0, 255));
        imageSaver->saveImage(drawing, frameNumber, "estimated_facelets");

        ///save potential & estimated facelets which match
        ///recompute the incomplete facelet model, in order to print only the facelets which passed through all the filtering steps
        ///it's a waste indeed to compute them once more...but then again, this is for debugging so..
        std::vector<std::vector<Circle>> faceletIncompleteModel = matchEstimatedWithPotentialFacelets(
                potentialFacelets, estimatedFacelets);

        drawing = cv::Mat::zeros(currentFrame.size(), CV_8UC3);
        utils::drawCircles(drawing, estimatedFacelets, cv::Scalar(0, 255, 80));
        utils::drawCircles(drawing, faceletIncompleteModel, cv::Scalar(255, 0, 0), 3, true);
        utils::drawCircle(drawing, referenceCircle, cv::Scalar(0, 0, 255));
        utils::drawCircle(drawing, referenceCircle, cv::Scalar(255, 0, 0), 3, true);
        imageSaver->saveImage(drawing, frameNumber, "matched_pot_est_facelets");

        ///save just the facelets which matched with the estimated ones
        drawing = cv::Mat::zeros(currentFrame.size(), CV_8UC3);
        utils::drawCircles(drawing, faceletIncompleteModel, cv::Scalar(0, 255, 0), 3);
        utils::drawCircle(drawing, referenceCircle, cv::Scalar(0, 0, 255));
        imageSaver->saveImage(drawing, frameNumber, "matched_potential_facelets");
    }
    LOG_DEBUG("RUBIK_JNI_PART.cpp",
              "COLORS: [1]:{ %d %d %d } [2]:{ %d %d %d } [3]:{ %d %d %d }",
              colors[0][0], colors[0][1], colors[0][2], colors[1][0],
              colors[1][1], colors[1][2], colors[2][0], colors[2][1],
              colors[2][2]);
    //end debug data saving
}

std::vector<uchar>
CubeDetectorBehavior::findCube(const std::vector<uint8_t> &imageData, int width, int height) {
    cv::Mat &currentFrame = *new cv::Mat((int) (height * 1.5f), width, CV_8UC1,
                                         (uchar *) imageData.data());
    cv::cvtColor(currentFrame, currentFrame, cv::COLOR_YUV2RGB_NV21);
    findCube(currentFrame);
    cv::cvtColor(currentFrame, currentFrame, cv::COLOR_RGB2RGBA);
    std::vector<uchar> array;
    if (currentFrame.isContinuous()) {
        array.assign(currentFrame.datastart, currentFrame.dataend);
    } else {
        for (int i = 0; i < currentFrame.rows; ++i) {
            array.insert(array.end(), currentFrame.ptr<uchar>(i),
                         currentFrame.ptr<uchar>(i) + currentFrame.cols);
        }
    }

    return array;
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
                cv::Mat stickerRoiHSV = currentFrame(roi).clone();
                // Convert the image to HSV
                cv::cvtColor(stickerRoiHSV, stickerRoiHSV, CV_RGB2HSV);
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

void CubeDetectorBehavior::setDebuggable(const bool isDebuggable) {
    if (debuggable) {
        LOG_DEBUG("RUBIK_JNI_PART.cpp", "setDebuggable. current:%d, new: %d, frameNumber: %d",
                  debuggable, isDebuggable, frameNumber);
    }
    frameNumber++;
    debuggable = isDebuggable;
    colorDetector->setDebuggable(isDebuggable);
}

bool CubeDetectorBehavior::isDebuggable() {
    return debuggable;
}

/* return current time in milliseconds */
static double getCurrentTimeMillis(void) {
    struct timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    return 1000.0 * res.tv_sec + (double) res.tv_nsec / 1e6;
}
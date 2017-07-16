//
// Created by catalin on 12.07.2017.
//

#include "CubeDetectorBehavior.hpp"
#include "OnCubeDetectionResultListener.hpp"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/types_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include <android/log.h>

/* return current time in milliseconds */
static double getCurrentTimeMillis(void);

using namespace std;
using namespace cv;

CubeDetectorBehavior::CubeDetectorBehavior() {
    //empty default constructor
}

CubeDetectorBehavior::~CubeDetectorBehavior() {
    delete &onCubeDetectionResultListener;
}

void
CubeDetectorBehavior::setOnCubeDetectionResultListener(OnCubeDetectionResultListener &listener) {
    onCubeDetectionResultListener = &listener;
}

void CubeDetectorBehavior::findCube(cv::Mat &mat) {
    performCannyProcessing(mat);
}

float CubeDetectorBehavior::pointsDistance(Point firstPoint, Point secondPoint) {
    return sqrt((float) ((firstPoint.x - secondPoint.x) * (firstPoint.x - secondPoint.x)
                         + (firstPoint.y - secondPoint.y) * (firstPoint.y - secondPoint.y)));
}

void CubeDetectorBehavior::saveImage(cv::Mat mat, int frameNumber, int lineNumber) {

    stringstream stringStream, stringStream2;
    stringStream << frameNumber;
    stringStream2 << lineNumber;

    string store_path = "/storage/emulated/0/RubikResults/pic_"
                        + stringStream.str() + "_" + stringStream2.str() + ".jpg";

    bool write_success = cv::imwrite(store_path, mat);
}

/*
 Print the histogram and also specify the row & column of the printed sticker
 */
void CubeDetectorBehavior::printOwnHistogram(int row, int column, int histSize, int hist[]) {
    int hist_w = histSize * 4;
    int hist_h = 2300;
    Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(255, 255, 255));

    /// Draw for each channel
    for (int i = 0; i < histSize; i++) {
        int height = hist[i];
        if (height > 0) {
            line(histImage, Point(i * 4 + 2, hist_h),
                 Point(i * 4 + 2, hist_h - height), Scalar(255, 0, 0), 4, 0,
                 0);
        }
    }
    saveImage(histImage, frameNumber, row * 100 + column * 10 + 2);
}

int CubeDetectorBehavior::getColorInSticker(Mat stickerRoi, int row, int column, int mode) {
    //params common for both hue & saturation channels
    int saturationHistogramSize = 256;
    int saturationHistogram[256];
    int hueHistogramSize = 180;
    int hueHistogram[180];
    int totalNrOfColorPixels;
    vector<Mat> hsvChannels;
    uchar hueVal, saturationVal, valueVal;
    int stickerCols, stickerRows, saturationSum;

    stickerCols = stickerRoi.cols;
    stickerRows = stickerRoi.rows;
    split(stickerRoi, hsvChannels);

    for (int i = 0; i < saturationHistogramSize; i++) {
        saturationHistogram[i] = 0;
    }
    totalNrOfColorPixels = 0;
    for (int i = 0; i < stickerRows; i++) {
        for (int j = 0; j < stickerCols; j++) {
            float distance = pointsDistance(Point(j, i),
                                            Point(stickerRoi.rows / 2,
                                                  stickerRoi.rows / 2));
            if (round(distance) < stickerRoi.rows / 2 || mode) {
                //if point is in circle
                saturationVal = hsvChannels[SATURATION].at<uchar>(i, j);
                valueVal = hsvChannels[VALUE].at<uchar>(i, j);
                if (saturationVal <= saturationThreshold && valueVal > 160) {
                    saturationHistogram[saturationVal]++;
                }
                if (valueVal > 80) {
                    totalNrOfColorPixels++;
                }
            }

        }
    }

    saturationSum = 0;
    for (int i = 0; i <= saturationThreshold; i++) {
        saturationSum += saturationHistogram[i];
    }

    float whitePixelRatio = (float) saturationSum / totalNrOfColorPixels;
    float whiteMinRatio;
    if (row == 1 & column == 1) {
        whiteMinRatio = 0.44f;
    } else {
        whiteMinRatio = 0.5f;
    }
    if (whitePixelRatio > whiteMinRatio) {
        //if the majority of the saturation values are in the "almost white" range of the saturation domain, then assume color is white
        //print saturation histogram
        if (saveDebugData) {
            printOwnHistogram((row + 1), (column + 1), 256,
                              saturationHistogram);
            cvtColor(stickerRoi, stickerRoi, COLOR_HSV2BGR);
            saveImage(stickerRoi, frameNumber, (row + 1) * 10 + (column + 1));
        }
        return WHITE;
    } else {
        for (int i = 0; i < hueHistogramSize; i++) {
            hueHistogram[i] = 0;
        }
        for (int i = 0; i < stickerRows; i++) {
            for (int j = 0; j < stickerCols; j++) {
                float distance = pointsDistance(Point(j, i),
                                                Point(stickerRoi.rows / 2,
                                                      stickerRoi.rows / 2));
                if (distance < stickerRoi.rows / 2 || mode) {
                    //if point is in circle
                    hueVal = hsvChannels[HUE].at<uchar>(i, j);
                    valueVal = hsvChannels[VALUE].at<uchar>(i, j);
                    if (valueVal > 80) {
                        hueHistogram[hueVal]++;
                    }
                }
            }
        }

        HueColorEvidence colorEvidence[5] = {HueColorEvidence(RED),
                                             HueColorEvidence(ORANGE), HueColorEvidence(YELLOW),
                                             HueColorEvidence(GREEN), HueColorEvidence(BLUE)};

        for (int i = 0; i < hueHistogramSize; i++) {
            if (i >= 4 && i <= 18) {
                colorEvidence[ORANGE].evidence += hueHistogram[i];
            } else if (i > 18 && i <= 39) {
                colorEvidence[YELLOW].evidence += hueHistogram[i];
            } else if (i > 39 && i <= 76) {
                colorEvidence[GREEN].evidence += hueHistogram[i];
            } else if (i > 76 && i <= 136) {
                colorEvidence[BLUE].evidence += hueHistogram[i];
            } else if ((i >= 171 && i <= 179) || (i >= 0 && i < 4)) {
                colorEvidence[RED].evidence += hueHistogram[i];
            }
        }

        std::sort(colorEvidence, colorEvidence + 5,
                  [](const HueColorEvidence &firstItem, const HueColorEvidence &secondItem) {
                      return firstItem.evidence > secondItem.evidence;
                  });
        if (saveDebugData) {
            printOwnHistogram(row + 1, column + 1, 180, hueHistogram);
            cvtColor(stickerRoi, stickerRoi, COLOR_HSV2BGR);
            saveImage(stickerRoi, frameNumber, (row + 1) * 10 + (column + 1));
        }
        return colorEvidence[0].color;
    }
}

int CubeDetectorBehavior::getSmallestMargin(Circle circle, vector<Circle> validCircles) {
    int margin = 32768;
    for (int i = 0; i < validCircles.size(); i++) {
        Circle testedCircle = validCircles[i];
        if (circle.contains(testedCircle.center)
            || (testedCircle.center.x
                > circle.center.x + 3 * circle.radius + 2 * 10)
            || (testedCircle.center.y
                > circle.center.y + 3 * circle.radius + 2 * 10)) {
            //if the current rectangle is just a double of the current one, or is either to far from the current rectangle (either to the left or to the right, then just skip it
            continue;
        }
        int currentMargin =
                (int) pointsDistance(circle.center, testedCircle.center) - circle.radius -
                testedCircle.radius;
        if (currentMargin < margin && currentMargin > 0) {
            margin = currentMargin;
        }
    }

    return (margin == 32768 || margin > circle.radius) ? 10 : margin;
}

int CubeDetectorBehavior::getPositionInVector(int i, int j) {

    if (i == 0) {

        if (j == 1) {
            return 0;
        } else if (j == 2) {
            return 1;
        }
    } else if (i == 1) {

        if (j == 0) {
            return 2;
        } else if (j == 1) {
            return 3;
        } else if (j == 2) {
            return 4;
        }
    } else if (i == 2) {

        if (j == 0) {
            return 5;
        } else if (j == 1) {
            return 6;
        } else if (j == 2) {
            return 7;
        }
    }
    return -1;
}

Scalar CubeDetectorBehavior::getColorAsScalar(int color) {
    switch (color) {
        case WHITE:
            return Scalar(255, 255, 255);
        case ORANGE:
            return Scalar(255, 127, 0);
        case YELLOW:
            return Scalar(255, 255, 0);
        case GREEN:
            return Scalar(20, 240, 20);
        case BLUE:
            return Scalar(0, 0, 200);
        case RED:
            return Scalar(255, 0, 0);
        default:
            return Scalar(-1, -1, -1);
            break;
    }
}

void CubeDetectorBehavior::performCannyProcessing(Mat &currentFrame) {
    double startTime = getCurrentTimeMillis();
    Mat processingFrame;
    int lowThreshold = 10;
    int thresholdRatio = 2;
    int kernel_size = 3;

    // Convert the image to grayscale
    cvtColor(currentFrame, processingFrame, CV_RGB2GRAY);
    /// Reduce noise with a kernel 5x5
    GaussianBlur(processingFrame, processingFrame, Size(5, 5), 0, 0);
    // Canny detector
    Canny(processingFrame, processingFrame, lowThreshold, lowThreshold * thresholdRatio,
          kernel_size);

    //BOUNDARY
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    /// Find contours
    findContours(processingFrame, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE,
                 Point(0, 0));
    vector<vector<Point> > contours_poly(contours.size());
    vector<RotatedRect> minimumContainingRectangle;
    vector<Circle> innerCircles;
    int totalImageArea = currentFrame.cols * currentFrame.rows;
    /// Approximate contours to polygons + get bounding rects and circles
    for (int i = 0; i < contours.size(); i++) {
        //approximate contour to a polygon, save it.
        approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
        //computer the minimum containing rectangle of the current contour
        RotatedRect currentRect = minAreaRect(Mat(contours_poly[i]));
        //calculate aspect aspectRatio
        float maxDimensionValue = (float) max(currentRect.size.height, currentRect.size.width);
        float minDimensionValue = min(currentRect.size.height, currentRect.size.width);
        float aspectRatio = maxDimensionValue / minDimensionValue;
        //verify contour aspect ratio, size and area.
        if (aspectRatio < 1.2f &&
            currentRect.size.area() > 0.0025f * totalImageArea &&
            currentRect.size.height < currentFrame.rows * 0.25f &&
            currentRect.size.width < currentFrame.cols * 0.25f) {
            //if based on the above conditions it is considered as being a valid contour,
            //then save it for later processing
            minimumContainingRectangle.push_back(currentRect);
            innerCircles.push_back(Circle(currentRect));
        }
    }

    Circle referenceCircle, testedCircle;
    for (int i = 0; i < innerCircles.size(); i++) {
        referenceCircle = innerCircles[i];
        vector<Circle> validCircles;
        //only have rectangles that have an area similar to the initial one
        for (int j = 0; j < innerCircles.size(); j++) {
            if (i != j) {
                testedCircle = innerCircles[j];
            } else {
                continue;
            }
            float maxArea = (float) max(referenceCircle.area, testedCircle.area);
            int minArea = min(referenceCircle.area, testedCircle.area);
            float ratio = maxArea / minArea;
            float angleDifference = (float) abs(
                    (referenceCircle.angle - testedCircle.angle) * 180 / CV_PI);
            if (ratio < 1.35f && angleDifference < 15.0f) {
                validCircles.push_back(testedCircle);
            }
        }
        //test each and every rectangle
        if (validCircles.size() < 3) {
            //if we have at most 2 other rectangles that have an area similar to the inital one, just continue, can't be a cube facet
            continue;
        }

        int margin = getSmallestMargin(referenceCircle, validCircles);
        int diameterWithMargin = referenceCircle.radius * 2 + margin;

        //draw the remaining rects
        vector<Circle> newCircles;

        float angle = referenceCircle.angle;
        float xOffset = diameterWithMargin * cos(angle);
        float yOffset = diameterWithMargin * sin(angle);
        float xOffset2 = diameterWithMargin * (float) cos(angle + CV_PI / 2);
        float yOffset2 = diameterWithMargin * (float) sin(angle + CV_PI / 2);
        Circle secondRowFirstCircle = Circle(
                Point2f(referenceCircle.center.x + xOffset2,
                        referenceCircle.center.y + yOffset2),
                referenceCircle.radius, referenceCircle.angle);
        Circle thirdRowFirstCircle = Circle(
                Point2f(referenceCircle.center.x + 2 * xOffset2,
                        referenceCircle.center.y + 2 * yOffset2),
                referenceCircle.radius, referenceCircle.angle);

        newCircles.push_back(Circle(Point2f(referenceCircle.center.x + xOffset,
                                            referenceCircle.center.y + yOffset),
                                    referenceCircle.radius, referenceCircle.angle)
        );
        newCircles.push_back(Circle(Point2f(referenceCircle.center.x + 2 * xOffset,
                                            referenceCircle.center.y + 2 * yOffset),
                                    referenceCircle.radius, referenceCircle.angle)
        );

        newCircles.push_back(secondRowFirstCircle);
        newCircles.push_back(Circle(Point2f(secondRowFirstCircle.center.x + xOffset,
                                            secondRowFirstCircle.center.y + yOffset),
                                    referenceCircle.radius, referenceCircle.angle)
        );
        newCircles.push_back(Circle(Point2f(secondRowFirstCircle.center.x + 2 * xOffset,
                                            secondRowFirstCircle.center.y + 2 * yOffset),
                                    referenceCircle.radius, referenceCircle.angle)
        );

        newCircles.push_back(thirdRowFirstCircle);
        newCircles.push_back(Circle(Point2f(thirdRowFirstCircle.center.x + xOffset,
                                            thirdRowFirstCircle.center.y + yOffset),
                                    referenceCircle.radius, referenceCircle.angle)
        );
        newCircles.push_back(Circle(Point2f(thirdRowFirstCircle.center.x + 2 * xOffset,
                                            thirdRowFirstCircle.center.y + 2 * yOffset),
                                    referenceCircle.radius, referenceCircle.angle)
        );

        //segment
        Circle confirmedDetectedCircles[3][3];
        int confirmedCirclesFlags[3][3];
        vector<Circle> commonAreaCirclesFound;
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                confirmedCirclesFlags[j][k] = 0;
            }
        }

        for (int j = 0; j < 8; j++) {
            Circle circleUnderTest = newCircles[j];
            for (int k = 0; k < validCircles.size(); k++) {
                Circle actualCircle = validCircles[k];
                if (actualCircle.contains(circleUnderTest.center)) {

                    float R = max(circleUnderTest.radius, actualCircle.radius);
                    float r = min(circleUnderTest.radius, actualCircle.radius);
                    float d = pointsDistance(circleUnderTest.center, actualCircle.center);

                    float part1 = r * r * acos((d * d + r * r - R * R) / (2 * d * r));
                    float part2 = R * R * acos((d * d + R * R - r * r) / (2 * d * R));
                    float part3 =
                            0.5f * sqrt((-d + r + R) * (d + r - R) * (d - r + R) * (d + r + R));
                    float intersectionArea = part1 + part2 - part3;

                    float areasRatio = actualCircle.area / intersectionArea;
                    if (areasRatio < 1.45f) {
                        switch (j) {
                            case 0:
                                confirmedDetectedCircles[0][1] = actualCircle;
                                confirmedCirclesFlags[0][1] = 1;
                                break;
                            case 1:
                                confirmedDetectedCircles[0][2] = actualCircle;
                                confirmedCirclesFlags[0][2] = 1;
                                break;
                            case 2:
                                confirmedDetectedCircles[1][0] = actualCircle;
                                confirmedCirclesFlags[1][0] = 1;
                                break;
                            case 3:
                                confirmedDetectedCircles[1][1] = actualCircle;
                                confirmedCirclesFlags[1][1] = 1;
                                break;
                            case 4:
                                confirmedDetectedCircles[1][2] = actualCircle;
                                confirmedCirclesFlags[1][2] = 1;
                                break;
                            case 5:
                                confirmedDetectedCircles[2][0] = actualCircle;
                                confirmedCirclesFlags[2][0] = 1;
                                break;
                            case 6:
                                confirmedDetectedCircles[2][1] = actualCircle;
                                confirmedCirclesFlags[2][1] = 1;
                                break;
                            case 7:
                                confirmedDetectedCircles[2][2] = actualCircle;
                                confirmedCirclesFlags[2][2] = 1;
                                break;
                            default:
                                break;
                        }
                        commonAreaCirclesFound.push_back(circleUnderTest);
                        //already found a matching rectangle, we are not interested in others. just continue
                        continue;
                    }
                }
            }
        }

        //verify if valid cube model
        //top left element is always considered to be 1. if any of these is 0, then it means no sticker was detected on the specific row
        int topRow = 100 + 10 * confirmedCirclesFlags[0][1] + confirmedCirclesFlags[0][2];
        int middleRow = 100 * confirmedCirclesFlags[1][0] + 10 * confirmedCirclesFlags[1][1] +
                        confirmedCirclesFlags[1][2];
        int bottomRow = 100 * confirmedCirclesFlags[2][0] + 10 * confirmedCirclesFlags[2][1] +
                        confirmedCirclesFlags[2][2];

        bool cubeFound = false;
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

        if (cubeFound) {
            confirmedCirclesFlags[0][0] = 1;
            confirmedDetectedCircles[0][0] = referenceCircle;
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    if (confirmedCirclesFlags[j][k] == 0) {
                        confirmedDetectedCircles[j][k] = newCircles[getPositionInVector(j, k)];
                    }
                }
            }
            int colors[3][3];
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    Circle detectedCircle = confirmedDetectedCircles[j][k];
                    if (0 <= (detectedCircle.center.x - detectedCircle.radius) &&
                        0 <= (2 * detectedCircle.radius) &&
                        (detectedCircle.center.x + detectedCircle.radius) <= currentFrame.cols &&
                        0 <= (detectedCircle.center.y - detectedCircle.radius) &&
                        (detectedCircle.center.y + detectedCircle.radius) <= currentFrame.rows) {
                        Rect roi = Rect(
                                Point(detectedCircle.center.x - detectedCircle.radius,
                                      detectedCircle.center.y - detectedCircle.radius),
                                Point(detectedCircle.center.x + detectedCircle.radius,
                                      detectedCircle.center.y + detectedCircle.radius)
                        );
                        Mat stickerRoiHSV = currentFrame(roi).clone();
                        // Convert the image to HSV
                        cvtColor(stickerRoiHSV, stickerRoiHSV, CV_RGB2HSV);
                        colors[j][k] = getColorInSticker(stickerRoiHSV, j, k, 1);
                    } else {
                        colors[j][k] = WHITE;
                        __android_log_print(ANDROID_LOG_DEBUG,
                                            "RUBIK_JNI_PART.cpp",
                                            "\nframeNumberOld: %d FOUND INVALID RECT WHEN DETECTING COLORS",
                                            frameNumber);
                    }
                }
            }

            if (saveDebugData) {
                saveDebugDataM(currentFrame, processingFrame, minimumContainingRectangle,
                               referenceCircle, validCircles, newCircles, commonAreaCirclesFound,
                               colors);
            }

            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    Circle detectedCircle = confirmedDetectedCircles[j][k];
                    //draw the found circles on screen
                    circle(currentFrame, detectedCircle.center, detectedCircle.radius,
                           getColorAsScalar(colors[j][k]), 4, 8, 0);
                }
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
    frameRateAverage = frameRateSum / frameNumber;

    __android_log_print(ANDROID_LOG_DEBUG, "RUBIK_JNI_PART.cpp",
                        "\frameNumber: %d, frameRate current frame: %.2f, frameRateAverage: %.2f, frameRate: %.2f",
                        frameNumber, fps, frameRateAverage, fps);
    //end
}

void CubeDetectorBehavior::saveDebugDataM(const Mat &currentFrame, const Mat &dst,
                                          const vector<RotatedRect> &minimumContainingRectangle,
                                          const Circle &referenceCircle,
                                          const vector<Circle> &validCircles,
                                          const vector<Circle> &newCircles,
                                          const vector<Circle> &commonAreaCirclesFound,
                                          const int (&colors)[3][3]) {///BEGIN PRINT
    ///1
    cvtColor(currentFrame, currentFrame, CV_RGB2BGR);
    saveImage(currentFrame, frameNumber, 1);
    cvtColor(currentFrame, currentFrame, CV_BGR2RGB);
    ///2
    saveImage(dst, frameNumber, 2);

    Mat drawing = (Mat) Mat::zeros(dst.size(), CV_8UC3);
    for (int i = 0; i < minimumContainingRectangle.size(); i++) {
        // rotated rectangle
        Point2f rect_points[4];
        minimumContainingRectangle[i].points(rect_points);
        for (int j = 0; j < 4; j++)
            line(drawing, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 255, 0), 1, 8);
    }
    ///3
    saveImage(drawing, frameNumber, 3);

    drawing = Mat::zeros(dst.size(), CV_8UC3);
    for (int j = 0; j < validCircles.size(); j++) {
        circle(drawing, validCircles[j].center,
               validCircles[j].radius, Scalar(255, 0, 0), 2, 8, 0);
    }
    circle(drawing, referenceCircle.center,
           referenceCircle.radius, Scalar(0, 0, 255), 2, 8, 0);
    ///4
    saveImage(drawing, frameNumber, 4);
    drawing = Mat::zeros(dst.size(), CV_8UC3);
    for (int j = 0; j < newCircles.size(); j++) {
        circle(drawing, newCircles[j].center,
               newCircles[j].radius, Scalar(0, 255, 80), 2, 8, 0);
    }
    circle(drawing, referenceCircle.center,
           referenceCircle.radius, Scalar(0, 0, 255), 2, 8, 0);
    ///5
    saveImage(drawing, frameNumber, 5);

    drawing = Mat::zeros(dst.size(), CV_8UC3);
    for (int j = 0; j < newCircles.size(); j++) {
        circle(drawing, newCircles[j].center,
               newCircles[j].radius, Scalar(0, 255, 80), 2, 8, 0);
    }
    for (int k = 0; k < commonAreaCirclesFound.size(); k++) {
        Circle rect = commonAreaCirclesFound[k];
        Scalar color = Scalar(255, 0, 0);
        circle(drawing, rect.center, rect.radius - 3, color, -1, 8, 0);
    }
    circle(drawing, referenceCircle.center,
           referenceCircle.radius, Scalar(0, 0, 255), 2, 8, 0);
    ///6
    saveImage(drawing, frameNumber, 6);

    drawing = Mat::zeros(dst.size(), CV_8UC3);
    for (int k = 0; k < commonAreaCirclesFound.size(); k++) {
        Circle rect = commonAreaCirclesFound[k];
        Scalar color = Scalar(0, 255, 0);
        circle(drawing, rect.center, rect.radius, color, 2, 8, 0);
    }
    circle(drawing, referenceCircle.center,
           referenceCircle.radius, Scalar(0, 0, 255), 2, 8, 0);
    ///7
    saveImage(drawing, frameNumber, 7);

    __android_log_print(ANDROID_LOG_DEBUG, "RUBIK_JNI_PART.cpp",
                        "\nCOLORS: [1]:{ %d %d %d } [2]:{ %d %d %d } [3]:{ %d %d %d }",
                        colors[0][0], colors[0][1], colors[0][2], colors[1][0],
                        colors[1][1], colors[1][2], colors[2][0], colors[2][1],
                        colors[2][2]);
    //end debug data saving
}

/* return current time in milliseconds */
static double getCurrentTimeMillis(void) {
    struct timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    return 1000.0 * res.tv_sec + (double) res.tv_nsec / 1e6;
}
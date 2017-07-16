//
// Created by catalin on 12.07.2017.
//

#ifndef RUBIKDETECTORDEMO_CUBEDETECTORBEHAVIOR_HPP
#define RUBIKDETECTORDEMO_CUBEDETECTORBEHAVIOR_HPP

#include <opencv2/core/core.hpp>
#include "Circle.hpp"
#include "HueColorEvidence.hpp"
#include <iostream>


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

#ifndef  RED
#define RED 0
#endif

#ifndef  ORANGE
#define ORANGE 1
#endif

#ifndef  YELLOW
#define YELLOW 2
#endif

#ifndef  GREEN
#define GREEN 3
#endif

#ifndef  BLUE
#define BLUE 4
#endif

#ifndef  WHITE
#define WHITE 5
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

    ~CubeDetectorBehavior();

    void setOnCubeDetectionResultListener(OnCubeDetectionResultListener &listener);

    void findCube(cv::Mat &mat);

private:
    OnCubeDetectionResultListener *onCubeDetectionResultListener;

    int scale = 1;
    int frameNumber = 0;
    int saturationThreshold = 100;
    int saveDebugData = 0;

    double frameRateSum = 0;
    double frameRateAverage = 0;

    double pointsDistance(cv::Point firstPoint, cv::Point secondPoint);

    void saveImage(cv::Mat mat, int frameNumber, int lineNumber);

    void printOwnHistogram(int row, int column, int histSize, int hist[]);

    int getColorInSticker(cv::Mat stickerRoi, int row, int column, int mode);

    int getSmallestMargin(Circle circle, std::vector<Circle> validCircles);

    int getPositionInVector(int i, int j);

    cv::Scalar getColorAsScalar(int color);

    void performCannyProcessing(cv::Mat &mat);

    void saveDebugDataM(const cv::Mat &currentFrame, const cv::Mat &dst,
                        const std::vector<cv::RotatedRect> &minimumContainingRectangle,
                        const Circle &referenceCircle, const std::vector<Circle> &validCircles,
                        const std::vector<Circle> &newCircles,
                        const std::vector<Circle> &commonAreaCirclesFound,
                        const int (&colors)[3][3]);
};

#endif //RUBIKDETECTORDEMO_CUBEDETECTORBEHAVIOR_HPP

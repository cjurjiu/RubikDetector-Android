//
// Created by catalin on 26.07.2017.
//

#include <cmath>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "Utils.hpp"
#include "../detectors/cubedetector/CubeDetector.hpp"

namespace utils {
    float pointsDistance(const cv::Point2f &firstPoint, const cv::Point2f &secondPoint) {
        return std::sqrt((firstPoint.x - secondPoint.x) * (firstPoint.x - secondPoint.x) +
                         (firstPoint.y - secondPoint.y) * (firstPoint.y - secondPoint.y));
    }

    bool quickSaveImage(const cv::Mat &mat, const std::string path, const int frameNumber,
                        const int regionId) {

        std::stringstream stringStream, stringStream2;
        stringStream << frameNumber;
        stringStream2 << regionId;

        std::string store_path = path + "/pic_"
                                 + stringStream.str() + "_" + stringStream2.str() + ".jpg";

        return cv::imwrite(store_path, mat);
    }

    char colorIntToChar(const int colorInt) {
        switch (colorInt) {
            case CubeColors::RED:
                return 'r';
            case CubeColors::ORANGE:
                return 'o';
            case CubeColors::YELLOW:
                return 'y';
            case CubeColors::GREEN:
                return 'g';
            case CubeColors::BLUE:
                return 'b';
            case CubeColors::WHITE:
                return 'w';
            default:
                return 'x';
        }
    }

    cv::Scalar getColorAsScalar(int color) {
        switch (color) {
            case CubeColors::WHITE:
                return cv::Scalar(255, 255, 255, 255);
            case CubeColors::ORANGE:
                return cv::Scalar(255, 127, 0, 255);
            case CubeColors::YELLOW:
                return cv::Scalar(255, 255, 0, 255);
            case CubeColors::GREEN:
                return cv::Scalar(20, 240, 20, 255);
            case CubeColors::BLUE:
                return cv::Scalar(0, 0, 200, 255);
            case CubeColors::RED:
                return cv::Scalar(255, 0, 0, 255);
            default:
                return cv::Scalar(-1, -1, -1, 255);
        }
    }

    void drawCircle(cv::Mat &drawingCanvas, const Circle circle,
                    const cv::Scalar color, const float scalingFactor,
                    const int radiusModifier, const bool fillArea) {
        cv::circle(drawingCanvas, circle.center,
                   (int) round((circle.radius - radiusModifier < 0) ?
                               circle.radius : circle.radius -
                                               (int) round(radiusModifier * scalingFactor)),
                   color,
                //-1 means the circle will be filled, otherwise draw it with a 2px stroke
                   fillArea ? -1 : (int) round(1 * scalingFactor),
                   CV_AA, 0);
    }

    void drawCircles(cv::Mat &drawingCanvas, const std::vector<Circle> circles,
                     const cv::Scalar color, const float scalingFactor) {
        for (int i = 0; i < circles.size(); i++) {
            drawCircle(drawingCanvas, circles[i], color, scalingFactor);
        }
    }

    void drawCircles(cv::Mat &drawingCanvas, const std::vector<std::vector<Circle>> circles,
                     const cv::Scalar color, const float scalingFactor,
                     const int radiusModifier, const bool fillArea) {
        for (int i = 0; i < circles.size(); i++) {
            for (int j = 0; j < circles.size(); j++) {
                drawCircle(drawingCanvas, circles[i][j], color,
                           scalingFactor, radiusModifier,
                           fillArea);
            }
        }
    }

    /* return current time in milliseconds */
    double getCurrentTimeMillis() {
        struct timespec res;
        clock_gettime(CLOCK_REALTIME, &res);
        return 1000.0 * res.tv_sec + (double) res.tv_nsec / 1e6;
    }
}
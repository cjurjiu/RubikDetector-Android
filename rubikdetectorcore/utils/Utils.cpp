//
// Created by catalin on 26.07.2017.
//

#include <cmath>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "Utils.hpp"
#include "../detectors/cubedetector/CubeDetector.hpp"
#include "CrossLog.hpp"

namespace utils {
    float pointsDistance(const cv::Point &firstPoint, const cv::Point &secondPoint) {
        return std::sqrt((float) ((firstPoint.x - secondPoint.x) * (firstPoint.x - secondPoint.x) +
                                  (firstPoint.y - secondPoint.y) * (firstPoint.y - secondPoint.y)));
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
            case RED:
                return 'r';
            case ORANGE:
                return 'o';
            case YELLOW:
                return 'y';
            case GREEN:
                return 'g';
            case BLUE:
                return 'b';
            case WHITE:
                return 'w';
            default:
                return 'x';
        }
    }

    void drawCircle(cv::Mat &drawingCanvas, const Circle circle,
                    const cv::Scalar color, const float scalingFactor,
                    const int radiusModifier, const bool fillArea) {
        cv::circle(drawingCanvas, circle.center,
                   (circle.radius - radiusModifier < 0) ?
                   circle.radius : circle.radius - (int) round(radiusModifier * scalingFactor),
                   color,
                //-1 means the circle will be filled, otherwise draw it with a 2px stroke
                   fillArea ? -1 : (int) round(2 * scalingFactor),
                   CV_AA, 0);
    }

    void
    drawCircles(cv::Mat &drawingCanvas, const std::vector<Circle> circles, const cv::Scalar color,
                const float scalingFactor) {
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
}
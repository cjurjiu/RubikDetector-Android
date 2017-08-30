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

    void encodeNV21(const cv::Mat &inputArgb, const cv::Mat &outputNv21, int width, int height) {
        int frameSize = width * height;

        uint8_t *argb = inputArgb.data;
        uint8_t *nv21 = outputNv21.data;

        int yIndex = 0;
        int uvIndex = frameSize;

        int a, R, G, B, Y, U, V;
        int index = 0;
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {

                a = (argb[index] & 0xff000000) >> 24; // a is not used obviously
                R = (argb[index] & 0xff0000) >> 16;
                G = (argb[index] & 0xff00) >> 8;
                B = (argb[index] & 0xff) >> 0;

                // well known RGB to YUV algorithm
                Y = ((66 * R + 129 * G + 25 * B + 128) >> 8) + 16;
                U = ((-38 * R - 74 * G + 112 * B + 128) >> 8) + 128;
                V = ((112 * R - 94 * G - 18 * B + 128) >> 8) + 128;

                // NV21 has a plane of Y and interleaved planes of VU each sampled by a factor of 2
                //    meaning for every 4 Y pixels there are 1 V and 1 U.  Note the sampling is every other
                //    pixel AND every other scanline.
                nv21[yIndex++] = (uint8_t) ((Y < 0) ? 0 : ((Y > 255) ? 255 : Y));
                if (j % 2 == 0 && index % 2 == 0) {
                    nv21[uvIndex++] = (uint8_t) ((V < 0) ? 0 : ((V > 255) ? 255 : V));
                    nv21[uvIndex++] = (uint8_t) ((U < 0) ? 0 : ((U > 255) ? 255 : U));
                }

                index++;
            }
        }
    }

    void encodeNV12(const cv::Mat &inputArgb, const cv::Mat &outputNv12, int width, int height) {
        int frameSize = width * height;

        uint8_t *argb = inputArgb.data;
        uint8_t *nv12 = outputNv12.data;

        int yIndex = 0;
        int uvIndex = frameSize;

        int a, R, G, B, Y, U, V;
        int index = 0;
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {

                a = (argb[index] & 0xff000000) >> 24; // a is not used obviously
                R = (argb[index] & 0xff0000) >> 16;
                G = (argb[index] & 0xff00) >> 8;
                B = (argb[index] & 0xff) >> 0;

                // well known RGB to YUV algorithm
                Y = ((66 * R + 129 * G + 25 * B + 128) >> 8) + 16;
                U = ((-38 * R - 74 * G + 112 * B + 128) >> 8) + 128;
                V = ((112 * R - 94 * G - 18 * B + 128) >> 8) + 128;

                // NV21 has a plane of Y and interleaved planes of VU each sampled by a factor of 2
                //    meaning for every 4 Y pixels there are 1 V and 1 U.  Note the sampling is every other
                //    pixel AND every other scanline.
                nv12[yIndex++] = (uint8_t) ((Y < 0) ? 0 : ((Y > 255) ? 255 : Y));
                if (j % 2 == 0 && index % 2 == 0) {
                    nv12[uvIndex++] = (uint8_t) ((U < 0) ? 0 : ((U > 255) ? 255 : U));
                    nv12[uvIndex++] = (uint8_t) ((V < 0) ? 0 : ((V > 255) ? 255 : V));
                }

                index++;
            }
        }
    }
}
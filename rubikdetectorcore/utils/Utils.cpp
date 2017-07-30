//
// Created by catalin on 26.07.2017.
//

#include <cmath>
#include <opencv2/highgui/highgui.hpp>
#include "Utils.hpp"
#include "../detectors/cubedetector/CubeDetector.hpp"

namespace utils {
    float pointsDistance(const cv::Point &firstPoint, const cv::Point &secondPoint) {
        return std::sqrt((float) ((firstPoint.x - secondPoint.x) * (firstPoint.x - secondPoint.x) +
                                  (firstPoint.y - secondPoint.y) * (firstPoint.y - secondPoint.y)));
    }

    bool saveImage(const cv::Mat &mat, const int frameNumber, const int regionId) {

        std::stringstream stringStream, stringStream2;
        stringStream << frameNumber;
        stringStream2 << regionId;

        std::string store_path = "./pic_"
                                 + stringStream.str() + "_" + stringStream2.str() + ".jpg";

        return cv::imwrite(store_path, mat);
    }

    char colorIntToChar(int colorInt) {
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
}
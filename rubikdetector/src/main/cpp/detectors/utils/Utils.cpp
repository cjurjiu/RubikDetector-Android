//
// Created by catalin on 26.07.2017.
//

#include <cmath>
#include <opencv2/highgui/highgui.hpp>
#include "Utils.hpp"

namespace utils {
    float pointsDistance(const cv::Point &firstPoint, const cv::Point &secondPoint) {
        return std::sqrt((float) ((firstPoint.x - secondPoint.x) * (firstPoint.x - secondPoint.x) +
                                  (firstPoint.y - secondPoint.y) * (firstPoint.y - secondPoint.y)));
    }

    bool saveImage(const cv::Mat &mat, const int frameNumber, const int regionId) {

        std::stringstream stringStream, stringStream2;
        stringStream << frameNumber;
        stringStream2 << regionId;

        std::string store_path = "/storage/emulated/0/RubikResults/pic_"
                                 + stringStream.str() + "_" + stringStream2.str() + ".jpg";

        return cv::imwrite(store_path, mat);
    }
}
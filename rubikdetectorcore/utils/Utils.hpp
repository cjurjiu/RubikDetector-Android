//
// Created by catalin on 26.07.2017.
//

#ifndef RUBIKDETECTORDEMO_UTILS_HPP
#define RUBIKDETECTORDEMO_UTILS_HPP

namespace utils {
    float pointsDistance(const cv::Point &firstPoint, const cv::Point &secondPoint);

    bool saveImage(const cv::Mat &mat, const int frameNumber, const int regionId);

    char colorIntToChar(int colorInt);
}
#endif //RUBIKDETECTORDEMO_UTILS_HPP

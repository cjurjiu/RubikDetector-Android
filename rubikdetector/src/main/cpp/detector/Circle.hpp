//
// Created by catalin on 13.07.2017.
//

#ifndef RUBIKDETECTORDEMO_CIRCLE_H
#define RUBIKDETECTORDEMO_CIRCLE_H

#include <opencv2/core/core.hpp>

class Circle {
private:
    float computeAngle(cv::Point2f *points);

public:
    cv::Point2i center;
    int radius;
    int area;
    float angle;

    Circle();

    Circle(cv::Point2f c, float r, float a);

    Circle(const cv::RotatedRect &rect);

    bool contains(cv::Point2f point);

    bool contains(cv::Point2i point);
};


#endif //RUBIKDETECTORDEMO_CIRCLE_H

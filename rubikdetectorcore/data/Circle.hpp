//
// Created by catalin on 13.07.2017.
//

#ifndef RUBIKDETECTORDEMO_CIRCLE_H
#define RUBIKDETECTORDEMO_CIRCLE_H

#include <opencv2/core/core.hpp>

class Circle {
public:
    cv::Point2f center;
    float radius;
    int area;
    float angle;
    float originalRectWidth;
    float originalRectHeight;

    Circle();

    Circle(const Circle &original);

    Circle(const Circle &original, const cv::Point2f &centerOffset);

    Circle(const cv::RotatedRect &rect);

    bool contains(const cv::Point2i &point) const;

    bool contains(const cv::Point2f &point) const;

    bool isEmpty() const;

private:
    float computeAngle(const cv::Point2f points[]);
};


#endif //RUBIKDETECTORDEMO_CIRCLE_H

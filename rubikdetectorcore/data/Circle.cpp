//
// Created by catalin on 13.07.2017.
//

#include "opencv2/core/core.hpp"
#include "Circle.hpp"

float Circle::computeAngle(cv::Point2f *points) {
    cv::Point_<float> smallestYIndex;
    cv::Point_<float> secondSmallestYIndex;
    int minY = 32768, secondSmallestY = 32768;
    for (int i = 0; i < 4; i++) {
        if (points[i].y < minY) {
            secondSmallestYIndex = smallestYIndex;
            secondSmallestY = (int) secondSmallestYIndex.y;
            smallestYIndex = points[i];
            minY = (int) smallestYIndex.y;
        } else if (points[i].y < secondSmallestY) {
            secondSmallestYIndex = points[i];
            secondSmallestY = (int) secondSmallestYIndex.y;
        }
    }
    float angle = std::atan2(secondSmallestYIndex.y - smallestYIndex.y,
                             secondSmallestYIndex.x - smallestYIndex.x);
    return angle;
}

Circle::Circle() {
    center = cv::Point2i(0, 0);
    radius = 0;
    area = 0;
    angle = 0;
}

Circle::Circle(cv::Point2f c, float r, float a) {
    center = cv::Point2i((int) roundf(c.x), (int) roundf(c.y));
    radius = (int) roundf(r);
    area = (int) (CV_PI * r * r);
    angle = a;
}

Circle::Circle(const cv::RotatedRect &rect) {
    center = cv::Point2i((int) roundf(rect.center.x), (int) round(rect.center.y));
    radius = (int) roundf(std::min(rect.size.width, rect.size.height) / 2);
    area = (int) roundf((float) CV_PI * radius * radius);
    cv::Point_<float> pts[4];
    rect.points(pts);
    angle = computeAngle(pts);
}

bool Circle::contains(cv::Point2f point) const {
    return std::abs(point.x - center.x) < radius
           && std::abs(point.y - center.y) < radius;
}

bool Circle::contains(cv::Point2i point) const {
    return std::abs(point.x - center.x) < radius
           && std::abs(point.y - center.y) < radius;
}

bool Circle::isEmpty() const {
    return (int) angle == 0 && area == 0 && radius == 0 && center.x == 0 && center.y == 0;
}

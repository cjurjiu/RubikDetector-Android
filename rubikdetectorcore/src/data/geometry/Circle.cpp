//
// Created by catalin on 13.07.2017.
//

#include "opencv2/core/core.hpp"
#include "../../../include/rubikdetector/data/geometry/internal/Circle.hpp"

namespace rbdt {

Circle::Circle() : center(cv::Point2i(0, 0)), radius(0),
                   area(0), angle(0), originalRectWidth(0),
                   originalRectHeight(0) {}

Circle::Circle(const Circle &original, const cv::Point2f &centerOffset) {
    center = original.center + centerOffset;
    radius = original.radius;
    area = original.area;
    angle = original.angle;
    originalRectWidth = original.originalRectWidth;
    originalRectHeight = original.originalRectHeight;
}

Circle::Circle(const cv::RotatedRect &rect) {
    center = rect.center;
    radius = std::min(rect.size.width, rect.size.height) / 2;
    area = (int) round(CV_PI * radius * radius);
    cv::Point2f pts[4];
    rect.points(pts);
    angle = computeAngle(pts);
    originalRectWidth = rect.size.width;
    originalRectHeight = rect.size.height;
}

bool Circle::contains(const cv::Point2i &point) const {
    return std::abs(point.x - center.x) < radius
           && std::abs(point.y - center.y) < radius;
}

bool Circle::contains(const cv::Point2f &point) const {
    return std::abs(point.x - center.x) < radius
           && std::abs(point.y - center.y) < radius;
}

bool Circle::isEmpty() const {
    return (int) angle == 0 && area == 0 && (int) radius == 0 && (int) center.x == 0 &&
           (int) center.y == 0;
}

float Circle::computeAngle(const cv::Point2f points[]) {
    cv::Point2f smallestYIndex;
    cv::Point2f secondSmallestYIndex;
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

} //namespace rbdt
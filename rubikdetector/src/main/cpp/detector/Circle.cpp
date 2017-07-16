//
// Created by catalin on 13.07.2017.
//

#include "opencv2/core/core.hpp"
#include "Circle.hpp"

using namespace cv;

float Circle::computeAngle(Point2f *points) {
    Point_<float> smallestYIndex;
    Point_<float> secondSmallestYIndex;
    int minY = 32768, secondSmallestY = 32768;
    for (int i = 0; i < 4; i++) {
        if (points[i].y < minY) {
            secondSmallestYIndex = smallestYIndex;
            secondSmallestY = secondSmallestYIndex.y;
            smallestYIndex = points[i];
            minY = smallestYIndex.y;
        } else if (points[i].y < secondSmallestY) {
            secondSmallestYIndex = points[i];
            secondSmallestY = secondSmallestYIndex.y;
        }
    }
    float angle = atan2(secondSmallestYIndex.y - smallestYIndex.y,
                        secondSmallestYIndex.x - smallestYIndex.x);
    return angle;
}

Circle::Circle() {
    center = *new Point2i(0, 0);
    radius = 0;
    area = 0;
    angle = 0;
}

Circle::Circle(Point2f c, float r, float a) {
    center = Point2i(roundf(c.x), roundf(c.y));
    radius = roundf(r);
    area = CV_PI * r * r;
    angle = a;
}

Circle::Circle(const cv::RotatedRect &rect) {
    center = Point2i(roundf(rect.center.x), round(rect.center.y));
    radius = roundf(min(rect.size.width, rect.size.height) / 2);
    area = roundf(CV_PI * radius * radius);
    Point_<float> pts[4];
    rect.points(pts);
    angle = computeAngle(pts);
}

bool Circle::contains(Point2f point) {
    if (abs(point.x - center.x) < radius
        && abs(point.y - center.y) < radius) {
        return true;
    }
    return false;
}

bool Circle::contains(Point2i point) {
    if (abs(point.x - (int) center.x) < (int) radius
        && abs(point.y - (int) center.y) < (int) radius) {
        return true;
    }
    return false;
}

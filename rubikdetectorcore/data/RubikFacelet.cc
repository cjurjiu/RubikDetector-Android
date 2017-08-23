//
// Created by catalin on 18.08.2017.
//

#include <cmath>
#include "RubikFacelet.hpp"

RubikFacelet::RubikFacelet() : RubikFacelet(0, Point2d(0, 0), 0, 0, 0) {}

RubikFacelet::RubikFacelet(int color, Point2d center, float width, float height, float angle)
        : color(color),
          center(center),
          width(width),
          height(height),
          angle(angle) {}

RubikFacelet::~RubikFacelet() {}

std::vector<Point2d> RubikFacelet::getPoints() {
    std::vector<Point2d> result(4);
    float angleSinHalf = std::sin(angle) * 0.5f;
    float angleCosHalf = std::cos(angle) * 0.5f;

    result[0] = Point2d(
            center.x - angleSinHalf * height - angleCosHalf * width,
            center.y + angleCosHalf * height - angleSinHalf * width);

    result[1] = Point2d(
            center.x + angleSinHalf * height - angleCosHalf * width,
            center.y - angleCosHalf * height - angleSinHalf * width);

    result[2] = Point2d(2 * center.x - result[0].x, 2 * center.y - result[0].y);

    result[3] = Point2d(2 * center.x - result[1].x, 2 * center.y - result[1].y);
    return result;
}

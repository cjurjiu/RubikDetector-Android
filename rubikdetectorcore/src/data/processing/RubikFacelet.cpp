//
// Created by catalin on 18.08.2017.
//

#include <cmath>
#include "../../../include/rubikdetector/data/processing/RubikFacelet.hpp"

namespace rbdt {

RubikFacelet::RubikFacelet() : RubikFacelet(Point2d(0, 0), 0, 0, 0, Color::WHITE) {}

RubikFacelet::RubikFacelet(Point2d center, float width, float height, float angle, Color color)
        : center(center),
          width(width),
          height(height),
          angle(angle),
          color(color) {}

RubikFacelet::~RubikFacelet() {}

std::vector<Point2d> RubikFacelet::corners() {
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

float RubikFacelet::innerCircleRadius() {
    return std::min(width, height) / 2;
}

} //end namespace rbdt
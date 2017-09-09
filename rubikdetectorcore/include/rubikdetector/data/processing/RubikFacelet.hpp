//
// Created by catalin on 18.08.2017.
//

#ifndef RUBIKDETECTOR_RUBIKFACELET_HPP
#define RUBIKDETECTOR_RUBIKFACELET_HPP

#include <vector>
#include "../geometry/Point2d.hpp"

namespace rbdt {

class RubikFacelet {
public:

    enum class Color {
        RED, ORANGE, YELLOW, GREEN, BLUE, WHITE
    };

    RubikFacelet();

    RubikFacelet(Point2d center, float width, float height, float angle,
                 Color color = Color::WHITE);

    ~RubikFacelet();

    std::vector<Point2d> corners();

    float innerCircleRadius();

    Color color;

    Point2d center;

    float width;

    float height;

    float angle;
};

} //end namespace rbdt
#endif //RUBIKDETECTOR_RUBIKFACELET_HPP

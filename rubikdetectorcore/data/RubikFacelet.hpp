//
// Created by catalin on 18.08.2017.
//

#ifndef RUBIKDETECTOR_RUBIKFACELET_HPP
#define RUBIKDETECTOR_RUBIKFACELET_HPP

#include <vector>
#include "Point2d.hpp"

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


#endif //RUBIKDETECTOR_RUBIKFACELET_HPP

//
// Created by catalin on 18.08.2017.
//

#ifndef RUBIKDETECTORANDROID_RUBIKFACELET_HPP
#define RUBIKDETECTORANDROID_RUBIKFACELET_HPP


#include <vector>
#include "Point2d.hpp"

class RubikFacelet {
public:
    RubikFacelet();

    RubikFacelet(int color, Point2d center, float width, float height, float angle);

    ~RubikFacelet();

    std::vector<Point2d> getPoints();

    int color;

    Point2d center;

    float width;

    float height;

    float angle;
};


#endif //RUBIKDETECTORANDROID_RUBIKFACELET_HPP

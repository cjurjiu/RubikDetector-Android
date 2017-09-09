//
// Created by catalin on 18.08.2017.
//

#ifndef RUBIKDETECTOR_POINT2D_HPP
#define RUBIKDETECTOR_POINT2D_HPP

namespace rbdt {

class Point2d {
public:
    Point2d();

    Point2d(const float x, const float y);

    ~Point2d();

    float x;

    float y;
};

} //end namespace rbdt
#endif //RUBIKDETECTOR_POINT2D_HPP

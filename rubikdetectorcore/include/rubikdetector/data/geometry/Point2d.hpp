//
// Created by catalin on 18.08.2017.
//

#ifndef RUBIKDETECTOR_POINT2D_HPP
#define RUBIKDETECTOR_POINT2D_HPP

namespace rbdt {

/**
 * Simple data class representing a 2D Point.
 */
class Point2d {
public:
    Point2d();

    Point2d(const float x, const float y);

    ~Point2d();

    /**
     * x of the point, offset in pixels from origin
     */
    float x;

    /**
     * y of the point, offset in pixels from origin
     */
    float y;
};

} //end namespace rbdt
#endif //RUBIKDETECTOR_POINT2D_HPP

//
// Created by catalin on 18.08.2017.
//

#include "../../../include/rubikdetector/data/geometry/Point2d.hpp"

namespace rbdt {

Point2d::Point2d() : Point2d(0, 0) {}

Point2d::Point2d(float x, float y) : x(x), y(y) {}

Point2d::~Point2d() {}

} //end namespace rbdt

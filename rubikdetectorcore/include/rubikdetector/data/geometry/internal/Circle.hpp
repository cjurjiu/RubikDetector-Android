//
// Created by catalin on 13.07.2017.
//

#ifndef RUBIKDETECTOR_CIRCLE_HPP
#define RUBIKDETECTOR_CIRCLE_HPP

#include <opencv2/core/core.hpp>

namespace rbdt {
/**
 * Internal representation of a Circle.
 *
 * Typically, this class is used to represent the inner circle of a cv::RotatedRect. Because of this, it also stores
 * the rectangle specific data of the rectangle it was created from.
 *
 * The created Circle uses the Rotated Rectangle's centre of gravity as its center point. The smallest
 * distance from the center to one of the rectangle's sides is used as the Circle's radius.
 *
 * Additionally, the rotation angle, width and height of the original rotated rectangle are also stored.
 *
 */
class Circle {
public:
    /**
     * Default constructor.
     *
     * @return a Circle instance which returns <i>true</i> if its Circle::isEmpty() method is called.
     */
    Circle();

    /**
     * Creates a Circle from an OpenCV Rotated Rectangle (cv::RotatedRect) object.
     *
     * The created Circle uses the Rotated Rectangle's centre of gravity as its center point. The smallest
     * distance from the center to one of the rectangle's sides is used as the Circle's radius.
     *
     * Additionally, the rotation angle, width and height of the original rotated rectangle are also stored.
     *
     * @param [in] rect
     * @return
     */
    Circle(const cv::RotatedRect &rect);

    /**
     * Copies a Circle, and applies an offset, through a cv::Point2f, on the center of the copy.
     * @param [in] original Circle to be copied
     * @param [in] centerOffset cv::Point which will be used to offset the center of the copy from the position of the original Circle center
     * @return a Circle copy of the original, having an offseted centre
     */
    Circle(const Circle &original, const cv::Point2f &centerOffset);

    /**
     * Checks if a cv::Point2i is within the centre's bounds.
     * @param [in] point the cv::Point2i to be tested
     * @return <i>true</i> if the point is inside this Circle, <i>false</i> otherwise
     */
    bool contains(const cv::Point2i &point) const;

    /**
     * Checks if a cv::Point2f is within the centre's bounds.
     * @param [in] point the cv::Point2f to be tested
     * @return <i>true</i> if the point is inside this Circle, <i>false</i> otherwise
     */
    bool contains(const cv::Point2f &point) const;

    /**
     * Checks whether this Circle has any relevant data.
     *
     * A Circle is considered as being "empty" if the data in its fields make conceptually equal to a point at (0,0) - i.e.:
     *   - centre at (0, 0);
     *   - radius of 0;
     *   - angle of 0;
     *   - area of 0;
     * @return <i>true</i> if the Circle contains no meaningful data, <i>false</i> otherwise
     */
    bool isEmpty() const;

    cv::Point2f center;

    float radius;

    int area;

    /**
     * Rotation angle in radians of the cv::RotatedRect from which this Circle was created.
     */
    float angle;

    /**
     * Width in pixels of the cv::RotatedRect from which this Circle was created.
     */
    float originalRectWidth;

    /**
     * Height in pixels of the cv::RotatedRect from which this Circle was created.
     */
    float originalRectHeight;

private:
    /**
     * Computes the rotation angle, in radians, of the rotated rect corners, present in the array parameter.
     * @param [in] points array of 4 points representing the 4 corners of a cv::RotatedRect
     * @return a floar representing the rotation angle, in radians.
     */
    float computeAngle(const cv::Point2f points[]);
};

} //namespace rbdt

#endif //RUBIKDETECTOR_CIRCLE_HPP

//
// Created by catalin on 18.08.2017.
//

#ifndef RUBIKDETECTOR_RUBIKFACELET_HPP
#define RUBIKDETECTOR_RUBIKFACELET_HPP

#include <vector>
#include "../geometry/Point2d.hpp"

namespace rbdt {
/**
 * Data class representing a detected Rubik's Cube facelet.
 *
 * This class holds the color & the location of a particular cube facelet within the input frame in which it was detected.
 *
 * The position of the facelet within the input frame is stored as a rotated rectangle, for which the centre of mass,
 * angle (radians), width & height are known.
 *
 * The four corners of the facelets can be obtained as an array of Point2d through RubikFacelet::corners(). Additionally the radius of its
 * inner circle can also be obtained through RubikFacelet::innerCircleRadius().
 *
 * The color of the facelet is specified through a RubikFacelet::Color. The default color of any facelet is RubikFacelet::Color::WHITE.
 */
class RubikFacelet {
public:

    /**
     * Enumeration containing the possible colors a RubikFacelet can take.
     */
    enum class Color {
        RED, ORANGE, YELLOW, GREEN, BLUE, WHITE
    };

    /**
     * Default constructor. The created RubikFacelet will have the following properties:
     *   - centre: Point2d(0, 0)
     *   - width: 0
     *   - height: 0
     *   - angle: 0
     *   - color: RubikFacelet::Color::WHITE
     * @return a RubikFacelet constructed as mentioned above.
     */
    RubikFacelet();

    /**
     * Constructor.
     *
     * @param center Point2d representing the location of centre of mass of the new RubikFacelet, in the input frame it was detected.
     * @param width Width of the RubikFacelet in pixels
     * @param height Height of the RubikFacelet in pixels
     * @param angle Angle of the RubikFacelet in radians
     * @param color A RubikFacelet::Color representing the color of the RubikFacelet
     * @return a RubikFacelet constructed as mentioned above.
     */
    RubikFacelet(Point2d center, float width, float height, float angle,
                 Color color = Color::WHITE);

    /**
     * Destructor.
     */
    ~RubikFacelet();

    /**
     * Computes the 4 corners of the facelet in the input frame, given the known properties of the facelet, and returns them in a vector.
     *
     * @return a std::vector containing 4 Point2d objects, which represent the 4 corners of the facelet.
     */
    std::vector<Point2d> corners();

    /**
     * Computes the radius of the circle centered in the facelet's center of mass, and touches at least two opposite sides of the facelet.
     *
     * @return a float that can be used as the radius of the facelet's inner circle.
     */
    float innerCircleRadius();

    /**
     * A RubikFacelet::Color representing the color of the facelet.
     */
    Color color;

    /**
     * Center of mass of the facelet.
     */
    Point2d center;

    /**
     * Width of the facelet, in pixels.
     */
    float width;

    /**
     * Height of the facelet, in pixels.
     */
    float height;

    /**
     * Rotation angle of the facelet, in radians.
     */
    float angle;
};

} //end namespace rbdt
#endif //RUBIKDETECTOR_RUBIKFACELET_HPP

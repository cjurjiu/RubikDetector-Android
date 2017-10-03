//
// Created by catalin on 07.09.2017.
//

#ifndef RUBIKDETECTOR_DRAWCONFIG_HPP
#define RUBIKDETECTOR_DRAWCONFIG_HPP

namespace rbdt {

/**
 * Immutable class which specifies how the facelets will be drawn by the RubikProcessor.
 *
 * Static convenience methods are available, as in:
 *   - DrawConfig::Default();
 *   - DrawConfig::DoNotDraw();
 *   - DrawConfig::Circles();
 *   - DrawConfig::FilledCircles();
 *   - DrawConfig::Rectangles().
 *
 * Drawing filled rectangles is currently unsupported.
 */
class DrawConfig {

public:
    /**
     * Specifies the currently available DrawModes.
     */
    enum class DrawMode {
        /**
         * Disables drawing, even if facelets are found.
         */
                DO_NOT_DRAW,
        /**
         * Draws facelets as rectangles.
         */
                DRAW_RECTANGLES,
        /**
         * Draws facelets as circles.
         */
                DRAW_CIRCLES
    };

    /**
     * Creates a new immutable DrawConfig instance with the specified properties.
     *
     * If <b>fillShape</b> is set to <i>true</i>, then the value of <b>strokeWidth</b> is ignored.
     *
     * @param drawMode a DrawConfig::DrawMode
     * @param strokeWidth the width of the rectangle/circle contour, in pixels.
     * @param fillShape <i>true</i> if the shape should be filled, <i>false</i> otherwise.
     */
    DrawConfig(DrawMode drawMode, int strokeWidth, bool fillShape);

    /**
     * Returns the DrawConfig::DrawMode set for this instance.
     *
     * @return current DrawConfig::DrawMode.
     */
    DrawMode getDrawMode() const;

    /**
     * Returns the width of the contour set for this instance.
     *
     * @return the contour width, in pixels.
     */
    int getStrokeWidth() const;

    /**
     * Can be used to check whether the shape will be filled or not, when drawn.
     *
     * @return <i>true</i> if the shape is filled, <i>false</i> otherwise.
     */
    bool isFillShape() const;

    /**
     * Creates an immutable DrawConfig with default values. This implies:
     *   - DrawConfig::DrawMode: DRAW_CIRCLES
     *   - strokeWidth: 2 pixels
     *   - fillShape: false
     * @return DrawConfig
     */
    static DrawConfig Default();

    /**
     * Creates an immutable DrawConfig that does not draw, even when facelets are found.
     *
     * @return DrawConfig
     */
    static DrawConfig DoNotDraw();

    /**
     * Creates an immutable DrawConfig that draws facelets as circles.
     *
     * The stroke width needs to be specified in the parameter.
     *
     * @param [in] strokeWidth the desired stroke width, in pixels.
     * @return DrawConfig
     */
    static DrawConfig Circles(int strokeWidth);

    /**
     * Creates an immutable DrawConfig that draws facelets as filled circles.
     * @return DrawConfig
     */
    static DrawConfig FilledCircles();

    /**
     * Creates an immutable DrawConfig that draws facelets as rectangles.
     *
     * The stroke width needs to be specified in the parameter.
     *
     * @param [in] strokeWidth the desired stroke width, in pixels.
     * @return DrawConfig
     */
    static DrawConfig Rectangles(int strokeWidth);

private:
    DrawConfig();

    DrawMode drawMode;

    /**
     * Stroke width, in pixels.
     */
    int strokeWidth;

    bool fillShape;
};

} //end namespace rbdt
#endif //RUBIKDETECTOR_DRAWCONFIG_HPP

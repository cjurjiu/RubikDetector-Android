//
// Created by catalin on 07.09.2017.
//

#ifndef RUBIKDETECTOR_DRAWCONFIG_HPP
#define RUBIKDETECTOR_DRAWCONFIG_HPP

namespace rbdt {

class DrawConfig {

public:
    enum class DrawMode {
        DO_NOT_DRAW, DRAW_RECTANGLES, DRAW_CIRCLES
    };

    DrawConfig(DrawMode drawMode, int strokeWidth, bool fillShape);

    DrawMode getDrawMode() const;

    int getStrokeWidth() const;

    bool isFillShape() const;

    static DrawConfig Default();

    static DrawConfig DoNotDraw();

    static DrawConfig Circles(int strokeWidth);

    static DrawConfig FilledCircles();

    static DrawConfig Rectangles(int strokeWidth);

private:
    DrawConfig();

    DrawMode drawMode;

    int strokeWidth;

    bool fillShape;
};

} //end namespace rbdt
#endif //RUBIKDETECTOR_DRAWCONFIG_HPP

//
// Created by catalin on 07.09.2017.
//

#include "../../../include/rubikdetector/data/config/DrawConfig.hpp"

namespace rbdt {

DrawConfig::DrawConfig() : drawMode(DrawMode::DO_NOT_DRAW), strokeWidth(0), fillShape(false) {}

DrawConfig::DrawConfig(DrawConfig::DrawMode drawMode, int strokeWidth, bool fillShape) :
        drawMode(drawMode), strokeWidth(strokeWidth), fillShape(fillShape) {}

DrawConfig::DrawMode DrawConfig::getDrawMode() const {
    return drawMode;
}

int DrawConfig::getStrokeWidth() const {
    return strokeWidth;
}

bool DrawConfig::isFillShape() const {
    return fillShape;
}

DrawConfig DrawConfig::Default() {
    return DrawConfig(DrawMode::DRAW_CIRCLES, 2, false);
}

DrawConfig DrawConfig::DoNotDraw() {
    return DrawConfig();
}

DrawConfig DrawConfig::Circles(int strokeWidth) {
    return DrawConfig(DrawMode::DRAW_CIRCLES, strokeWidth, false);
}

DrawConfig DrawConfig::FilledCircles() {
    return DrawConfig(DrawMode::DRAW_CIRCLES, 0, true);
}

DrawConfig DrawConfig::Rectangles(int strokeWidth) {
    return DrawConfig(DrawMode::DRAW_RECTANGLES, strokeWidth, false);
}

} //end namespace rbdt
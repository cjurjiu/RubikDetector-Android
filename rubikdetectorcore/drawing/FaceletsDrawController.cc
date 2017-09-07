//
// Created by catalin on 07.09.2017.
//

#include <opencv2/imgproc.hpp>
#include "FaceletsDrawController.hpp"
#include "../utils/Utils.hpp"
#include "../utils/CrossLog.hpp"

FaceletsDrawController::FaceletsDrawController(DrawConfig drawConfig) : drawConfig(drawConfig) {}

FaceletsDrawController::~FaceletsDrawController() {
    if (debuggable) {
        LOG_DEBUG("RubikJniPart.cpp", "FaceletsDrawController - destructor.");
    }
}

void FaceletsDrawController::drawResultToMat(cv::Mat &frame,
                                             const std::vector<std::vector<RubikFacelet>> result) const {

    switch (drawConfig.getDrawMode()) {
        case DrawConfig::DrawMode::DO_NOT_DRAW:
            if (debuggable) {
                LOG_DEBUG("RubikJniPart.cpp",
                          "FaceletsDrawController - DO_NOT_DRAW - doing nothing.");
            }
            return;
        case DrawConfig::DrawMode::DRAW_RECTANGLES:
            if (debuggable) {
                LOG_DEBUG("RubikJniPart.cpp", "FaceletsDrawController - DRAW_RECTANGLES.");
            }
            drawFoundFaceletsRectangles(frame, result, drawConfig.getStrokeWidth(),
                                        drawConfig.isFillShape());
            break;
        case DrawConfig::DrawMode::DRAW_CIRCLES:
            if (debuggable) {
                LOG_DEBUG("RubikJniPart.cpp", "FaceletsDrawController - DRAW_CIRCLES.");
            }
            drawFoundFaceletsCircles(frame, result, drawConfig.getStrokeWidth(),
                                     drawConfig.isFillShape());
            break;
    }
}

void FaceletsDrawController::updateDrawConfig(DrawConfig newDrawConfig) {
    drawConfig = newDrawConfig;
}

void FaceletsDrawController::setDebuggable(const bool debuggable) {
    FaceletsDrawController::debuggable = debuggable;
}

bool FaceletsDrawController::isDebuggable() const {
    return debuggable;
}

void FaceletsDrawController::drawFoundFaceletsCircles(cv::Mat &outputFrame,
                                                      const std::vector<std::vector<RubikFacelet>> facetModel,
                                                      const int strokeWidth,
                                                      const bool fillShape) const {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            RubikFacelet facelet = facetModel[i][j];
            cv::circle(outputFrame,
                       cv::Point2f(facelet.center.x, facelet.center.y),
                       (int) roundf(std::min(facelet.width, facelet.height) / 2),
                       utils::getColorAsScalar(facelet.color),
                       fillShape ? -1 : strokeWidth, CV_AA);
        }
    }
}

void FaceletsDrawController::drawFoundFaceletsRectangles(cv::Mat &outputFrame,
                                                         const std::vector<std::vector<RubikFacelet>> facetModel,
                                                         const int strokeWidth,
                                                         const bool fillShape) const {

    if (fillShape || strokeWidth <= 0) {
        //don't draw anything if fill shape is desired, or if stroke width is non-positive
        return;
    }
    //TODO add support for filled rectangles
    cv::Point points[4];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            RubikFacelet facelet = facetModel[i][j];
            std::vector<Point2d> faceletContour = facelet.corners();
            for (int k = 0; k < 4; k++) {
                points[k] = cv::Point2d(faceletContour[k].x, faceletContour[k].y);
            }
            cv::Scalar color = utils::getColorAsScalar(facelet.color);
            //draw contour
            cv::line(outputFrame, points[0], points[1], color, strokeWidth, CV_AA);
            cv::line(outputFrame, points[1], points[2], color, strokeWidth, CV_AA);
            cv::line(outputFrame, points[2], points[3], color, strokeWidth, CV_AA);
            cv::line(outputFrame, points[3], points[0], color, strokeWidth, CV_AA);
        }
    }
}

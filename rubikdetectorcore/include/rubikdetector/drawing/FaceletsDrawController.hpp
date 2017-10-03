//
// Created by catalin on 07.09.2017.
//

#ifndef RUBIKDETECTOR_FACELETSDRAWCONTROLLER_HPP
#define RUBIKDETECTOR_FACELETSDRAWCONTROLLER_HPP

#include "../data/config/DrawConfig.hpp"
#include "../data/processing/RubikFacelet.hpp"
#include "../misc/Debuggable.hpp"

namespace cv {
class Mat;
}
namespace rbdt {

/**
 * Object capable of drawing the detected facelets on an RGBA cv::Mat.
 *
 * It relies on its DrawConfig to know how the facelets should be drawn. The DrawConfig is set through the
 * constructor but can be updated through FaceletsDrawController::updateDrawConfig().
 *
 * Currently this object does not support to draw the facelets as filled rectangles.
 */
class FaceletsDrawController : Debuggable {

public:
    /**
     * Constructor of the FaceletsDrawController
     * @param drawConfig nonnull DrawConfig to be used when drawing
     * @return FaceletsDrawController
     */
    FaceletsDrawController(DrawConfig drawConfig);

    /**
     * Empty destructor. Used only for logging when debuggable.
     */
    ~FaceletsDrawController();

    /**
     * Draws the RubikFacelet objects in the array to the frame parameter.
     * @param [in/out] frame the frame in which drawing will occur. Needs to be in RGBA.
     * @param facelets 2d 3x3 array of RubikFacelet objects that need to be drawn.
     */
    void drawResultToMat(cv::Mat &frame, const std::vector<std::vector<RubikFacelet>> facelets) const;

    /**
     * Sets a new DrawConfig.
     * @param newDrawConfig  the new DrawConfig to be used.
     */
    void updateDrawConfig(DrawConfig newDrawConfig);

    void setDebuggable(const bool debuggable) override;

    bool isDebuggable() const override;

private:
    DrawConfig drawConfig;

    bool debuggable;

    void drawFoundFaceletsCircles(cv::Mat &outputFrame,
                                  const std::vector<std::vector<RubikFacelet>> facetModel,
                                  const int strokeWidth,
                                  const bool fillShape) const;

    void drawFoundFaceletsRectangles(cv::Mat &outputFrame,
                                     const std::vector<std::vector<RubikFacelet>> facetModel,
                                     const int strokeWidth,
                                     const bool fillShape) const;
};

} //end namespace rbdt
#endif //RUBIKDETECTOR_FACELETSDRAWCONTROLLER_HPP

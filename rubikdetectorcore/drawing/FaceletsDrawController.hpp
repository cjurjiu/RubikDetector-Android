//
// Created by catalin on 07.09.2017.
//

#ifndef RUBIKDETECTOR_FACELETSDRAWCONTROLLER_HPP
#define RUBIKDETECTOR_FACELETSDRAWCONTROLLER_HPP

#include "../data/DrawConfig.hpp"
#include "../data/RubikFacelet.hpp"
#include "../misc/Debuggable.hpp"

namespace cv {
    class Mat;
}

class FaceletsDrawController : Debuggable {

public:
    FaceletsDrawController(DrawConfig drawConfig);

    ~FaceletsDrawController();

    void drawResultToMat(cv::Mat &frame, const std::vector<std::vector<RubikFacelet>> result) const;

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


#endif //RUBIKDETECTOR_FACELETSDRAWCONTROLLER_HPP

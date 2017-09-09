//
// Created by catalin on 05.09.2017.
//

#ifndef RUBIKDETECTOR_FACELETSDETECTOR_HPP
#define RUBIKDETECTOR_FACELETSDETECTOR_HPP

#include <cstdint>
#include "../../../misc/public/Debuggable.hpp"
#include "OnCubeDetectionResultListener.hpp"
#include "../../../processor/public/ImageProcessor.hpp"

namespace cv {
    class Mat;
}

class FaceletsDetector : public Debuggable {
public:

    virtual ~FaceletsDetector() {

    }

    virtual std::vector<std::vector<RubikFacelet>> findFacelets(cv::Mat &frameRgba,
                                                                cv::Mat &frameGray,
                                                                const int frameNumber = 0)=0;

    virtual void
    onFrameSizeSelected(int width, int height)=0;
};

#endif //RUBIKDETECTOR_FACELETSDETECTOR_HPP

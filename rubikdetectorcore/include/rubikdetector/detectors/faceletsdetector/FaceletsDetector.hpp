//
// Created by catalin on 05.09.2017.
//

#ifndef RUBIKDETECTOR_FACELETSDETECTOR_HPP
#define RUBIKDETECTOR_FACELETSDETECTOR_HPP

#include <cstdint>
#include "../../misc/Debuggable.hpp"
#include "../../processor/ImageProcessor.hpp"

namespace cv {
class Mat;
}
namespace rbdt {

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
} //end namespace rbdt
#endif //RUBIKDETECTOR_FACELETSDETECTOR_HPP

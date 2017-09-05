//
// Created by catalin on 04.09.2017.
//

#ifndef RUBIKDETECTORANDROID_COLORDETECTOR_HPP
#define RUBIKDETECTORANDROID_COLORDETECTOR_HPP

#include "../../misc/Debuggable.hpp"
#include "../../data/RubikFacelet.hpp"

namespace cv {
    class Mat;
}

class ColorDetector : public Debuggable {
public:
    virtual ~ColorDetector() {

    };

    virtual RubikFacelet::Color detectColor(const cv::Mat &image,
                                            const float whiteRatio = 0.5,
                                            const int regionInfo = -1,
                                            const int frameNr = -1) = 0;
};


#endif //RUBIKDETECTORANDROID_COLORDETECTOR_HPP

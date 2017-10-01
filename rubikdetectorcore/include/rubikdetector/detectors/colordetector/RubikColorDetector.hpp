//
// Created by catalin on 04.09.2017.
//

#ifndef RUBIKDETECTOR_RUBIKCOLORDETECTOR_HPP
#define RUBIKDETECTOR_RUBIKCOLORDETECTOR_HPP

#include "../../processing_templates/ColorDetector.hpp"
#include "../../data/processing/RubikFacelet.hpp"

namespace cv {
class Mat;
}
namespace rbdt {

/**
 * Interface which defines a component capable of extracting the strongest color in a frame.
 */
class RubikColorDetector : public ColorDetector<const cv::Mat &, RubikFacelet::Color> {
public:
    /**
     * Empty virtual destructor.
     */
    virtual ~RubikColorDetector() {

    };
};

} //end namespace rbdt
#endif //RUBIKDETECTOR_RUBIKCOLORDETECTOR_HPP

//
// Created by catalin on 05.09.2017.
//

#ifndef RUBIKDETECTOR_RUBIKSFACELETSDETECTOR_HPP
#define RUBIKDETECTOR_RUBIKSFACELETSDETECTOR_HPP

#include <cstdint>
#include "../../processing_templates/GenericDetector.hpp"
#include "../../data/processing/RubikFacelet.hpp"

namespace cv {
class Mat;
}
namespace rbdt {

/**
 * @brief Interface which defines a component capable of detecing the presence of a Rubik's Cube face in an image, and
 * capable of returning the facelets of the visible cube face, when found.
 *
 * The input image is expected to be passed as a cv::Mat parameter to FaceletsDetector::detect(). When a result is found,
 * a 3x3 std::vector of RubikFacelet elements is returned.
 */
class RubikFaceletsDetector : public GenericDetector<cv::Mat &, std::vector<std::vector<RubikFacelet>>> {
public:

    virtual ~RubikFaceletsDetector() {}
};
} //end namespace rbdt
#endif //RUBIKDETECTOR_FACELETSDETECTOR_HPP

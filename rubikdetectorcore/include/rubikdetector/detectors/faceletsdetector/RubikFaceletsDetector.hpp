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

class FaceletsDetector : public GenericDetector<cv::Mat &, std::vector<std::vector<RubikFacelet>>> {
public:

    virtual ~FaceletsDetector() {

    }
};
} //end namespace rbdt
#endif //RUBIKDETECTOR_FACELETSDETECTOR_HPP

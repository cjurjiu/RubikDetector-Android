//
// Created by catalin on 13.07.2017.
//

#ifndef RUBIKDETECTOR_HUECOLOREVIDENCE_HPP
#define RUBIKDETECTOR_HUECOLOREVIDENCE_HPP

#include "public/RubikFacelet.hpp"

namespace rbdt {

class HueColorEvidence {

public:
    HueColorEvidence(const RubikFacelet::Color color);

    RubikFacelet::Color color;

    int evidence;

};

} //namespace rbdt
#endif //RUBIKDETECTOR_HUECOLOREVIDENCE_HPP

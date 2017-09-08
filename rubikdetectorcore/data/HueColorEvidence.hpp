//
// Created by catalin on 13.07.2017.
//

#ifndef RUBIKDETECTOR_HUECOLOREVIDENCE_HPP
#define RUBIKDETECTOR_HUECOLOREVIDENCE_HPP

#include "RubikFacelet.hpp"

class HueColorEvidence {

public:
    HueColorEvidence(const RubikFacelet::Color color);

    RubikFacelet::Color color;

    int evidence;

};


#endif //RUBIKDETECTOR_HUECOLOREVIDENCE_HPP

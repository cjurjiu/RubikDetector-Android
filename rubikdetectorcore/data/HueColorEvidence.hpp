//
// Created by catalin on 13.07.2017.
//

#ifndef RUBIKDETECTORDEMO_HUECOLOREVIDENCE_H
#define RUBIKDETECTORDEMO_HUECOLOREVIDENCE_H

#include "RubikFacelet.hpp"

class HueColorEvidence {

public:
    RubikFacelet::Color color;
    int evidence;

    HueColorEvidence(const RubikFacelet::Color color);
};


#endif //RUBIKDETECTORDEMO_HUECOLOREVIDENCE_H

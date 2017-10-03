//
// Created by catalin on 13.07.2017.
//

#ifndef RUBIKDETECTOR_HUECOLOREVIDENCE_HPP
#define RUBIKDETECTOR_HUECOLOREVIDENCE_HPP

#include "../RubikFacelet.hpp"

namespace rbdt {

/**
 * Simple data class which connects a certain RubikFacelet::Color to an int representing evidence towards picking that color, for a particular facelet.
 */
class HueColorEvidence {

public:
    HueColorEvidence(const RubikFacelet::Color color);

    RubikFacelet::Color color;

    int evidence;

};

} //namespace rbdt
#endif //RUBIKDETECTOR_HUECOLOREVIDENCE_HPP

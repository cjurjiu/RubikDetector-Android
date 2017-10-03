//
// Created by catalin on 04.09.2017.
//

#ifndef RUBIKDETECTOR_COLORDETECTOR_HPP
#define RUBIKDETECTOR_COLORDETECTOR_HPP

#include "../misc/Debuggable.hpp"

namespace rbdt {

/**
 * Interface which defines a component capable of extracting the strongest color in a frame.
 *
 * @tparam INPUT_TYPE the type of the input frame data
 * @tparam RESULT_TYPE the type of the data containing the detected color
 */
template<typename INPUT_TYPE, typename RESULT_TYPE>
class ColorDetector : public Debuggable {
public:
    /**
     * Empty virtual destructor.
     */
    virtual ~ColorDetector() {};

    /**
     * Returns the strongest color present in the frame received as a parameter.
     *
     * @param [in] inputFrame the image in which the color will be detected
     * @param [in] whiteRatio sensibility to white pixels. lower values means less senstivie, larger values means more sensitive. range - [0.0, 1.0]. default= 0.5
     * @param [in] regionInfo a unique identifier for the region for which color detection is occuring. this is purely for logging purposes
     * @param [in] frameNr the current frame number, for logging purposes.
     * @return the detected color.
     */
    virtual RESULT_TYPE detectColor(INPUT_TYPE inputFrame,
                          const float whiteRatio = 0.5,
                          const int regionInfo = -1,
                          const int frameNr = -1) = 0;
};

} //end namespace rbdt
#endif //RUBIKDETECTOR_COLORDETECTOR_HPP

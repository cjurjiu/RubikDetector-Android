//
// Created by catalin on 05.09.2017.
//

#ifndef RUBIKDETECTOR_GENERICDETECTOR_HPP
#define RUBIKDETECTOR_GENERICDETECTOR_HPP

#include <cstdint>
#include "../misc/Debuggable.hpp"

namespace rbdt {

/**
 * Defines a generic detector capable of extracting relevant information from a given image frame.
 *
 * The exact meaning of the construct "relevant information" is specific to each subclass.
 * @tparam INPUT_TYPE the type of the input frame data
 * @tparam RESULT_TYPE the type of the result
 */
template<typename INPUT_TYPE, typename RESULT_TYPE>
class GenericDetector : public Debuggable {
public:

    /**
     * Empyy virtual destructor.
     */
    virtual ~GenericDetector() {}

    /**
     * Attempts to detect relevant information within the frame received as a parameter.
     *
     * The exact meaning of "relevant information" will be implementation specific.
     *
     * @param [in] inputFrame the image in which detection will occur
     * @return nullptr if nothing is found, or RESULT_TYPE if detection was succesful.
     */
    virtual RESULT_TYPE detect(INPUT_TYPE inputFrame,
                               const int frameNumber = 0)=0;

    /**
     * Call this as soon as the size of the frame to be processed is known.
     *
     * It is the client's responsability to inform this detector whenever the input frame size is changed.
     *
     * @param [in] width of the frame to be processed, in pixels
     * @param [in] height of the frame to be processed, in pixels
     */
    virtual void onFrameSizeSelected(int width, int height)=0;
};
} //end namespace rbdt
#endif //RUBIKDETECTOR_GENERICDETECTOR_HPP

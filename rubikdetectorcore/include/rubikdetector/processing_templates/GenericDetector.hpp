//
// Created by catalin on 05.09.2017.
//

#ifndef RUBIKDETECTOR_GENERICDETECTOR_HPP
#define RUBIKDETECTOR_GENERICDETECTOR_HPP

#include <cstdint>
#include "../misc/Debuggable.hpp"

namespace rbdt {

template<typename INPUT_TYPE, typename RESULT_TYPE>
class GenericDetector : public Debuggable {
public:

    virtual ~GenericDetector() {}

    virtual RESULT_TYPE detect(INPUT_TYPE inputFrame,
                     const int frameNumber = 0)=0;

    virtual void
    onFrameSizeSelected(int width, int height)=0;
};
} //end namespace rbdt
#endif //RUBIKDETECTOR_GENERICDETECTOR_HPP

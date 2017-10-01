//
// Created by catalin on 06.09.2017.
//

#ifndef RUBIKDETECTOR_IMAGEPROPERTIES_HPP
#define RUBIKDETECTOR_IMAGEPROPERTIES_HPP

#include "../../detectors/rubikdetector/RubikProcessor.hpp"

namespace rbdt {

class ImageProperties {
public:
    ImageProperties(const int width,
                    const int height,
                    const RubikProcessor::ImageFormat inputImageFormat);

    const int width;

    const int height;

    const RubikProcessor::ImageFormat inputImageFormat;

    const RubikProcessor::ImageFormat outputImageFormat;
};

} //end namespace rbdt
#endif //RUBIKDETECTOR_IMAGEPROPERTIES_HPP

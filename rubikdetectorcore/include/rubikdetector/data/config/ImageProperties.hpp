//
// Created by catalin on 06.09.2017.
//

#ifndef RUBIKDETECTOR_IMAGEPROPERTIES_HPP
#define RUBIKDETECTOR_IMAGEPROPERTIES_HPP

#include "../../processor/ImageProcessor.hpp"

namespace rbdt {

class ImageProperties {
public:
    ImageProperties(const int width,
                    const int height,
                    const ImageProcessor::ImageFormat inputImageFormat);

    const int width;

    const int height;

    const ImageProcessor::ImageFormat inputImageFormat;

    const ImageProcessor::ImageFormat outputImageFormat;
};

} //end namespace rbdt
#endif //RUBIKDETECTOR_IMAGEPROPERTIES_HPP

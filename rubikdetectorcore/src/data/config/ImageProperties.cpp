//
// Created by catalin on 06.09.2017.
//

#include "../../../include/rubikdetector/data/config/ImageProperties.hpp"

namespace rbdt {

ImageProperties::ImageProperties(const int width,
                                 const int height,
                                 const ImageProcessor::ImageFormat inputImageFormat) :
        width(width),
        height(height),
        inputImageFormat(inputImageFormat),
        outputImageFormat(ImageProcessor::ImageFormat::RGBA8888) {
    //empty
}

} //end namespace rbdt

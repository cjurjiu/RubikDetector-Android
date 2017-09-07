//
// Created by catalin on 06.09.2017.
//

#ifndef RUBIKDETECTORANDROID_IMAGEPROPERTIES_HPP
#define RUBIKDETECTORANDROID_IMAGEPROPERTIES_HPP

#include "../processor/ImageProcessor.hpp"

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


#endif //RUBIKDETECTORANDROID_IMAGEPROPERTIES_HPP

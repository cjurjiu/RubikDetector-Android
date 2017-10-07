//
// Created by catalin on 06.09.2017.
//

#ifndef RUBIKDETECTOR_IMAGEPROPERTIES_HPP
#define RUBIKDETECTOR_IMAGEPROPERTIES_HPP

#include "../../rubikprocessor/RubikProcessor.hpp"

namespace rbdt {

/**
 * Immutable class which specifies the properties of the input & output frames processed by the RubikProcesssor.
 *
 * The properties declared by this class are the following:
 *   - input & output frames size (width & height)
 *   - input frame format, as a RubikProcessor::ImageFormat
 *   - output frame format, as a RubikProcessor::ImageFormat
 *
 *  Currently only the input frame format can be specified, since at the time being the only supported output frame format is RubikProcessor::ImageFormat::RGBA8888.
 */
class ImageProperties {
public:
    /**
     * Creates a new immutable ImageProperties object with the properties mentioned in the parameters, and with the output frame format set to RubikProcessor::ImageFormat::RGBA8888.
     *
     * @param [in] width input & output frame width, in pixels
     * @param [in] height input & output frame width, in pixels
     * @param [in] inputImageFormat - the format of the input frame, as a RubikProcessor::ImageFormat.
     * @return ImageProperties
     */
    ImageProperties(const int width,
                    const int height,
                    const RubikProcessor::ImageFormat inputImageFormat);

    /**
     * Width of both the input & output frames, in pixels.
     */
    const int width;

    /**
     * Height of both the input & output frames, in pixels.
     */
    const int height;

    /**
     * Image format of the input frame.
     */
    const RubikProcessor::ImageFormat inputImageFormat;

    /**
     * Image format of the output frame.
     */
    const RubikProcessor::ImageFormat outputImageFormat;
};

} //end namespace rbdt
#endif //RUBIKDETECTOR_IMAGEPROPERTIES_HPP

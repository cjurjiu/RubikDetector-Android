//
// Created by catalin on 06.09.2017.
//

#ifndef RUBIKDETECTOR_RUBIKPROCESSORBUILDER_HPP
#define RUBIKDETECTOR_RUBIKPROCESSORBUILDER_HPP


#include <bits/unique_ptr.h>
#include <memory>
#include "../../colordetector/RubikColorDetector.hpp"
#include "../../faceletsdetector/RubikFaceletsDetector.hpp"
#include "../../../imagesaver/ImageSaver.hpp"
#include "../RubikProcessor.hpp"
#include "../../../data/config/DrawConfig.hpp"

namespace rbdt {
/**
 * Builder class for a RubikProcessor. A new instance of the RubikProcessor can be created just with:
 *
 * <pre>
 * rbdt::RubikProcessor *rubikDetector = rbdt::RubikProcessorBuilder().build();
 * </pre>
 *
 * This creates a RubikProcessor with the following defaults:
 *   - input frame size: 320 x 240;
 *   - input frame format: RubikProcessor::ImageFormat::YUV_NV21;
 *   - DrawConfig: DrawConfig::Default()
 *   - RubikFaceletsDetector: an instance of a SimpleFaceletsDetector
 *   - RubikColorDetector: an instance of a HistogramColorDetector
 *   - ImageSaver: nullptr
 *   - debuggable: false
 *
 * See various methods customizing the properties before building the desire RubikProcessor.
 *
 */
class RubikProcessorBuilder {

public:

    /**
     * Creates a builder with default settings. This implies:
     *   - input frame size: 320 x 240;
     *   - input frame format: RubikProcessor::ImageFormat::YUV_NV21;
     *   - DrawConfig: DrawConfig::Default()
     *   - RubikFaceletsDetector: an instance of a SimpleFaceletsDetector
     *   - RubikColorDetector: an instance of a HistogramColorDetector
     *   - ImageSaver: nullptr
     *   - debuggable: false
     *
     * @return a RubikProcessorBuilder
     */
    RubikProcessorBuilder();

    /**
     * Set the resoulution of the input frame.
     *
     * This can later be changed through RubikProcessor::updateImageProperties().
     *
     * @param [in] width in pixels
     * @param [in] height in pixels
     * @return the same RubikProcessorBuilder instance
     */
    RubikProcessorBuilder &inputFrameSize(int width, int height);

    /**
     * Defines the image format used by the input frame.
     *
     * This can later be changed through RubikProcessor::updateImageProperties().
     *
     * @param [in] format a RubikProcessor::ImageFormat representing the format in which the input frame is stored
     * @return the same RubikProcessorBuilder instance
     */
    RubikProcessorBuilder &inputFrameFormat(RubikProcessor::ImageFormat format);

    /**
     * Specifies the DrawConfig. Drawing only ocurrs when he facelets are found. If you wish to not draw even
     * if the facelets are found, use DrawConfig::DoNotDraw();
     *
     * This can later be changed through RubikProcessor::updateDrawConfig().
     *
     * @param [in] drawConfig the desired DrawConfig
     * @return the same RubikProcessorBuilder instance
     */
    RubikProcessorBuilder &drawConfig(DrawConfig drawConfig);

    /**
     * Specifies the RubikColorDetector used by the RubikProcessor when detecting color.
     *
     * A custom implementation can be provided here. If not set, then a HistogramColorDetector will be used by default.
     *
     * @param [in] colorDetector the RubikColorDetector to be used.
     * @return the same RubikProcessorBuilder instance
     */
    RubikProcessorBuilder &colorDetector(std::unique_ptr<RubikColorDetector> colorDetector);

    /**
     * Specifies the RubikFaceletsDetector used by the RubikProcessor to search for the Rubik's Cube in a frame.
     *
     * A custom implementation can be provided here. If not set, then a SimpleFaceletsDetector will be used by default.
     *
     * @param [in] faceletsDetector the RubikFaceletsDetector to be used.
     * @return the same RubikProcessorBuilder instance
     */
    RubikProcessorBuilder &faceletsDetector(
            std::unique_ptr<RubikFaceletsDetector> faceletsDetector);

    /**
     * Set an ImageSaver object which can be used by the RubikProcessor to save to disk internal processing frames, when debuggable.
     *
     * Passing nullptr is safe and tells the RubikProcessor that not frames need to be saved, even if debuggable. By default,
     * nullptr is used.
     *
     * @param [in] imageSaver instance of a ImageSaver
     * @return the same RubikProcessorBuilder instance
     */
    RubikProcessorBuilder &imageSaver(std::shared_ptr<ImageSaver> imageSaver);

    /**
     * Pass true to enable logging various debugging information, false otherwisel.
     *
     * Default is false. This can be updated through RubikProcessor::setDebuggable()
     *
     * @param [in] debuggable
     * @return the same RubikProcessorBuilder instance
     */
    RubikProcessorBuilder &debuggable(bool debuggable);

    /**
     * Builds a RubikProcessor with the configuration provided through this builder.
     * @return RubikProcessor
     */
    RubikProcessor *build();

private:

    static constexpr int DEFAULT_WIDTH = 320;

    static constexpr int DEFAULT_HEIGHT = 240;

    static constexpr RubikProcessor::ImageFormat DEFAULT_FRAME_FORMAT = RubikProcessor::ImageFormat::YUV_NV21;

    int mFrameWidth;

    int mFrameHeight;

    RubikProcessor::ImageFormat mFrameFormat;

    DrawConfig mDrawConfig;

    std::unique_ptr<RubikColorDetector> mColorDetector;

    std::unique_ptr<RubikFaceletsDetector> mFaceletsDetector;

    std::shared_ptr<ImageSaver> mImageSaver;

    bool mDebuggable;
};

} //end namespace rbdt
#endif //RUBIKDETECTOR_RUBIKPROCESSORBUILDER_HPP

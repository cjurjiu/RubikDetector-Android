//
// Created by catalin on 12.07.2017.
//

#ifndef RUBIKDETECTOR_RUBIKPROCESSOR_HPP
#define RUBIKDETECTOR_RUBIKPROCESSOR_HPP

#include <vector>
#include <cstdint>
#include <memory>
#include "../data/processing/RubikFacelet.hpp"
#include "../processing_templates/ImageProcessor.hpp"
#include "../detectors/colordetector/RubikColorDetector.hpp"
#include "../detectors/faceletsdetector/RubikFaceletsDetector.hpp"
#include "../data/config/DrawConfig.hpp"
#include "../drawing/FaceletsDrawController.hpp"

namespace rbdt {

class OnCubeDetectionResultListener;

class RubikProcessorImpl;

class ImageSaver;

class RubikProcessorBuilder;

class ImageProperties;

/**
 * Concrete implementation of the ImageProcessor, capable of detecing a Rubik's Cube in an image, and of
 * returning the position & colors of the visible Rubiks' Cube facelets.
 *
 * In broad lines, the operations performed by this component are the following:
 *  -# Computes its memory requirements, s.t. it can inform its clients of these requirements through it various methods (e.g. RubikProcessor::getRequiredMemory())
 *    * this also happens every time RubikProcessor::updateImageProperties() is called)
 *  -# Performes minimal pre-processing to the input & output data (i.e. scales, convers colors, etc)
 *  -# Delegates searching for the facelets to a RubikFaceletsDetector
 *  -# Delegates to a RubikColorDetector the task of mapping each of the found RubikFacelet object to a RubikFacelet::Color
 *  -# Delegates the drawing of the found facelets to a FaceletsDrawController.
 *  -# Prints debug info & saves debug images through a ImageSaver, if debuggable mode is active.
 *
 * This class expects input data to be provided as a <i>const uint8_t *</i> array, and will return a 2D std::vector
 * of RubikFacelet objects, if the Rubik's Cube is detected within the processed frame.
 *
 * Additionally, it expects the image properties passed to RubikProcessor::updateImageProperties() to be of type ImageProperties.
 *
 * The currently supported image formats for the input frame are specified here: RubikProcessor::ImageFormat. For the output format
 * only RubikProcessor::ImageFormat::RGBA8888 is currently supported.
 *
 * Use the RubikProcessorBuilder to create instances of this class. Once created, only the following properties can be modified:
 *   - the current ImageProperties, through RubikProcessor::updateImageProperties();
 *   - the current DrawConfig, through RubikProcessor::updateDrawConfig();
 *   - the debuggable state, through RubikProcessor::setDebuggable().
 *
 * This class is <b>not</b> thread safe. If used from multiple threads, the only hard requirement is to make sure RubikProcessor::process() is
 * not called for obsolete ImageProperties. In other words, always make sure no thread calls RubikProcessor::process() before
 * RubikProcessor::updateImageProperties() is called, after a change in input image size or format.
 *
 * Enabling the debug mode on this processor makes it print information relevant for debugging. If a non-null ImageSaver is also provided through the
 * RubikProcessorBuilder, then internal processing images are saved for debugging purposes, when debugging is turned on. However, performance is drastically
 * reduced when the images are saved, since writing to disk is typically very slow.
 *
 * Please read the documentation of the ImageProcessor for more information about the inner workings & requirements of this component.
 *
 * @see ImageProcessor
 */
class RubikProcessor
        : public ImageProcessor<const uint8_t *, const ImageProperties &, std::vector<std::vector<RubikFacelet>>> {

public:

    /**
    * Defines the image formats that need to be supported by the processor for input & output frames.
    */
    enum class ImageFormat {
        YUV_NV21, YUV_NV12, YUV_I420, YUV_YV12, RGBA8888
    };

    /**
     * Virtual destructor. Only needed for debug logging.
     */
    virtual ~RubikProcessor();

    /**
     * Processes the data present in the input array. The input array is expected to have a size equal to the value
     * returned by ImageProcessor::getRequiredMemory().
     *
     * Within the array, the input frame data is required to be at the offset returned by ImageProcessor::getInputFrameBufferOffset(), and
     * to have a byte length equal to ImageProcessor::getInputFrameByteCount().
     *
     * If the input frame is modified during processing due to the settings present in the active DrawConfig, the result of the drawing will be
     * present as an "output frame". This output frame will be written back in the array at an offset equal to ImageProcessor::getOutputFrameBufferOffset(),
     * and will have a byte length equal to ImageProcessor::getOutputFrameByteCount().
     *
     * @param [in/out] imageData.
     * @return nullptr if nothing is detected in the current frame, or a 3x3 2d std::vector of RubikFacelet objects, if the cube face is found.
     *
     * @see ImageProcessor::process()
     */
    std::vector<std::vector<RubikFacelet>> process(const uint8_t *imageData) override;

    void updateImageProperties(const ImageProperties &imageProperties) override;

    void overrideInputFrameWithOutputFrame(const uint8_t *imageData) override;

    int getRequiredMemory() override;

    int getOutputFrameBufferOffset() override;

    int getOutputFrameByteCount() override;

    int getInputFrameByteCount() override;

    int getInputFrameBufferOffset() override;

    void setDebuggable(const bool debuggable) override;

    bool isDebuggable() const override;

    /**
     * Updates the DrawConfig of this RubikProcessor.
     *
     * @param drawConfig the updated DrawConfig
     */
    void updateDrawConfig(DrawConfig drawConfig);

private:
    friend class RubikProcessorBuilder;

    /**
     * Private constructor only called by the RubikProcessorBuilder.
     * @param imageProperties ImageProperties object representing the initial image properties.
     * @param faceletsDetector RubikFaceletsDetector which will be used to detect the Rubik's Cube facelets
     * @param colorDetector RubikColorDetector used to detect the facelets colors
     * @param drawController FaceletsDrawController used to draw the result over the image
     * @param imageSaver ImageSaver used to save debug frames on disk, if the processor is is debug mode
     * @return a RubikProcessor
     */
    RubikProcessor(const ImageProperties imageProperties,
                   std::unique_ptr<RubikFaceletsDetector> faceletsDetector,
                   std::unique_ptr<RubikColorDetector> colorDetector,
                   std::unique_ptr<FaceletsDrawController> drawController,
                   std::shared_ptr<ImageSaver> imageSaver);

    /**
     * Pointer to private implementation (PIMPL Pattern)
     */
    std::unique_ptr<RubikProcessorImpl> behavior;
};
} //end namespace rbdt
#endif //RUBIKDETECTOR_RUBIKPROCESSOR_HPP

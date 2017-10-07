//
// Created by catalin on 12.07.2017.
//

#ifndef RUBIKDETECTOR_RUBIKPROCESSORIMPL_HPP
#define RUBIKDETECTOR_RUBIKPROCESSORIMPL_HPP

#include <opencv2/core/core.hpp>
#include "../../processing_templates/ImageProcessor.hpp"
#include "../../detectors/faceletsdetector/RubikFaceletsDetector.hpp"
#include "../../detectors/colordetector/RubikColorDetector.hpp"
#include "../../data/geometry/internal/Circle.hpp"
#include "../../data/processing/internal/HueColorEvidence.hpp"
#include "../../imagesaver/ImageSaver.hpp"
#include "../../data/config/DrawConfig.hpp"
#include "../../drawing/FaceletsDrawController.hpp"
#include "../../rubikprocessor/RubikProcessor.hpp"
#include <iostream>
#include <memory>

namespace rbdt {
    class OnCubeDetectionResultListener;

    class RubikFacelet;

    class ImageProperties;

/**
 * Class to which a RubikProcessor delegates its work to.
 *
 * This is effectivelly the <i>private implementation</i> of a RubikProcessor's behavior.
 *
 * @warning Do not use RubikProcessorImpl directly, do not expose this in the API. Use the RubikProcessor instead.
 *
 * @note This class is effectivelly *is* the RubikProcessor (one does not make sense without the other), hence it should be regarded
 * as simply being the "RubikProcessor" instead of its implementation, or something else.
 *
 * This RubikProcessor relies on the concept of "processing frames". Typically, a processing frame is a regarded as being a scaled down
 * version of the input frame. Since there are enough details in a 320x240 frame for accurate detections to occur, we don't need the
 * full resolution of the input frame (if the input frame has a size of 1920x1080 pixels, for instance).
 *
 * Because of this this, before delegating work to its subcomponents, this processor first downscales the input frame to a resolution
 * close to 320x240 (actual aspect ratio may vary). This 320x240 frame is called the "processing frame". Generally, two processing frames
 * are required:
 *   - a grayscale one, which is used by the RubikFaceletsDetector to find where the facelets are located within the frame;
 *   - and a RGBA one, which will be used when detecting the facelets colors.
 *
 * These processing frames, when created, do not allocate additional memory. They instead are written back into the byte array received at
 * RubikProcessor::process(), at a location specific to the currently set ImageProperties. It is expected that the aforemention byte array is
 * large enough to also hold these processing frames, if its length is equal to the value returned by RubikProcessor::getRequiredMemory().
 *
 * The processing frames are written back in the array at the offset & length specified through fields such as RubikProcessorImpl::processingGrayImageOffset or
 * RubikProcessorImpl::processingRgbaImageByteCount. These are recomputed each time RubikProcessor::updateImageProperties() is called.
 *
 * Additionally, depending on the current ImageProperties, certain processing frames are not required. For instance, if the input frame size is already
 * 320x240, then the RGBA 320x240 processing frame is not needed, since the output frame will have the same format & size, and can be used instead
 * for color detection. In this case, only the grayscale processing frame will be created.
 *
 * These optimizations, although they substantially increased the implementation complexity, have been done primarily to prevent allocating & deallocating
 * memory each frame.
 *
 * @see RubikProcessor
 */
    class RubikProcessorImpl
            : public ImageProcessor<const uint8_t *, const ImageProperties &, std::vector<std::vector<RubikFacelet>>> {
    public:
        virtual ~RubikProcessorImpl();

        /**
         * @copydoc RubikProcessor::process()
         */
        std::vector<std::vector<RubikFacelet>> process(const uint8_t *imageData) override;

        void updateImageProperties(const ImageProperties &imageProperties) override;

        void overrideInputFrameWithOutputFrame(const uint8_t *imageData) override;

        int getRequiredMemory() override;

        int getOutputFrameBufferOffset() override;

        int getOutputFrameByteCount() override;

        int getInputFrameByteCount() override;

        int getInputFrameBufferOffset() override;

        void setDebuggable(const bool isDebuggable) override;

        bool isDebuggable() const override;

        /**
         * @copydoc RubikProcessor::updateDrawConfig()
         */
        void updateDrawConfig(DrawConfig drawConfig);

    private:
        friend class RubikProcessor;

        /**
         * Private constructor only called by the RubikProcessor.
         * @param imageProperties ImageProperties object representing the initial image properties.
         * @param faceletsDetector RubikFaceletsDetector which will be used to detect the Rubik's Cube facelets
         * @param colorDetector RubikColorDetector used to detect the facelets colors
         * @param drawController FaceletsDrawController used to draw the result over the image
         * @param imageSaver ImageSaver used to save debug frames on disk, if the processor is is debug mode
         * @return a RubikProcessorImpl
         */
        RubikProcessorImpl(const ImageProperties imageProperties,
                           std::unique_ptr<RubikFaceletsDetector> faceletsDetector,
                           std::unique_ptr<RubikColorDetector> colorDetector,
                           std::unique_ptr<FaceletsDrawController> faceletsDrawController,
                           std::shared_ptr<ImageSaver> imageSaver);

        std::vector<std::vector<RubikFacelet>> findCubeInternal(const uint8_t *imageData);

        /**
         * Extracts the color of each RubikFacelet in the array, then updates its color to match its detected color.
         *
         * This method relies on the RubikColorDetector instance associated with this RubikProcessor to detect the colors.
         *
         * @param [in] currentFrame The input frame which contains the detected facelet, which will be used to extract their colors.
         * @param [in] facetModel 3x3 std::vector of detected facelets.
         *
         * @return a 3x3 std::vector of RubikFacelet object with their correct colors set.
         */
        std::vector<std::vector<RubikFacelet::Color>> detectFacetColors(const cv::Mat &currentFrame,
                                                                        const std::vector<std::vector<RubikFacelet>> facetModel);

        /**
         * Applies the colors in the color array, to the facelets in the facelets array.
         *
         * @param [in/out] facelets that will be updated with correct color
         * @param colors the detected facelets color.
         */
        void applyColorsToResult(std::vector<std::vector<RubikFacelet>> &facelets,
                                 const std::vector<std::vector<RubikFacelet::Color>> colors);

        /**
         * Upscales the detected facelets positions to match the resolution of the desired output frame.
         *
         * This is necessary because processing is typically performed at resolutions lower that the desired output resolution,
         * for performance  considerations. Give this, the result is not usable, without first upscaling it to the desired
         * resolution.
         *
         * @param facelets 3x3 std::vector of detected RubikFacelet objects, to be upscaled
         */
        void upscaleResult(std::vector<std::vector<RubikFacelet>> &facelets);

        /**
         * Applies updated ImageProperties to this processor. This promts the RubikProcessor to recompute its memory requirements,
         * scaling factors, processing size, and others.
         *
         * @param properties updated ImageProperties
         */
        void applyImageProperties(const ImageProperties &properties);

        /**
        * Just a value representing that there is no need to perform any conversion on the current frame.
        *
        * The exact value of the int is not important. It just needs to differ from any of the codes used by cv::cvtColor(...)
        */
        static constexpr int NO_CONVERSION_NEEDED = 2504;

        /**
         * Default expected largest dimension(i.e. max(width, height)) of the input frame.
         */
        static constexpr int DEFAULT_DIMENSION = 320;

        static constexpr int NO_OFFSET = 0;

        /**
         * Detector used to search for facelets within the frame
         */
        std::unique_ptr<RubikFaceletsDetector> faceletsDetector;

        /**
         * Detector used to extract facelet colors from the frame, once found
         */
        std::unique_ptr<RubikColorDetector> colorDetector;

        /**
         * Component used to draw the facelets on the output frame, if required
         */
        std::unique_ptr<FaceletsDrawController> faceletsDrawController;

        /**
         * Used to save debugging frames, if present and this RubikProcessor is is debuggable mode.
         */
        std::shared_ptr<ImageSaver> imageSaver;

        /**
         * used to compute the processing frame rate
         */
        int frameNumber = 0;

        /**
         * used to compute the processing frame rate
         */
        int frameRateSum = 0;

        bool debuggable = false;

        /**
         * input & output frame height
         */
        int imageHeight;

        /**
         * input & output frame width
         */
        int imageWidth;

        /**
         * total required length in bytes of the input array passed to RubikProcessor::process()
         *
         * @see RubikProcessor::getRequiredMemory()
         */
        int totalRequiredMemory;

        /**
         * @see RubikProcessor::getOutputFrameBufferOffset()
         */
        int outputRgbaImageOffset;

        /**
         * @see RubikProcessor::getOutputFrameByteCount()
         */
        int outputRgbaImageByteCount;

        /**
         * @see RubikProcessor::getInputFrameBufferOffset()
         */
        int inputImageOffset;

        /**
         * @see RubikProcessor::getInputFrameByteCount()
         */
        int inputImageByteCount;

        /**
         * ratio used to upscale the detected facelets to the resolution of the output frame
         */
        float upscalingRatio;

        /**
         * ratio used to downscale the input frame to a size fit for processing
         */
        float downscalingRatio;

        /**
         * basically max(inputFrameWidth, inputFrameHeight)
         */
        int largestDimension;

        /**
         * computed width of the processing frame width
         */
        int processingWidth;

        /**
         * computed width of the processing frame height
         */
        int processingHeight;

        /**
         * true if the given input frame is too large, and should be downscaled to a lower resolution,
         * with lower impact on processing (i.e. the "processing size")
         */
        bool needsResize;

        /**
         * the offset in the input array where the processing frame (RGBA) will be saved while processing
         */
        int processingRgbaImageOffset;

        /**
         * the size in bytes of the RGBA processing frame
         */
        int processingRgbaImageByteCount;

        /**
         * the offset in the input array where the processing frame (GRAYSCALE) will be saved while processing
         */
        int processingGrayImageOffset;

        /**
         * the size in bytes of the GRAYSCALE processing frame
         */
        int processingGrayImageSize;

        /**
         * the image format of the input frame.
         */
        RubikProcessor::ImageFormat inputImageFormat;

        /**
         * the opencv value passed to cv::cvtColor when changing color from the input frame format to the output frame format
         */
        int cvColorConversionCode;
    };

} //namespace rbdt
#endif //RUBIKDETECTOR_RUBIKPROCESSORIMPL_HPP

//
// Created by catalin on 12.07.2017.
//

#ifndef RUBIKDETECTOR_RUBIKPROCESSOR_HPP
#define RUBIKDETECTOR_RUBIKPROCESSOR_HPP

#include <vector>
#include <cstdint>
#include <memory>
#include "../../data/processing/RubikFacelet.hpp"
#include "../../processing_templates/ImageProcessor.hpp"
#include "../colordetector/RubikColorDetector.hpp"
#include "../faceletsdetector/RubikFaceletsDetector.hpp"
#include "../../data/config/DrawConfig.hpp"
#include "../../drawing/FaceletsDrawController.hpp"

namespace rbdt {

class OnCubeDetectionResultListener;

class RubikProcessorImpl;

class ImageSaver;

class RubikProcessorBuilder;

class ImageProperties;

class RubikProcessor
        : public ImageProcessor<const uint8_t *, const ImageProperties &, std::vector<std::vector<RubikFacelet>>> {

public:

    /**
    * Defines the image formats that need to be supported by the processor for input & output frames.
    */
    enum class ImageFormat {
        YUV_NV21, YUV_NV12, YUV_I420, YUV_YV12, RGBA8888
    };

    virtual ~RubikProcessor();

    std::vector<std::vector<RubikFacelet>> process(const uint8_t *imageData) override;

    void
    updateImageProperties(const ImageProperties &imageProperties) override;

    void overrideInputFrameWithOutputFrame(const uint8_t *imageData) override;

    int getRequiredMemory() override;

    int getOutputFrameBufferOffset() override;

    int getOutputFrameByteCount() override;

    int getInputFrameByteCount() override;

    int getInputFrameBufferOffset() override;

    void setDebuggable(const bool debuggable) override;

    bool isDebuggable() const override;

    void updateDrawConfig(DrawConfig drawConfig);

private:
    friend class RubikProcessorBuilder;

    RubikProcessor(const ImageProperties imageProperties,
                   std::unique_ptr<FaceletsDetector> faceletsDetector,
                   std::unique_ptr<RubikColorDetector> colorDetector,
                   std::unique_ptr<FaceletsDrawController> drawController,
                   std::shared_ptr<ImageSaver> imageSaver);

    std::unique_ptr<RubikProcessorImpl> behavior;
};
} //end namespace rbdt
#endif //RUBIKDETECTOR_RUBIKPROCESSOR_HPP

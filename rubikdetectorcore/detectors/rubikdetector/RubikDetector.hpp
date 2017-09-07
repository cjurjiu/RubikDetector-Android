//
// Created by catalin on 12.07.2017.
//

#ifndef RUBIKDETECTORDEMO_RUBIKDETECTOR_HPP
#define RUBIKDETECTORDEMO_RUBIKDETECTOR_HPP

#include <vector>
#include <cstdint>
#include <memory>
#include "../../data/RubikFacelet.hpp"
#include "../../processor/ImageProcessor.hpp"
#include "../colordetector/ColorDetector.hpp"
#include "../faceletsdetector/FaceletsDetector.hpp"
#include "../../data/ImageProperties.hpp"
#include "../../data/DrawConfig.hpp"
#include "../../drawing/FaceletsDrawController.hpp"

class OnCubeDetectionResultListener;

class RubikDetectorBehavior;

class ImageSaver;

class RubikDetectorBuilder;

class RubikDetector : public ImageProcessor {
    friend class RubikDetectorBuilder;

private:
    RubikDetector(const ImageProperties imageProperties,
                  std::unique_ptr<FaceletsDetector> faceletsDetector,
                  std::unique_ptr<ColorDetector> colorDetector,
                  std::unique_ptr<FaceletsDrawController> drawController,
                  std::shared_ptr<ImageSaver> imageSaver);

public:
    virtual ~RubikDetector();

    std::vector<std::vector<RubikFacelet>> process(const uint8_t *imageData) override;

    void
    updateImageProperties(const ImageProperties &imageProperties) override;

    void overrideInputFrameWithResultFrame(const uint8_t *imageData) override;

    int getRequiredMemory() override;

    int getOutputFrameBufferOffset() override;

    int getOutputFrameByteCount() override;

    int getInputFrameByteCount() override;

    int getInputFrameBufferOffset() override;

    void setDebuggable(const bool debuggable) override;

    bool isDebuggable() const override;

    void updateDrawConfig(DrawConfig drawConfig);

private:
    std::unique_ptr<RubikDetectorBehavior> behavior;
};

#endif //RUBIKDETECTORDEMO_RUBIKDETECTOR_HPP

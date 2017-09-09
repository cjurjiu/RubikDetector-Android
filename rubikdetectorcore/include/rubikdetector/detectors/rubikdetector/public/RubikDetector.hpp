//
// Created by catalin on 12.07.2017.
//

#ifndef RUBIKDETECTOR_RUBIKDETECTOR_HPP
#define RUBIKDETECTOR_RUBIKDETECTOR_HPP

#include <vector>
#include <cstdint>
#include <memory>
#include "../../../data/processing/public/RubikFacelet.hpp"
#include "../../../processor/public/ImageProcessor.hpp"
#include "../../colordetector/public/ColorDetector.hpp"
#include "../../faceletsdetector/public/FaceletsDetector.hpp"
#include "../../../data/config/public/ImageProperties.hpp"
#include "../../../data/config/public/DrawConfig.hpp"
#include "../../../drawing/public/FaceletsDrawController.hpp"

namespace rbdt {

class OnCubeDetectionResultListener;

class RubikDetectorImpl;

class ImageSaver;

class RubikDetectorBuilder;

class RubikDetector : public ImageProcessor {

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
    friend class RubikDetectorBuilder;

    RubikDetector(const ImageProperties imageProperties,
                  std::unique_ptr<FaceletsDetector> faceletsDetector,
                  std::unique_ptr<ColorDetector> colorDetector,
                  std::unique_ptr<FaceletsDrawController> drawController,
                  std::shared_ptr<ImageSaver> imageSaver);

    std::unique_ptr<RubikDetectorImpl> behavior;
};
} //end namespace rbdt
#endif //RUBIKDETECTOR_RUBIKDETECTOR_HPP

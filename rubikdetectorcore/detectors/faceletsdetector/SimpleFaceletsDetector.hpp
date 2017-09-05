//
// Created by catalin on 12.07.2017.
//

#ifndef RUBIKDETECTORDEMO_CUBEDETECTOR_HPP
#define RUBIKDETECTORDEMO_CUBEDETECTOR_HPP

#include <vector>
#include <cstdint>
#include <memory>
#include "../../data/RubikFacelet.hpp"
#include "FaceletsDetector.hpp"

class OnCubeDetectionResultListener;

class SimpleFaceletsDetectorBehavior;

class ImageSaver;

class SimpleFaceletsDetector : public FaceletsDetector {
public:
    SimpleFaceletsDetector();

    SimpleFaceletsDetector(std::shared_ptr<ImageSaver> imageSaver);

    virtual ~SimpleFaceletsDetector();

    std::vector<std::vector<RubikFacelet>> findCube(const uint8_t *imageData) override;

    void findCubeAsync(const uint8_t *imageData) override;

    void setOnCubeDetectionResultListener(OnCubeDetectionResultListener *listener) override;

    void setImageProperties(int width, int height,
                            ImageFormat imageFormat = ImageFormat::YUV_NV21) override;

    void overrideInputFrameWithResultFrame(const uint8_t *imageData) override;

    void setShouldDrawFoundFacelets(bool shouldDrawFoundFacelets) override;

    int getRequiredMemory() override;

    int getOutputFrameBufferOffset() override;

    int getOutputFrameByteCount() override;

    int getInputFrameByteCount() override;

    int getInputFrameBufferOffset() override;

    void setDebuggable(const bool debuggable) override;

    bool isDebuggable() const override;

private:
    std::unique_ptr<SimpleFaceletsDetectorBehavior> behavior;
};

#endif //RUBIKDETECTORDEMO_CUBEDETECTOR_HPP

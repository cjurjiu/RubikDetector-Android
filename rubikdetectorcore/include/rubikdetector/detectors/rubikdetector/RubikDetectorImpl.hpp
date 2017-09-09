//
// Created by catalin on 12.07.2017.
//

#ifndef RUBIKDETECTOR_RUBIKDETECTORIMPL_HPP
#define RUBIKDETECTOR_RUBIKDETECTORIMPL_HPP

#include <opencv2/core/core.hpp>
#include "../../processor/public/ImageProcessor.hpp"
#include "../faceletsdetector/public/FaceletsDetector.hpp"
#include "../colordetector/public/ColorDetector.hpp"
#include "../../data/geometry/Circle.hpp"
#include "../../data/processing/HueColorEvidence.hpp"
#include "../../imagesaver/public/ImageSaver.hpp"
#include "../../data/config/public/ImageProperties.hpp"
#include "../../data/config/public/DrawConfig.hpp"
#include "../../drawing/public/FaceletsDrawController.hpp"
#include <iostream>
#include <memory>

namespace rbdt {
class OnCubeDetectionResultListener;

class RubikFacelet;

class RubikDetectorImpl : ImageProcessor {
public:
    RubikDetectorImpl(const ImageProperties imageProperties,
                      std::unique_ptr<FaceletsDetector> faceletsDetector,
                      std::unique_ptr<ColorDetector> colorDetector,
                      std::unique_ptr<FaceletsDrawController> faceletsDrawController,
                      std::shared_ptr<ImageSaver> imageSaver);

    virtual ~RubikDetectorImpl();

    std::vector<std::vector<RubikFacelet>> process(const uint8_t *imageData) override;

    void updateImageProperties(const ImageProperties &imageProperties) override;

    void overrideInputFrameWithResultFrame(const uint8_t *imageData) override;

    int getRequiredMemory() override;

    int getOutputFrameBufferOffset() override;

    int getOutputFrameByteCount() override;

    int getInputFrameByteCount() override;

    int getInputFrameBufferOffset() override;

    void setDebuggable(const bool isDebuggable) override;

    bool isDebuggable() const override;

    void updateDrawConfig(DrawConfig drawConfig);

private:

    std::vector<std::vector<RubikFacelet>> findCubeInternal(const uint8_t *imageData);

    std::vector<std::vector<RubikFacelet::Color>> detectFacetColors(const cv::Mat &currentFrame,
                                                                    const std::vector<std::vector<RubikFacelet>> facetModel);

    void applyColorsToResult(std::vector<std::vector<RubikFacelet>> &facelets,
                             const std::vector<std::vector<RubikFacelet::Color>> colors);

    void upscaleResult(std::vector<std::vector<RubikFacelet>> &facelets);

    void applyImageProperties(const ImageProperties &properties);

    static constexpr int DEFAULT_DIMENSION = 320;

    static constexpr int NO_CONVERSION_NEEDED = 2504;

    static constexpr int NO_OFFSET = 0;

    std::unique_ptr<FaceletsDetector> faceletsDetector;

    std::unique_ptr<ColorDetector> colorDetector;

    std::unique_ptr<FaceletsDrawController> faceletsDrawController;

    std::shared_ptr<ImageSaver> imageSaver;

    int frameNumber = 0;

    int frameRateSum = 0;

    bool debuggable = false;

    int imageHeight;

    int imageWidth;

    int totalRequiredMemory;

    int outputRgbaImageOffset;

    int outputRgbaImageByteCount;

    int inputImageByteCount;

    float upscalingRatio;

    int processingWidth;

    int processingHeight;

    int largestDimension;

    float downscalingRatio;

    bool needsResize;

    int inputImageOffset;

    int processingRgbaImageOffset;

    int processingRgbaImageByteCount;

    int processingGrayImageOffset;

    int processingGrayImageSize;

    ImageProcessor::ImageFormat inputImageFormat;

    int cvColorConversionCode;
};

} //namespace rbdt
#endif //RUBIKDETECTOR_RUBIKDETECTORIMPL_HPP

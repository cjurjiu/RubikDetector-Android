//
// Created by catalin on 05.09.2017.
//

#ifndef RUBIKDETECTOR_IMAGEPROCESSOR_HPP
#define RUBIKDETECTOR_IMAGEPROCESSOR_HPP

#include <cstdint>
#include "../../misc/public/Debuggable.hpp"
#include "../../data/processing/public/RubikFacelet.hpp"

class ImageProperties;

class ImageProcessor : public Debuggable {
public:

    enum class ImageFormat {
        YUV_NV21, YUV_NV12, YUV_I420, YUV_YV12, RGBA8888
    };

    virtual ~ImageProcessor() {}

    virtual std::vector<std::vector<RubikFacelet>> process(const uint8_t *imageData)=0;

    virtual void
    updateImageProperties(const ImageProperties &imageProperties)=0;

    virtual void overrideInputFrameWithResultFrame(const uint8_t *imageData)=0;

    virtual int getRequiredMemory()=0;

    virtual int getOutputFrameBufferOffset()=0;

    virtual int getOutputFrameByteCount()=0;

    virtual int getInputFrameByteCount()=0;

    virtual int getInputFrameBufferOffset()=0;
};

#endif //RUBIKDETECTOR_IMAGEPROCESSOR_HPP
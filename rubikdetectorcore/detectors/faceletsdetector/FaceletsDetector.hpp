//
// Created by catalin on 05.09.2017.
//

#ifndef RUBIKDETECTORANDROID_FACELETSDETECTOR_HPP
#define RUBIKDETECTORANDROID_FACELETSDETECTOR_HPP

#include <cstdint>
#include "../../misc/Debuggable.hpp"
#include "OnCubeDetectionResultListener.hpp"

class FaceletsDetector : public Debuggable {
public:

    enum class ImageFormat {
        YUV_NV21, YUV_NV12, YUV_I420, YUV_YV12, RGBA8888
    };

    virtual ~FaceletsDetector() {

    }

    virtual std::vector<std::vector<RubikFacelet>> findCube(const uint8_t *imageData)=0;

    virtual void findCubeAsync(const uint8_t *imageData)=0;

    virtual void setOnCubeDetectionResultListener(OnCubeDetectionResultListener *listener)=0;

    virtual void
    setImageProperties(int width, int height, ImageFormat imageFormat = ImageFormat::YUV_NV21)=0;

    virtual void overrideInputFrameWithResultFrame(const uint8_t *imageData)=0;

    virtual void setShouldDrawFoundFacelets(bool shouldDrawFoundFacelets)=0;

    virtual int getRequiredMemory()=0;

    virtual int getOutputFrameBufferOffset()=0;

    virtual int getOutputFrameByteCount()=0;

    virtual int getInputFrameByteCount()=0;

    virtual int getInputFrameBufferOffset()=0;
};

#endif //RUBIKDETECTORANDROID_FACELETSDETECTOR_HPP

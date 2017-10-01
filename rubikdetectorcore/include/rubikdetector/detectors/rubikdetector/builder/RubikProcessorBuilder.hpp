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

class RubikProcessorBuilder {

public:

    RubikProcessorBuilder();

    RubikProcessorBuilder &inputFrameSize(int width, int height);

    RubikProcessorBuilder &inputFrameFormat(RubikProcessor::ImageFormat format);

    RubikProcessorBuilder &drawConfig(DrawConfig drawConfig);

    RubikProcessorBuilder &colorDetector(std::unique_ptr<RubikColorDetector> colorDetector);

    RubikProcessorBuilder &faceletsDetector(std::unique_ptr<FaceletsDetector> faceletsDetector);

    RubikProcessorBuilder &imageSaver(std::shared_ptr<ImageSaver> imageSaver);

    RubikProcessorBuilder &debuggable(bool debuggable);

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

    std::unique_ptr<FaceletsDetector> mFaceletsDetector;

    std::shared_ptr<ImageSaver> mImageSaver;

    bool mDebuggable;
};

} //end namespace rbdt
#endif //RUBIKDETECTOR_RUBIKPROCESSORBUILDER_HPP

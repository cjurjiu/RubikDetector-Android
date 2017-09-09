//
// Created by catalin on 06.09.2017.
//

#ifndef RUBIKDETECTOR_RUBIKDETECTORBUILDER_HPP
#define RUBIKDETECTOR_RUBIKDETECTORBUILDER_HPP


#include <bits/unique_ptr.h>
#include <memory>
#include "../../colordetector/ColorDetector.hpp"
#include "../../faceletsdetector/FaceletsDetector.hpp"
#include "../../../imagesaver/ImageSaver.hpp"
#include "../RubikDetector.hpp"
#include "../../../data/config/DrawConfig.hpp"

namespace rbdt {

class RubikDetectorBuilder {

public:

    RubikDetectorBuilder();

    RubikDetectorBuilder &inputFrameSize(int width, int height);

    RubikDetectorBuilder &inputFrameFormat(ImageProcessor::ImageFormat format);

    RubikDetectorBuilder &drawConfig(DrawConfig drawConfig);

    RubikDetectorBuilder &colorDetector(std::unique_ptr<ColorDetector> colorDetector);

    RubikDetectorBuilder &faceletsDetector(std::unique_ptr<FaceletsDetector> faceletsDetector);

    RubikDetectorBuilder &imageSaver(std::shared_ptr<ImageSaver> imageSaver);

    RubikDetectorBuilder &debuggable(bool debuggable);

    RubikDetector *build();

private:

    static constexpr int DEFAULT_WIDTH = 320;

    static constexpr int DEFAULT_HEIGHT = 240;

    static constexpr ImageProcessor::ImageFormat DEFAULT_FRAME_FORMAT = ImageProcessor::ImageFormat::YUV_NV21;

    int mFrameWidth;

    int mFrameHeight;

    ImageProcessor::ImageFormat mFrameFormat;

    DrawConfig mDrawConfig;

    std::unique_ptr<ColorDetector> mColorDetector;

    std::unique_ptr<FaceletsDetector> mFaceletsDetector;

    std::shared_ptr<ImageSaver> mImageSaver;

    bool mDebuggable;
};

} //end namespace rbdt
#endif //RUBIKDETECTOR_RUBIKDETECTORBUILDER_HPP

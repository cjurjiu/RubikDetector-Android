//
// Created by catalin on 06.09.2017.
//

#include "../../../include/rubikdetector/rubikprocessor/builder/RubikProcessorBuilder.hpp"
#include "../../../include/rubikdetector/detectors/faceletsdetector/SimpleFaceletsDetector.hpp"
#include "../../../include/rubikdetector/detectors/colordetector/HistogramColorDetector.hpp"
#include "../../../include/rubikdetector/data/config/ImageProperties.hpp"

namespace rbdt {

RubikProcessorBuilder::RubikProcessorBuilder() :
        mFrameWidth(DEFAULT_WIDTH),
        mFrameHeight(DEFAULT_HEIGHT),
        mFrameFormat(DEFAULT_FRAME_FORMAT),
        mDrawConfig(DrawConfig::Default()),
        mDebuggable(false),
        mFaceletsDetector(nullptr),
        mColorDetector(nullptr),
        mImageSaver(nullptr) {}

RubikProcessorBuilder &RubikProcessorBuilder::inputFrameSize(int width, int height) {
    mFrameWidth = width;
    mFrameHeight = height;
    return *this;
}

RubikProcessorBuilder &RubikProcessorBuilder::inputFrameFormat(RubikProcessor::ImageFormat format) {
    mFrameFormat = format;
    return *this;
}

RubikProcessorBuilder &RubikProcessorBuilder::drawConfig(DrawConfig drawConfig) {
    mDrawConfig = drawConfig;
    return *this;
}

RubikProcessorBuilder &RubikProcessorBuilder::colorDetector(
        std::unique_ptr<RubikColorDetector> colorDetector) {
    mColorDetector = std::move(colorDetector);
    return *this;
}

RubikProcessorBuilder &RubikProcessorBuilder::faceletsDetector(
        std::unique_ptr<RubikFaceletsDetector> faceletsDetector) {
    mFaceletsDetector = std::move(faceletsDetector);
    return *this;
}

RubikProcessorBuilder &RubikProcessorBuilder::imageSaver(std::shared_ptr<ImageSaver> imageSaver) {
    mImageSaver = imageSaver;
    return *this;
}

RubikProcessorBuilder &RubikProcessorBuilder::debuggable(bool debuggable) {
    mDebuggable = debuggable;
    return *this;
}

RubikProcessor *RubikProcessorBuilder::build() {

    if (mFaceletsDetector == nullptr) {
        //create default facelets detector, if a custom one wasn't set by caller
        mFaceletsDetector = std::unique_ptr<SimpleFaceletsDetector>(
                new SimpleFaceletsDetector(mImageSaver));
    }

    if (mColorDetector == nullptr) {
        //create default color detector, if a custom one wasn't set by caller
        mColorDetector = std::unique_ptr<HistogramColorDetector>(
                new HistogramColorDetector(mImageSaver));
    }

    std::unique_ptr<FaceletsDrawController> faceletsDrawController(
            new FaceletsDrawController(mDrawConfig));

    RubikProcessor *rubikDetector = new RubikProcessor(
            ImageProperties(mFrameWidth, mFrameHeight, mFrameFormat),
            std::move(mFaceletsDetector),
            std::move(mColorDetector),
            std::move(faceletsDrawController),
            mImageSaver);

    rubikDetector->setDebuggable(mDebuggable);
    return rubikDetector;
}

} //end namespace rbdt
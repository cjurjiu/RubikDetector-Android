//
// Created by catalin on 06.09.2017.
//

#include "RubikDetectorBuilder.hpp"
#include "../../faceletsdetector/SimpleFaceletsDetector.hpp"
#include "../../colordetector/HistogramColorDetector.hpp"

RubikDetectorBuilder::RubikDetectorBuilder() :
        mFrameWidth(DEFAULT_WIDTH),
        mFrameHeight(DEFAULT_HEIGHT),
        mFrameFormat(DEFAULT_FRAME_FORMAT),
        mDrawConfig(DrawConfig::Default()),
        mDebuggable(false),
        mFaceletsDetector(nullptr),
        mColorDetector(nullptr),
        mImageSaver(nullptr) {}

RubikDetectorBuilder &RubikDetectorBuilder::inputFrameSize(int width, int height) {
    mFrameWidth = width;
    mFrameHeight = height;
    return *this;
}

RubikDetectorBuilder &RubikDetectorBuilder::inputFrameFormat(ImageProcessor::ImageFormat format) {
    mFrameFormat = format;
    return *this;
}

RubikDetectorBuilder &RubikDetectorBuilder::drawConfig(DrawConfig drawConfig) {
    mDrawConfig = drawConfig;
    return *this;
}

RubikDetectorBuilder &
RubikDetectorBuilder::colorDetector(std::unique_ptr<ColorDetector> colorDetector) {
    mColorDetector = std::move(colorDetector);
    return *this;
}

RubikDetectorBuilder &
RubikDetectorBuilder::faceletsDetector(std::unique_ptr<FaceletsDetector> faceletsDetector) {
    mFaceletsDetector = std::move(faceletsDetector);
    return *this;
}

RubikDetectorBuilder &RubikDetectorBuilder::imageSaver(std::shared_ptr<ImageSaver> imageSaver) {
    mImageSaver = imageSaver;
    return *this;
}

RubikDetectorBuilder &RubikDetectorBuilder::debuggable(bool debuggable) {
    mDebuggable = debuggable;
    return *this;
}

RubikDetector *RubikDetectorBuilder::build() {

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

    RubikDetector *rubikDetector = new RubikDetector(
            ImageProperties(mFrameWidth, mFrameHeight, mFrameFormat),
            std::move(mFaceletsDetector),
            std::move(mColorDetector),
            std::move(faceletsDrawController),
            mImageSaver);

    rubikDetector->setDebuggable(mDebuggable);
    return rubikDetector;
}

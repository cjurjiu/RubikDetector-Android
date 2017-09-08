//
// Created by catalin on 12.07.2017.
//

#ifndef RUBIKDETECTOR_SIMPLEFACELETSDETECTOR_HPP
#define RUBIKDETECTOR_SIMPLEFACELETSDETECTOR_HPP

#include <vector>
#include <cstdint>
#include <memory>
#include "../../data/RubikFacelet.hpp"
#include "FaceletsDetector.hpp"

namespace cv {
    class Mat;
}

class OnCubeDetectionResultListener;

class SimpleFaceletsDetectorImpl;

class ImageSaver;

class SimpleFaceletsDetector : public FaceletsDetector {
public:
    SimpleFaceletsDetector();

    SimpleFaceletsDetector(std::shared_ptr<ImageSaver> imageSaver);

    virtual ~SimpleFaceletsDetector();

    virtual std::vector<std::vector<RubikFacelet>> findFacelets(cv::Mat &frameRgba,
                                                                cv::Mat &frameGray,
                                                                const int frameNumber = 0) override;

    void onFrameSizeSelected(int width, int height) override;

    void setDebuggable(const bool debuggable) override;

    bool isDebuggable() const override;

private:
    std::unique_ptr<SimpleFaceletsDetectorImpl> behavior;
};

#endif //RUBIKDETECTOR_SIMPLEFACELETSDETECTOR_HPP

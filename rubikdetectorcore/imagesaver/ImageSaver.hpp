//
// Created by catalin on 31.07.2017.
//

#ifndef RUBIKDETECTOR_IMAGESAVER_HPP
#define RUBIKDETECTOR_IMAGESAVER_HPP

#include <string>
#include "../misc/Debuggable.hpp"

namespace cv {
    class Mat;
}

class ImageSaver : Debuggable {
public:
    ImageSaver(const std::string saveLocation);

    ~ImageSaver();

    bool saveImage(const cv::Mat &mat, const int frameNumber, const int regionId);

    bool saveImage(const cv::Mat &mat, const int frameNumber, const std::string regionName);

    void setDebuggable(const bool debuggable) override;

    bool isDebuggable() const override;

private:
    const std::string path;
    bool debuggable;
};


#endif //RUBIKDETECTOR_IMAGESAVER_HPP

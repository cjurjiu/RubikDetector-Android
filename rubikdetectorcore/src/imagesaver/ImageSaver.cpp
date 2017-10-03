//
// Created by catalin on 31.07.2017.
//

#include <sstream>
#include <opencv2/highgui/highgui.hpp>
#include "../../include/rubikdetector/imagesaver/ImageSaver.hpp"
#include "../../include/rubikdetector/utils/CrossLog.hpp"

namespace rbdt {

ImageSaver::ImageSaver(const std::string saveLocation) : path(saveLocation) {}

ImageSaver::~ImageSaver() {
    if (debuggable) {
        LOG_DEBUG("RubikJniPart.cpp", "ImageSaver - destructor.");
    }
}

bool ImageSaver::saveImage(const cv::Mat &mat, const int frameNumber, const int regionId) {
    std::stringstream regionIdStringStream;
    regionIdStringStream << regionId;
    return saveImage(mat, frameNumber, regionIdStringStream.str());
}

bool ImageSaver::saveImage(const cv::Mat &mat,
                           const int frameNumber,
                           const std::string regionName) {
    std::stringstream frameNrStringStream;
    frameNrStringStream << frameNumber;
    std::string store_path = path + "/pic_" + frameNrStringStream.str() + "_" + regionName + ".jpg";
    return cv::imwrite(store_path, mat);
}

void ImageSaver::setDebuggable(const bool debuggable) {
    ImageSaver::debuggable = debuggable;
}

bool ImageSaver::isDebuggable() const {
    return ImageSaver::debuggable;
}

} //end namespace rbdt
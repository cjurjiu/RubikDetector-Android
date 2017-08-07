//
// Created by catalin on 31.07.2017.
//

#ifndef RUBIKDETECTORANDROID_IMAGESAVER_H
#define RUBIKDETECTORANDROID_IMAGESAVER_H

#include <string>

namespace cv {
    class Mat;
}

class ImageSaver {
public:
    ImageSaver(const std::string saveLocation);

    ~ImageSaver();

    bool saveImage(const cv::Mat &mat, const int frameNumber, const int regionId);

    bool saveImage(const cv::Mat &mat, const int frameNumber, const std::string regionName);

private:
    const std::string path;

};


#endif //RUBIKDETECTORANDROID_IMAGESAVER_H

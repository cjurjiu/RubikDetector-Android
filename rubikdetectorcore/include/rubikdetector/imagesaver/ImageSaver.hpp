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
namespace rbdt {

/**
 * Object capable of saving arbitrary cv::Mat objects to disk, primarily for debugging purposes.
 */
class ImageSaver : Debuggable {
public:
    /**
     * Constructor.
     * @param [in] saveLocation a std::string poiting to a local storage location where write access is granted. This is where the
     * saved frames will be saved.
     * @return a ImageSaver
     */
    ImageSaver(const std::string saveLocation);

    /**
     * Empty destructor. Used just for printing when debuggable.
     */
    ~ImageSaver();

    /**
     * Save the parameter cv::Mat as a JPEG with name "pic_<frameNumber>_<regionId>.jpeg".
     *
     * @param [in] mat the cv::Mat to be saved
     * @param [in] frameNumber the frame number to which this cv::Mat belongs to
     * @param [in] regionId an int capable of uniquelly identifying this image among other saved images,
     * that have the same frameNumber (i.e. have been extracted from the same Frame)
     * @return true if saving was successful, false otherwise
     */
    bool saveImage(const cv::Mat &mat, const int frameNumber, const int regionName);

    /**
     * Save the parameter cv::Mat as a JPEG with name "pic_<frameNumber>_<regionId>.jpeg".
     *
     * @param [in] mat the cv::Mat to be saved
     * @param [in] frameNumber the frame number to which this cv::Mat belongs to
     * @param [in] regionId a std::string capable of uniquelly identifying this image among other saved images,
     * that have the same frameNumber (i.e. have been extracted from the same Frame)
     * @return true if saving was successful, false otherwise
     */
    bool saveImage(const cv::Mat &mat, const int frameNumber, const std::string regionName);

    void setDebuggable(const bool debuggable) override;

    bool isDebuggable() const override;

private:
    const std::string path;
    bool debuggable;
};

} //end namespace rbdt
#endif //RUBIKDETECTOR_IMAGESAVER_HPP

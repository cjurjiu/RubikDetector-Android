//
// Created by catalin on 12.07.2017.
//

#ifndef RUBIKDETECTOR_SIMPLEFACELETSDETECTOR_HPP
#define RUBIKDETECTOR_SIMPLEFACELETSDETECTOR_HPP

#include <vector>
#include <cstdint>
#include <memory>
#include "../../data/processing/RubikFacelet.hpp"
#include "RubikFaceletsDetector.hpp"

namespace cv {
class Mat;
}

namespace rbdt {

class OnCubeDetectionResultListener;

class SimpleFaceletsDetectorImpl;

class ImageSaver;

/**
 * Concrete implementation of a RubikFaceletsDetector.
 *
 * This object requires its RubikFaceletsDetector::onFrameSizeSelected() method to be called before any call to detect() is made. Failing to call onFrameSizeSelected() at least once,
 * or when the size of the input frame changes, will cause undefined behavior.
 *
 * The cv::Mat passed to SimpleFaceletsDetector::detect() needs to be a <b>1 channel grayscale image.</b>
 *
 * Enabling the debug mode on this component makes it print information relevant for debugging. If a non-null ImageSaver is also provided through the
 * appropriate constructor, then various processing artifacts & images are saved for debugging purposes, when debugging is turned on. However, performance is drastically
 * reduced when the images are saved, since writing to disk is typically very slow.
 */
class SimpleFaceletsDetector : public RubikFaceletsDetector {
public:
    /**
     * Default constructor.
     * @return a SimpleFaceletsDetector.
     */
    SimpleFaceletsDetector();

    /**
     * Constructs a detector capable of saving various data & processing artifacts as images to disk, when in debug mode.
     *
     * The ImageSaver provided here will not be used (therefore, will not impact performance) when debugging is off.
     * @param imageSaver an ImageSaver which can be used to print additional image data, for debugging purposes.
     * @return a SimpleFaceletsDetector.
     */
    SimpleFaceletsDetector(std::shared_ptr<ImageSaver> imageSaver);

    /**
     * Virtual destructor. Used just for logging when the memory is freed, if this component is debuggable.
     */
    virtual ~SimpleFaceletsDetector();

    /**
     * Searches for a Rubik's Cube face in the given grayscale cv::Mat. If a cube face is found, then it returns
     * a 3x3 std::vector of RubikFacelet objects, representing the found facelets, without color.
     *
     * If no facelets are found, then nullptr is returned.
     */
    virtual std::vector<std::vector<RubikFacelet>> detect(cv::Mat &frameGray,
                                                          const int frameNumber = 0) override;

    /**
     * @copydoc GenericDetector::onFrameSizeSelected
     */
    void onFrameSizeSelected(int width, int height) override;

    void setDebuggable(const bool debuggable) override;

    bool isDebuggable() const override;

private:
    /**
     * Pointer to private implementation (PIMPL Pattern)
     */
    std::unique_ptr<SimpleFaceletsDetectorImpl> behavior;
};
} //end namespace rbdt
#endif //RUBIKDETECTOR_SIMPLEFACELETSDETECTOR_HPP

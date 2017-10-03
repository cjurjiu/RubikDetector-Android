//
// Created by catalin on 12.07.2017.
//

#ifndef RUBIKDETECTOR_SIMPLEFACELETSDETECTORIMPL_HPP
#define RUBIKDETECTOR_SIMPLEFACELETSDETECTORIMPL_HPP

#include <opencv2/core/core.hpp>
#include "../../../data/geometry/internal/Circle.hpp"
#include "../../../data/processing/internal/HueColorEvidence.hpp"
#include "../../colordetector/HistogramColorDetector.hpp"
#include "../../../imagesaver/ImageSaver.hpp"
#include "../RubikFaceletsDetector.hpp"
#include <iostream>
#include <memory>

namespace rbdt {

class OnCubeDetectionResultListener;

class RubikFacelet;

/**
 * Class to which a SimpleFaceletsDetector delegates its work to.
 *
 * This is effectivelly the <i>private implementation</i> of a SimpleFaceletsDetector's behavior.
 *
 * @note Copied from SimpleFaceletsDetector:
 *
 * @copydoc SimpleFaceletsDetector
 *
 * @note end copy.
 *
 * @warning Do not use SimpleFaceletsDetectorImpl directly, do not expose this in the API. Use the SimpleFaceletsDetector instead.
 *
 * @see SimpleFaceletsDetector
 */
class SimpleFaceletsDetectorImpl : RubikFaceletsDetector {
public:

    /**
     * @note Copied from SimpleFaceletsDetector:
     *
     * @copybrief SimpleFaceletsDetector::SimpleFaceletsDetector()
     *
     * @note end copy.
     *
     * @return SimpleFaceletsDetectorImpl
     */
    SimpleFaceletsDetectorImpl();

    /**
     * @note Copied from SimpleFaceletsDetector:
     *
     * @copybrief SimpleFaceletsDetector::SimpleFaceletsDetector(std::shared_ptr<ImageSaver> imageSaver)
     *
     * The ImageSaver provided here will not be used (therefore, will not impact performance) when debugging is off.
     * @param imageSaver an ImageSaver which can be used to print additional image data, for debugging purposes.
     *
     * @note end copy.
     *
     * @return SimpleFaceletsDetectorImpl
     */
    SimpleFaceletsDetectorImpl(std::shared_ptr<ImageSaver> imageSaver);

    /**
     * @note Copied from SimpleFaceletsDetector:
     *
     * @copybrief SimpleFaceletsDetector::~SimpleFaceletsDetector()
     *
     * @note end copy.
     */
    virtual ~SimpleFaceletsDetectorImpl();

    /**
     * @copydoc SimpleFaceletsDetector::detect()
     */
    std::vector<std::vector<RubikFacelet>> detect(cv::Mat &frameGray,
                                                  const int frameNumber = 0) override;

    /**
     * @copydoc SimpleFaceletsDetector::onFrameSizeSelected()
     */
    void onFrameSizeSelected(int processingWidth, int processingHeight) override;

    void setDebuggable(const bool isDebuggable) override;

    bool isDebuggable() const override;

private:

    /**
     * Applies basic pre-processing on the frame (e.g. blur) and then extracts the contours present in the frame
     * by using the Canny Edge detector implemented in OpenCV.
     *
     * @param [in] frameGray a 1 channel grayscale cv::Mat
     * @return a 2d std::vector containing a contour on each row, & the columns being the points present on the respective contour
     */
    std::vector<std::vector<cv::Point>> detectContours(const cv::Mat &frameGray) const;

    /**
     * Iterates over the contour list and adds the rotated rectangles that passed the filtering step, together with their Circle representation, to their
     * respective output parameter list.
     *
     * The main filtering criteria are the rectangle size and aspect ratio. Rectangles too big or too small are discarded. Rectangles which are not "square"
     * enough, as determined by the ratio between their width & their height, are also discarded.
     *
     * @param [in] contours list of contours to be filtered
     * @param [in/out] possibleFacelets std::vector of rotated rectangles which are good candidates for being cube facelets
     * @param [in/out] possibleFaceletsInnerCircles an array of Circle objects (inner circle internal representation) of the
     * elements in the rotated rectangles array
     * @param [in] frameNumber
     */
    void filterContours(const std::vector<std::vector<cv::Point>> &contours,
                        std::vector<cv::RotatedRect> &possibleFacelets,
                        std::vector<Circle> &possibleFaceletsInnerCircles,
                        const int frameNumber = 0) const;

    /**
     * Heuristic to compute a margin that can be used to estimate the positions of the remaining circles/facelets, given one reference circle.
     *
     * Typically this value is computed by finding the minimum margin between the reference circle and any of the other valid circles, and then by adding
     * a buffer.
     *
     * @param [in] referenceCircle a Circle representing the reference circle
     * @param [in] validCircles a std::vector of Circle object which are potential facelets, given the current reference Circle
     * @return a float representing the computed margin
     */
    float computeMargin(Circle referenceCircle, std::vector<Circle> validCircles);

    /**
     * Further filters the list of Circle elements, s.t. after this filtering only Circles of area & orientation similar to the
     * reference Circle will be considered valid.
     *
     * @param [in] referenceCircle Circle considered as the reference
     * @param [in] innerCircles std::vector of Circle elements that will be filtered
     * @param [in] referenceCircleIndex the index of the reference Circle, in the overall Circle list
     * @return a std::vector which only contains Circles higly similar to the reference circle
     */
    std::vector<Circle> findPotentialFacelets(const Circle &referenceCircle,
                                              const std::vector<Circle> &innerCircles,
                                              int referenceCircleIndex) const;

    /**
     * Given the reference Circle, its orientation, and the value for a margin between circles, this method
     * estimates the position of the remaining 8 circles needed to define a complete Rubik's Cube face.
     *
     * @param [in] referenceCircle a Circle representing the reference object
     * @param [in] margin the margin that will be used to position the remaining 8 circles, relative to the reference one
     * @return a std::vector containing 8 estimated facelets positions, represented as Circle objects, for the
     * reference Circle received as a parameter
     */
    std::vector<Circle> estimateRemainingFaceletsPositions(const Circle &referenceCircle,
                                                           float margin) const;

    /**
     * Given the list of estimated (or generated) facelets, searches for overlaps with Circles which represent real contours
     * that have passed the previous two filtering rounds.
     *
     * If an estimated facelet (circle) has a significant overlap with a circle representing a real contour, then it is added
     * to the result 2D array, at the row & column it represents on the Rubik's Cube face.
     *
     * @param [in] potentialFacelets std::vector of Circle objects representing potential facelets which have passed the previous two filtering rounds
     * @param [in] estimatedFacelets std::vector of Circle objects generated from a reference Circle, and an empirical margin
     * @return a 2D array representing an incomplete model of a Rubik's Cube face. The model is composed from Circles from the <b>potentialFacelets</b>
     * which were matched with estimated Circle objects.
     */
    std::vector<std::vector<Circle>> matchEstimatedWithPotentialFacelets(
            const std::vector<Circle> &potentialFacelets,
            const std::vector<Circle> &estimatedFacelets);

    /**
     * Asserts whether the input 2D array of facelets is complete enough to unambiguously define a Rubik's Cube facet.
     * @param [in] cubeFacet a 2D std::vector representing an incomplete model of a Rubik's Cube facet.
     * @return <b>true</b> if the model is sufficient to define a Rubik's Facet, <b>false</b> otherwise.
     */
    bool verifyIfCubeFound(const std::vector<std::vector<Circle>> &cubeFacet) const;

    /**
     * Fills with estimated facelets the missing facelets in an incomplete Rubik's Cube facet model.
     *
     * @param [in] estimatedFacelets std::vector of Circle objects representing the estimated facelets
     * @param [in/out] facetModel 2d std::vector representing an incomplete Rubik's Cube facet mode, to be
     * completed by the estimated facelets
     */
    void fillMissingFacelets(const std::vector<Circle> &estimatedFacelets,
                             std::vector<std::vector<Circle>> &facetModel);

    /**
     * Creates a Rubik's Cube facet model made of RubikFacelet objects, from a model composed from Circle objects.
     * @param [in] model complete Rubik's Cube facet model, composed from Circle objects
     * @return an equivalent Rubik's Cube facet model, made of RubikFacelet objects
     */
    std::vector<std::vector<RubikFacelet>> createResult(
            const std::vector<std::vector<Circle>> &model);

    void saveWholeFrame(const cv::Mat &currentFrame, int frameNr) const;

    cv::Mat saveFilteredRectangles(const cv::Mat &currentFrame,
                                   const std::vector<cv::RotatedRect> &filteredRectangles,
                                   int frameNr) const;

    void saveDebugData(const cv::Mat &frame,
                       const std::vector<cv::RotatedRect> &filteredRectangles,
                       const Circle &referenceCircle,
                       const std::vector<Circle> &potentialFacelets,
                       const std::vector<Circle> &estimatedFacelets,
                       const std::vector<std::vector<RubikFacelet::Color>> colors,
                       const int frameNumber);

    void drawRectangleToMat(const cv::Mat &currentFrame, const cv::RotatedRect &rotatedRect,
                            const cv::Scalar color = cv::Scalar(0, 255, 0)) const;

    static constexpr float MIN_VALID_SHAPE_TO_IMAGE_AREA_RATIO = 0.0025f;

    static constexpr float MIN_VALID_SHAPE_TO_IMAGE_SIDE_SIZE_RATIO = 0.25f;

    static constexpr int CIRCLE_DISTANCE_BUFFER = 2 * 10;

    static constexpr int BLUR_KERNEL_SIZE = 5;

    static constexpr int CANNY_LOW_THRESHOLD = 80;

    static constexpr int CANNY_THRESHOLD_RATIO = 3;

    static constexpr int CANNY_APERTURE_SIZE = 5;

    static constexpr int MIN_POTENTIAL_FACELETS_REQUIRED = 4;

    std::shared_ptr<ImageSaver> imageSaver;

    bool debuggable = false;

    int maxShapeSideSize;

    int minValidShapeArea;

};

} //namespace rbdt
#endif //RUBIKDETECTOR_SIMPLEFACELETSDETECTORIMPL_HPP
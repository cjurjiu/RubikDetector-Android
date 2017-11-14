//
// Created by catalin on 26.07.2017.
//

#ifndef RUBIKDETECTOR_UTILS_HPP
#define RUBIKDETECTOR_UTILS_HPP

#include "../data/geometry/internal/Circle.hpp"
#include "../data/processing/RubikFacelet.hpp"
#include "../detectors/faceletsdetector/RubikFaceletsDetector.hpp"
#include "../data/config/DrawConfig.hpp"

namespace cv {
class Mat;
}

namespace rbdt {

/**
 * Computes the distance between two points, in pixels.
 * @param [in] firstPoint
 * @param [in] secondPoint
 * @return the distance between the points, in pixels.
 */
float pointsDistance(const cv::Point2f &firstPoint, const cv::Point2f &secondPoint);

/**
 * Save the image data stored in the RGBA cv::Mat parameter, as a JPEG at the specified path.
 *
 * This method will perform a copy+converstion of the RGBA Mat, as it needs to convert it to BGR, before saving it to disk.
 *
 * The saved image will have the following name:
 * <pre>
 * pic_<frameNumber>_<regionId>.jpg
 * </pre>
 *
 * @param [in] mat the RGBA image data to save.
 * @param [in] path the persistent storage location where the image will be saved.
 * @param [in] frameNumber the frame number of the currently saved image, to be appended to the file name.
 * @param [in] regionId a number used to uniquelly identify this image among others saved in the same frame. will be appended to the file name.
 * @return true if saving was successful, false otherwise.
 */
bool quickSaveRGBAImage(const cv::Mat &mat, const std::string path, const int frameNumber,
                            const int regionId);

/**
 * Save the image data stored in the BGR cv::Mat parameter, as a JPEG at the specified path.
 *
 * The saved image will have the following name:
 * <pre>
 * pic_<frameNumber>_<regionId>.jpg
 * </pre>
 *
 * @param [in] mat the BGR image data to save.
 * @param [in] path the persistent storage location where the image will be saved.
 * @param [in] frameNumber the frame number of the currently saved image, to be appended to the file name.
 * @param [in] regionId a number used to uniquelly identify this image among others saved in the same frame. will be appended to the file name.
 * @return true if saving was successful, false otherwise.
 */
bool quickSaveBGRImage(const cv::Mat &mat, const std::string path, const int frameNumber,
                    const int regionId);

/**
 * Returns an intuitive char given a RubikFacelet::Color. Useful when printing.
 *
 * The matching between returned chars & RubiKFacelet::Color is the following:
 *
 *   - RubikFacelet::Color::RED returns: 'r';
 *   - RubikFacelet::Color::ORANGE returns: 'o';
 *   - RubikFacelet::Color::YELLOW returns: 'y';
 *   - RubikFacelet::Color::GREEN returns: 'g';
 *   - RubikFacelet::Color::BLUE returns: 'b';
 *   - RubikFacelet::Color::WHITE returns: 'w';
 *
 * @param [in] colorInt
 * @return a char representation of the RubikFacelet::Color
 */
char colorIntToChar(const RubikFacelet::Color colorInt);

/**
 * Given the RubikFacelet::Color, return a cv::Scalar which can be used with the OpenCV drawing API.
 *
 * @param [in] color RubikFacelet::Color to be retrieved as a cv::Scalar
 * @return a cv::Scalar representing the color
 */
cv::Scalar getColorAsScalar(const RubikFacelet::Color color);

/**
 *
 * @param drawingCanvas
 * @param circle
 * @param color
 * @param scalingFactor
 * @param radiusModifier
 * @param fillArea
 */
void drawCircle(cv::Mat &drawingCanvas, const Circle circle, const cv::Scalar color,
                const float scalingFactor = 1,
                const int radiusModifier = 0,
                const bool fillArea = false);

/**
 *
 * @param drawingCanvas
 * @param circles
 * @param color
 * @param scalingFactor
 */
void drawCircles(cv::Mat &drawingCanvas, const std::vector<Circle> circles,
                 const cv::Scalar color,
                 const float scalingFactor = 1);

/**
 *
 * @param drawingCanvas
 * @param circles
 * @param color
 * @param scalingFactor
 * @param radiusModifier
 * @param fillArea
 */
void drawCircles(cv::Mat &drawingCanvas, const std::vector<std::vector<Circle>> circles,
                 const cv::Scalar color,
                 const float scalingFactor = 1,
                 const int radiusModifier = 0,
                 const bool fillArea = false);

/**
 * Computes the current time in miliseconds, since epoch.
 * @return double
 */
double getCurrentTimeMillis();

/**
 * Encodes an ARGB frame to NV21.
 * @param inputArgb
 * @param outputNv21
 * @param width
 * @param height
 */
void encodeNV21(const cv::Mat &inputArgb, const cv::Mat &outputNv21, int width, int height);

/**
 * Encodes an ARGB frame to NV12.
 * @param inputArgb
 * @param outputNv12
 * @param width
 * @param height
 */
void encodeNV12(const cv::Mat &inputArgb, const cv::Mat &outputNv12, int width, int height);

/**
 *
 * @param value
 * @return
 */
template<typename Enumeration>
auto asInt(const Enumeration value)
-> typename std::underlying_type<Enumeration>::type {
    return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}
} //end namespace rbdt
#endif //RUBIKDETECTOR_UTILS_HPP

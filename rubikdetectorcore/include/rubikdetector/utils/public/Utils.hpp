//
// Created by catalin on 26.07.2017.
//

#ifndef RUBIKDETECTOR_UTILS_HPP
#define RUBIKDETECTOR_UTILS_HPP

#include "../../data/geometry/Circle.hpp"
#include "../../data/processing/public/RubikFacelet.hpp"
#include "../../detectors/faceletsdetector/public/FaceletsDetector.hpp"
#include "../../data/config/public/DrawConfig.hpp"

namespace cv {
    class Mat;
}

namespace utils {
    float pointsDistance(const cv::Point2f &firstPoint, const cv::Point2f &secondPoint);

    bool quickSaveImage(const cv::Mat &mat, const std::string path, const int frameNumber,
                        const int regionId);

    char colorIntToChar(const RubikFacelet::Color colorInt);

    cv::Scalar getColorAsScalar(const RubikFacelet::Color color);

    void drawCircle(cv::Mat &drawingCanvas, const Circle circle, const cv::Scalar color,
                    const float scalingFactor = 1,
                    const int radiusModifier = 0,
                    const bool fillArea = false);

    void drawCircles(cv::Mat &drawingCanvas, const std::vector<Circle> circles,
                     const cv::Scalar color,
                     const float scalingFactor = 1);

    void drawCircles(cv::Mat &drawingCanvas, const std::vector<std::vector<Circle>> circles,
                     const cv::Scalar color,
                     const float scalingFactor = 1,
                     const int radiusModifier = 0,
                     const bool fillArea = false);

    double getCurrentTimeMillis();

    void encodeNV21(const cv::Mat &inputArgb, const cv::Mat &outputNv21, int width, int height);

    void encodeNV12(const cv::Mat &inputArgb, const cv::Mat &outputNv12, int width, int height);

    template<typename Enumeration>
    auto asInt(const Enumeration value)
    -> typename std::underlying_type<Enumeration>::type {
        return static_cast<typename std::underlying_type<Enumeration>::type>(value);
    }

    RubikFacelet::Color colorFromInt(const int val);

    ImageProcessor::ImageFormat imageFormatFromInt(const int val);

    DrawConfig::DrawMode drawModeFromInt(const int val);

}
#endif //RUBIKDETECTOR_UTILS_HPP

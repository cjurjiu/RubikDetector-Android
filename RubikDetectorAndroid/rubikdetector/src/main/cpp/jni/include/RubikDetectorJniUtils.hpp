//
// Created by catalin on 10.09.2017.
//

#ifndef RUBIKDETECTORANDROID_RUBIKDETECTORJNIUTILS_HPP
#define RUBIKDETECTORANDROID_RUBIKDETECTORJNIUTILS_HPP

#include "../../rubikdetectorcore/include/rubikdetector/data/processing/RubikFacelet.hpp"
#include "../../rubikdetectorcore/include/rubikdetector/data/config/DrawConfig.hpp"
#include "../../rubikdetectorcore/include/rubikdetector/rubikprocessor/RubikProcessor.hpp"

namespace rbdt_jni {

/**
 *
 */
static constexpr int JNI_COLOR_RED = 0;

/**
 *
 */
static constexpr int JNI_COLOR_ORANGE = 1;

/**
 *
 */
static constexpr int JNI_COLOR_YELLOW = 2;

/**
 *
 */
static constexpr int JNI_COLOR_GREEN = 3;

/**
 *
 */
static constexpr int JNI_COLOR_BLUE = 4;

/**
 *
 */
static constexpr int JNI_COLOR_WHITE = 5;

/**
 *
 */
static constexpr int JNI_IMAGE_FORMAT_YUV_NV21 = 0;

/**
 *
 */
static constexpr int JNI_IMAGE_FORMAT_YUV_NV12 = 1;

/**
 *
 */
static constexpr int JNI_IMAGE_FORMAT_YUV_I420 = 2;

/**
 *
 */
static constexpr int JNI_IMAGE_FORMAT_YUV_YV12 = 3;

/**
 *
 */
static constexpr int JNI_IMAGE_FORMAT_RGBA8888 = 4;

/**
 *
 */
static constexpr int JNI_DRAW_MODE_DO_NOT_DRAW = 0;

/**
 *
 */
static constexpr int JNI_DRAW_MODE_DRAW_RECTANGLES = 1;

/**
 *
 */
static constexpr int JNI_DRAW_MODE_DRAW_CIRCLES = 2;

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

/**
 *
 * @param val
 * @return
 */
rbdt::RubikFacelet::Color colorFromInt(const int val);

/**
 *
 * @param val
 * @return
 */
rbdt::RubikProcessor::ImageFormat imageFormatFromInt(const int val);

/**
 *
 * @param val
 * @return
 */
rbdt::DrawConfig::DrawMode drawModeFromInt(const int val);

} //end namespace rbdt

#endif //RUBIKDETECTORANDROID_RUBIKDETECTORJNIUTILS_HPP

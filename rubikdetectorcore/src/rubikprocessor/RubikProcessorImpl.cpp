//// Created by catalin on 12.07.2017.


#include <math.h>
#include "../../include/rubikdetector/rubikprocessor/internal/RubikProcessorImpl.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "../../include/rubikdetector/utils/Utils.hpp"
#include "../../include/rubikdetector/utils/CrossLog.hpp"
#include "../../include/rubikdetector/data/config/ImageProperties.hpp"

namespace rbdt {

/**##### PUBLIC API #####**/
    RubikProcessorImpl::RubikProcessorImpl(const ImageProperties imageProperties,
                                           std::unique_ptr<RubikFaceletsDetector> faceletsDetector,
                                           std::unique_ptr<RubikColorDetector> colorDetector,
                                           std::unique_ptr<FaceletsDrawController> faceletsDrawController,
                                           std::shared_ptr<ImageSaver> imageSaver) :
            faceletsDetector(std::move(faceletsDetector)),
            colorDetector(std::move(colorDetector)),
            faceletsDrawController(std::move(faceletsDrawController)),
            imageSaver(imageSaver) {
        applyImageProperties(imageProperties);
    }

    RubikProcessorImpl::~RubikProcessorImpl() {
        if (debuggable) {
            LOG_DEBUG("NativeRubikProcessor", "RubikProcessorBehavior - destructor.");
        }
    }

    std::vector<std::vector<RubikFacelet>> RubikProcessorImpl::process(
            const uint8_t *imageData) {
        return findCubeInternal(imageData);
    }

    void RubikProcessorImpl::updateImageProperties(const ImageProperties &newProperties) {
        applyImageProperties(newProperties);
    }

    void RubikProcessorImpl::overrideInputFrameWithOutputFrame(const uint8_t *imageData) {
        if (inputImageFormat == RubikProcessor::ImageFormat::RGBA8888) {
            //input frame is equal to output frame already, this would have no effect.
            return;
        }
        cv::Mat yuvFrame = cv::Mat(imageHeight + imageHeight / 2, imageWidth, CV_8UC1,
                                   (uchar *) imageData + inputImageOffset);
        cv::Mat rgbaFrame = cv::Mat(imageHeight, imageWidth, CV_8UC4,
                                    (uchar *) imageData + outputRgbaImageOffset);
        int reverseColorConversionCode;
        switch (inputImageFormat) {
            case RubikProcessor::ImageFormat::YUV_I420:
                reverseColorConversionCode = cv::COLOR_RGBA2YUV_I420;
                break;
            case RubikProcessor::ImageFormat::YUV_YV12:
                reverseColorConversionCode = cv::COLOR_RGBA2YUV_YV12;
                break;
            default:
                reverseColorConversionCode = NO_CONVERSION_NEEDED;
        }

        if (reverseColorConversionCode != NO_CONVERSION_NEEDED) {
            cv::cvtColor(rgbaFrame, yuvFrame, reverseColorConversionCode);
        } else {
            if (inputImageFormat == RubikProcessor::ImageFormat::YUV_NV21) {
                rbdt::encodeNV21(rgbaFrame, yuvFrame, imageWidth, imageHeight);
            } else if (inputImageFormat == RubikProcessor::ImageFormat::YUV_NV12) {
                rbdt::encodeNV12(rgbaFrame, yuvFrame, imageWidth, imageHeight);
            }
        }
    }

    void RubikProcessorImpl::setDebuggable(const bool isDebuggable) {
        if (debuggable) {
            LOG_DEBUG("NativeRubikProcessor", "setDebuggable. current:%d, new: %d, frameNumber: %d",
                      debuggable, isDebuggable, frameNumber);
        }
        debuggable = isDebuggable;
        colorDetector->setDebuggable(isDebuggable);
        if (imageSaver != nullptr) {
            imageSaver->setDebuggable(isDebuggable);
        }
        faceletsDetector->setDebuggable(isDebuggable);
        faceletsDrawController->setDebuggable(isDebuggable);
    }

    bool RubikProcessorImpl::isDebuggable() const {
        return debuggable;
    }

    int RubikProcessorImpl::getRequiredMemory() {
        return totalRequiredMemory;
    }

    int RubikProcessorImpl::getOutputFrameBufferOffset() {
        return outputRgbaImageOffset;
    }

    int RubikProcessorImpl::getOutputFrameByteCount() {
        return outputRgbaImageByteCount;
    }

    int RubikProcessorImpl::getInputFrameByteCount() {
        return inputImageByteCount;
    }

    int RubikProcessorImpl::getInputFrameBufferOffset() {
        return inputImageOffset;
    }

    void RubikProcessorImpl::updateDrawConfig(DrawConfig drawConfig) {
        faceletsDrawController->updateDrawConfig(drawConfig);
    }
/**##### END PUBLIC API #####**/
/**##### PRIVATE MEMBERS FROM HERE #####**/

    void RubikProcessorImpl::applyImageProperties(const ImageProperties &properties) {
        RubikProcessorImpl::inputImageFormat = properties.inputImageFormat;
        switch (inputImageFormat) {
            case RubikProcessor::ImageFormat::YUV_NV21:
                cvColorConversionCode = cv::COLOR_YUV2RGBA_NV21;
                break;
            case RubikProcessor::ImageFormat::YUV_NV12:
                cvColorConversionCode = cv::COLOR_YUV2RGBA_NV12;
                break;
            case RubikProcessor::ImageFormat::YUV_I420:
                cvColorConversionCode = cv::COLOR_YUV2RGBA_I420;
                break;
            case RubikProcessor::ImageFormat::YUV_YV12:
                cvColorConversionCode = cv::COLOR_YUV2RGBA_YV12;
                break;
            case RubikProcessor::ImageFormat::RGBA8888:
                cvColorConversionCode = RubikProcessorImpl::NO_CONVERSION_NEEDED;
                break;
        }
        imageWidth = properties.width;
        imageHeight = properties.height;
        largestDimension = imageWidth > imageHeight ? imageWidth : imageHeight;
        if (largestDimension > DEFAULT_DIMENSION) {
            upscalingRatio = (float) largestDimension / DEFAULT_DIMENSION;
            downscalingRatio = (float) DEFAULT_DIMENSION / largestDimension;
            if (largestDimension == imageHeight) {
                processingWidth = (int) round(imageHeight * downscalingRatio *
                                              ((float) imageWidth / imageHeight));
                processingHeight = (int) round(imageHeight * downscalingRatio);
            } else {
                processingWidth = (int) round(imageWidth * downscalingRatio);
                processingHeight = (int) round(imageWidth * downscalingRatio *
                                               ((float) imageHeight / imageWidth));
            }
        } else if (largestDimension < DEFAULT_DIMENSION) {
            upscalingRatio = 1.0f;
            downscalingRatio = 1.0f;
            processingHeight = largestDimension == imageHeight ? 320 : 240;
            processingWidth = largestDimension == imageWidth ? 320 : 240;
        } else {
            upscalingRatio = 1.0f;
            downscalingRatio = 1.0f;
            processingHeight = imageHeight;
            processingWidth = imageWidth;
        }
        needsResize = imageHeight != processingHeight || imageWidth != processingWidth;

        if (inputImageFormat != RubikProcessor::ImageFormat::RGBA8888) {
            //image is in one of the supported YUV formats
            outputRgbaImageByteCount = imageWidth * imageHeight * 4;
            outputRgbaImageOffset = imageWidth * (imageHeight + imageHeight / 2);
            inputImageByteCount = imageWidth * (imageHeight + imageHeight / 2);
            inputImageOffset = RubikProcessorImpl::NO_OFFSET;

            if (needsResize) {
                processingRgbaImageOffset = outputRgbaImageByteCount + inputImageByteCount;
                processingRgbaImageByteCount = processingHeight * processingWidth * 4;
                processingGrayImageOffset =
                        outputRgbaImageByteCount + inputImageByteCount +
                        processingRgbaImageByteCount;
                processingGrayImageSize = processingHeight * processingWidth;

                totalRequiredMemory = outputRgbaImageByteCount + inputImageByteCount +
                                      processingGrayImageSize + processingRgbaImageByteCount;
            } else {
                totalRequiredMemory = outputRgbaImageByteCount + inputImageByteCount;
            }
        } else {
            inputImageByteCount = outputRgbaImageByteCount = imageWidth * imageHeight * 4;
            inputImageOffset = outputRgbaImageOffset = RubikProcessorImpl::NO_OFFSET;

            if (needsResize) {
                processingRgbaImageOffset = outputRgbaImageByteCount;
                processingRgbaImageByteCount = processingHeight * processingWidth * 4;
                processingGrayImageOffset = outputRgbaImageByteCount + processingRgbaImageByteCount;
                processingGrayImageSize = processingHeight * processingWidth;

                totalRequiredMemory =
                        outputRgbaImageByteCount + processingRgbaImageByteCount +
                        processingGrayImageSize;
            } else {
                processingGrayImageSize = processingHeight * processingWidth;
                processingGrayImageOffset = outputRgbaImageByteCount;
                totalRequiredMemory = outputRgbaImageByteCount + processingGrayImageSize;
            }
        }
        faceletsDetector->onFrameSizeSelected(processingWidth, processingHeight);
    }

    std::vector<std::vector<RubikFacelet>> RubikProcessorImpl::findCubeInternal(
            const uint8_t *imageData) {
        double processingStart = 0;

        if (debuggable) {
//            only calculate frame rate if the processor is debuggable
            frameNumber++;
            processingStart = rbdt::getCurrentTimeMillis();
        }

        cv::Mat outputFrameRgba(imageHeight, imageWidth, CV_8UC4,
                                (uchar *) imageData + outputRgbaImageOffset);

        cv::Mat processingFrameRgba;
        cv::Mat processingFrameGrey;

        if (inputImageFormat != RubikProcessor::ImageFormat::RGBA8888) {
            //YUV image, need to convert the output & processing frames to RGBA8888

            if (debuggable) {
                LOG_DEBUG("NativeRubikProcessor", "Frame not RGB, needs conversion to RGBA.");
            }

            cv::Mat frameYuv(imageHeight + imageHeight / 2, imageWidth, CV_8UC1,
                             (uchar *) imageData);

            cv::cvtColor(frameYuv, outputFrameRgba, cvColorConversionCode);

            if (needsResize) {
                if (debuggable) {
                    LOG_DEBUG("NativeRubikProcessor", "Resizing frame to processing size.");
                }
                processingFrameRgba = cv::Mat(processingHeight, processingWidth, CV_8UC4,
                                              (uchar *) imageData + processingRgbaImageOffset);
                processingFrameGrey = cv::Mat(processingHeight, processingWidth, CV_8UC1,
                                              (uchar *) imageData + processingGrayImageOffset);
                cv::resize(outputFrameRgba, processingFrameRgba,
                           cv::Size(processingWidth, processingHeight));
                cv::cvtColor(processingFrameRgba, processingFrameGrey, CV_RGBA2GRAY);
            } else {
                if (debuggable) {
                    LOG_DEBUG("NativeRubikProcessor", "Frame already at processing size, no resize needed.");
                }
                processingFrameRgba = cv::Mat(outputFrameRgba);
                processingFrameGrey = frameYuv(cv::Rect(0, 0, imageWidth, imageHeight));
            }

        } else {
            //input already in RGBA8888 format
            processingFrameGrey = cv::Mat(processingHeight, processingWidth, CV_8UC1,
                                          (uchar *) imageData + processingGrayImageOffset);
            if (debuggable) {
                LOG_DEBUG("NativeRubikProcessor", "###################ALREADY RGBA");
            }

            if (needsResize) {
                if (debuggable) {
                    LOG_DEBUG("NativeRubikProcessor", "Needs resize.");
                }
                processingFrameRgba = cv::Mat(processingHeight, processingWidth, CV_8UC4,
                                              (uchar *) imageData + processingRgbaImageOffset);
                cv::resize(outputFrameRgba, processingFrameRgba,
                           cv::Size(processingWidth, processingHeight));
            } else {
                if (debuggable) {
                    LOG_DEBUG("NativeRubikProcessor", "No resize.");
                }
                processingFrameRgba = cv::Mat(outputFrameRgba);
            }
            cv::cvtColor(processingFrameRgba, processingFrameGrey, CV_RGBA2GRAY);
        }
        if (isDebuggable()) {
            LOG_DEBUG("NativeRubikProcessor", "RubikProcessor - Searching for facelets.");
        }
        std::vector<std::vector<RubikFacelet>> facelets = faceletsDetector->detect(
                processingFrameGrey,
                frameNumber);
        if (facelets.size() != 0) {
            if (isDebuggable()) {
                LOG_DEBUG("NativeRubikProcessor", "RubikProcessor - Found facelets!");
            }
            std::vector<std::vector<RubikFacelet::Color>> colors = detectFacetColors(
                    processingFrameRgba,
                    facelets);
            if (isDebuggable()) {
                LOG_DEBUG("NativeRubikProcessor",
                          "RubikProcessor - Detected colors: {%c, %c, %c} {%c, %c, %c} {%c, %c, %c}\nApplying colors to facelets.",
                          rbdt::colorIntToChar(colors[0][0]),
                          rbdt::colorIntToChar(colors[0][1]),
                          rbdt::colorIntToChar(colors[0][2]),
                          rbdt::colorIntToChar(colors[1][0]),
                          rbdt::colorIntToChar(colors[1][1]),
                          rbdt::colorIntToChar(colors[1][2]),
                          rbdt::colorIntToChar(colors[2][0]),
                          rbdt::colorIntToChar(colors[2][1]),
                          rbdt::colorIntToChar(colors[2][2])
                );
            }
            applyColorsToResult(facelets, colors);

            if (needsResize) {
                if (isDebuggable()) {
                    LOG_DEBUG("NativeRubikProcessor", "RubikProcessor - Rescaling result.");
                }
                upscaleResult(facelets);
            } else {
                if (isDebuggable()) {
                    LOG_DEBUG("NativeRubikProcessor", "RubikProcessor - No rescaling needed.");
                }
            }
            if (isDebuggable()) {
                LOG_DEBUG("NativeRubikProcessor",
                          "RubikProcessor - Drawing the result to the output frame.");
            }
            faceletsDrawController->drawResultToMat(outputFrameRgba, facelets);
        } else {
            if (isDebuggable()) {
                LOG_DEBUG("NativeRubikProcessor", "RubikProcessor - No cube found.");
            }
        }

        //BASICALLY DONE
        if (debuggable) {

            double processingEnd = rbdt::getCurrentTimeMillis();
            double delta = processingEnd - processingStart;
            double fps = 1000 / delta;
            frameRateSum += fps;
            float frameRateAverage = (float) frameRateSum / frameNumber;

            LOG_DEBUG("NativeRubikProcessor",
                      "Done processing in this frame.\nframeNumber: %d,\nframeRate current frame: %.2f,\nframeRateAverage: %.2f,\nframeRate: %.2f. \nRubikProcessor - Returning the found facelets",
                      frameNumber, fps, frameRateAverage, fps);
        }

        return facelets;
    }

    std::vector<std::vector<RubikFacelet::Color>> RubikProcessorImpl::detectFacetColors(
            const cv::Mat &currentFrame,
            const std::vector<std::vector<RubikFacelet>> facetModel) {
        std::vector<std::vector<RubikFacelet::Color>> colors(3,
                                                             std::vector<RubikFacelet::Color>(3));
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                RubikFacelet faceletRect = facetModel[i][j];
                float innerCircleRadius = faceletRect.innerCircleRadius();
                if (0 <= (faceletRect.center.x - innerCircleRadius) &&
                    0 <= (2 * innerCircleRadius) &&
                    (faceletRect.center.x + innerCircleRadius) <= currentFrame.cols &&
                    0 <= (faceletRect.center.y - innerCircleRadius) &&
                    (faceletRect.center.y + innerCircleRadius) <= currentFrame.rows) {
                    cv::Rect roi = cv::Rect(
                            cv::Point2f(faceletRect.center.x - innerCircleRadius,
                                        faceletRect.center.y - innerCircleRadius),
                            cv::Point2f(faceletRect.center.x + innerCircleRadius,
                                        faceletRect.center.y + innerCircleRadius)
                    );

                    cv::Mat stickerRoiHSV;
                    // Convert the image to HSV
                    cv::cvtColor(currentFrame(roi), stickerRoiHSV, CV_RGB2HSV);

                    float whiteMinRatio;
                    if (i == 1 && j == 1) {
                        whiteMinRatio = 0.44f;
                    } else {
                        whiteMinRatio = 0.5f;
                    }
                    colors[i][j] = colorDetector->detectColor(stickerRoiHSV,
                                                              whiteMinRatio,
                                                              i * 10 + j,
                                                              frameNumber);
                } else {
                    colors[i][j] = RubikFacelet::Color::WHITE;
                    if (debuggable) {
                        LOG_DEBUG("NativeRubikProcessor",
                                  "frameNumberOld: %d FOUND INVALID RECT WHEN DETECTING COLORS",
                                  frameNumber);
                    }
                }
            }
        }
        return colors;
    }

    void RubikProcessorImpl::applyColorsToResult(std::vector<std::vector<RubikFacelet>> &facelets,
                                                 const std::vector<std::vector<RubikFacelet::Color>> colors) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                facelets[i][j].color = colors[i][j];
            }
        }
    }

    void RubikProcessorImpl::upscaleResult(std::vector<std::vector<RubikFacelet>> &facelets) {

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                facelets[i][j].center.x *= upscalingRatio;
                facelets[i][j].center.y *= upscalingRatio;
                facelets[i][j].width *= upscalingRatio;
                facelets[i][j].height *= upscalingRatio;
            }
        }
    }

} //namespace rbdt
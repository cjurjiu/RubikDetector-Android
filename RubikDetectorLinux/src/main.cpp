#include <iostream>
#include <memory>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "rubikdetectorcore/include/rubikdetector/imagesaver/ImageSaver.hpp"
#include "rubikdetectorcore/include/rubikdetector/utils/Utils.hpp"
#include "rubikdetectorcore/include/rubikdetector/utils/CrossLog.hpp"
#include "rubikdetectorcore/include/rubikdetector/rubikprocessor/RubikProcessor.hpp"
#include "rubikdetectorcore/include/rubikdetector/rubikprocessor/builder/RubikProcessorBuilder.hpp"

void performProcessingOnVideo();

void imageNV21Test();

int main() {
    std::cout << "Starting video processing." << std::endl;
    performProcessingOnVideo();
    std::cout << "Finished video processing." << std::endl;
    return 0;
}

void performProcessingOnVideo() {
    cv::VideoCapture cap;
    cap.open(
            "../videos/320x240.mp4");
    if (!cap.isOpened()) {
        std::cout << "Caption not opened, return." << std::endl;
        return;
    }

    std::shared_ptr<rbdt::ImageSaver> imageSaver = std::make_shared<rbdt::ImageSaver>(
            rbdt::ImageSaver(std::string("../debug_images")));
    imageSaver->setDebuggable(true);
    rbdt::RubikProcessor &rubikDetector = *rbdt::RubikProcessorBuilder()
            .inputFrameSize(320, 240)
            .inputFrameFormat(rbdt::RubikProcessor::ImageFormat::RGBA8888)
            .drawConfig(rbdt::DrawConfig::Circles(4))
            .imageSaver(imageSaver)
            .debuggable(true)
            .build();


    uchar largeBuffer[rubikDetector.getRequiredMemory()];

    cv::Mat frame;
    while (cap.read(frame)) {

        cv::cvtColor(frame, frame, CV_BGR2RGBA);

        const uchar *nv21Ptr = frame.ptr();
        for (int i = 0; i < rubikDetector.getInputFrameByteCount(); i++) {
            largeBuffer[i] = *nv21Ptr++;
        }

        std::vector<std::vector<rbdt::RubikFacelet>> result = rubikDetector.process(largeBuffer);
        if (result.size() != 0) {
            LOG_DEBUG("RUBIK_JNI_PART.cpp",
                      "COLORS: [1]:{ %c %c %c } [2]:{ %c %c %c } [3]:{ %c %c %c }",
                      rbdt::colorIntToChar(result[0][0].color),
                      rbdt::colorIntToChar(result[0][1].color),
                      rbdt::colorIntToChar(result[0][2].color),
                      rbdt::colorIntToChar(result[1][0].color),
                      rbdt::colorIntToChar(result[1][1].color),
                      rbdt::colorIntToChar(result[1][2].color),
                      rbdt::colorIntToChar(result[2][0].color),
                      rbdt::colorIntToChar(result[2][1].color),
                      rbdt::colorIntToChar(result[2][2].color));
        }
    }
    std::cout << "Finished processing." << std::endl;
}
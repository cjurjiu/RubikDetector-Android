#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "rubikdetectorcore/detectors/cubedetector/CubeDetector.hpp"
#include "rubikdetectorcore/detectors/cubedetector/OnCubeDetectionResultListener.hpp"
#include "rubikdetectorcore/utils/Utils.hpp"
#include "rubikdetectorcore/utils/CrossLog.hpp"
#include "rubikdetectorcore/helpers/ImageSaver.hpp"

void performProcessingOnVideo();

void imageNV21Test();

class ResultListener : public OnCubeDetectionResultListener {
public:

    ResultListener() {};

    ~ResultListener() {}

    void onCubeDetectionResult(const std::vector<std::vector<int>> result) const {
        LOG_DEBUG("RUBIK_JNI_PART.cpp",
                  "COLORS: [1]:{ %c %c %c } [2]:{ %c %c %c } [3]:{ %c %c %c }",
                  utils::colorIntToChar(result[0][0]),
                  utils::colorIntToChar(result[0][1]),
                  utils::colorIntToChar(result[0][2]),
                  utils::colorIntToChar(result[1][0]),
                  utils::colorIntToChar(result[1][1]),
                  utils::colorIntToChar(result[1][2]),
                  utils::colorIntToChar(result[2][0]),
                  utils::colorIntToChar(result[2][1]),
                  utils::colorIntToChar(result[2][2]));
    }
};

int main() {
    std::cout << "Starting video processing." << std::endl;
    performProcessingOnVideo();
//    imageNV21Test();
    std::cout << "Finished video processing." << std::endl;
    return 0;
}

void performProcessingOnVideo() {
    cv::VideoCapture cap;
    cap.open(
            "../videos/720x480.mp4");
    if (!cap.isOpened()) {
        std::cout << "Caption not opened, return." << std::endl;
        return;
    }
    cv::Mat frame;

    ResultListener resultListener;
    std::shared_ptr<ImageSaver> imageSaver = std::make_shared<ImageSaver>(
            ImageSaver(std::string(
                    "../debug_images")));
    CubeDetector rubikDetector(imageSaver);
    rubikDetector.setImageDimensions(720, 480);
    rubikDetector.setDebuggable(true);
    rubikDetector.setOnCubeDetectionResultListener(&resultListener);

    while (cap.read(frame)) {
        cv::cvtColor(frame, frame, CV_BGR2RGBA);
        rubikDetector.findCube(frame);
//        cv::namedWindow("Contours", CV_WINDOW_AUTOSIZE);
//        cv::imshow("Contours", frame);
//        if (cv::waitKey(30) >= 0) {
//            std::cout << "Got to end of video, break." << std::endl;
//            break;
//        }
    }
    std::cout << "Finished processing." << std::endl;
}

void performProcessingOnVideoSmart() {
    cv::VideoCapture cap;
    cap.open(
            "../videos/720x480.mp4");
    if (!cap.isOpened()) {
        std::cout << "Caption not opened, return." << std::endl;
        return;
    }
    cv::Mat frame;

    ResultListener resultListener;
    std::shared_ptr<ImageSaver> imageSaver = std::make_shared<ImageSaver>(
            ImageSaver(std::string(
                    "../debug_images")));
    CubeDetector rubikDetector(imageSaver);
    rubikDetector.setImageDimensions(720, 480);
    rubikDetector.setDebuggable(true);
    rubikDetector.setOnCubeDetectionResultListener(&resultListener);

    uchar largeBuffer[rubikDetector.getTotalRequiredMemory()];

    while (cap.read(frame)) {

        cv::cvtColor(frame, frame, CV_BGR2YUV_I420);

        const uchar *nv21Ptr = frame.ptr();
        for (int i = 0; i < rubikDetector.getNv21ImageSize(); i++) {
            largeBuffer[i] = *nv21Ptr++;
        }

        rubikDetector.findCube(largeBuffer, rubikDetector.getTotalRequiredMemory());
//        cv::namedWindow("Contours", CV_WINDOW_AUTOSIZE);
//        cv::imshow("Contours", frame);
//        if (cv::waitKey(30) >= 0) {
//            std::cout << "Got to end of video, break." << std::endl;
//            break;
//        }
    }
    std::cout << "Finished processing." << std::endl;
}

void imageNV21Test() {
    cv::Mat ssOrg = cv::imread(
            "../debug_images/pic_9_full_frame.jpg");

    cv::Mat ssGray;
    cv::cvtColor(ssOrg, ssOrg, CV_BGR2RGBA);
    cv::cvtColor(ssOrg, ssGray, CV_RGBA2YUV_I420);

    LOG_DEBUG("RubikMemoryInfo",
              "Right after cvtColor: chanels: %d,\n"
                      "width: %d,\n"
                      "height: %d,\n"
                      "depth: %d,\n"
                      "dataStart: %p,\n"
                      "dataEnd: %p,\n"
                      "dataLimit: %p,\n"
                      "sizeWEnd: %d\n"
                      "sizeWLimit: %d\n"
                      "computedSize: %d\n",
              ssGray.channels(),
              ssGray.cols,
              ssGray.rows,
              ssGray.depth(),
              (void *) ssGray.datastart,
              (void *) ssGray.dataend,
              (void *) ssGray.datalimit,
              ssGray.dataend - ssGray.datastart,
              ssGray.datalimit - ssGray.datastart,
              ssGray.total() * ssGray.elemSize());
    printf("\n");

    uchar largeBuffer[307200 + 115200];
    const uchar *nv21Ptr = ssGray.ptr();
    for (int i = 0; i < 360 * 320; i++) {
        largeBuffer[i] = *nv21Ptr++;
    }

    cv::Mat frameNV21(360, 320, CV_8UC1, largeBuffer);
    cv::Mat frameRGBA(240, 320, CV_8UC4, largeBuffer + 115200);

    LOG_DEBUG("RubikMemoryInfo",
              "frameNV21: chanels: %d,\n"
                      "width: %d,\n"
                      "height: %d,\n"
                      "depth: %d,\n"
                      "dataStart: %p,\n"
                      "dataEnd: %p,\n"
                      "dataLimit: %p,\n"
                      "sizeWEnd: %d\n"
                      "sizeWLimit: %d\n"
                      "computedSize: %d\n"
                      "largeBufBegin: %p\n"
                      "largeBufEnd: %p",
              frameNV21.channels(),
              frameNV21.cols,
              frameNV21.rows,
              frameNV21.depth(),
              (void *) frameNV21.datastart,
              (void *) frameNV21.dataend,
              (void *) frameNV21.datalimit,
              frameNV21.dataend - frameNV21.datastart,
              frameNV21.datalimit - frameNV21.datastart,
              frameNV21.total() * frameNV21.elemSize(),
              largeBuffer,
              (largeBuffer + 307200 + 115200));
    printf("\n");

    LOG_DEBUG("RubikMemoryInfo",
              "frameRGBA fake RGB: chanels: %d,\n"
                      "width: %d,\n"
                      "height: %d,\n"
                      "depth: %d,\n"
                      "dataStart: %p,\n"
                      "dataEnd: %p,\n"
                      "dataLimit: %p,\n"
                      "sizeWEnd: %d\n"
                      "sizeWLimit: %d\n"
                      "computedSize: %d",
              frameRGBA.channels(),
              frameRGBA.cols,
              frameRGBA.rows,
              frameRGBA.depth(),
              (void *) frameRGBA.datastart,
              (void *) frameRGBA.dataend,
              (void *) frameRGBA.datalimit,
              frameRGBA.dataend - frameRGBA.datastart,
              frameRGBA.datalimit - frameRGBA.datastart,
              frameRGBA.total() * frameRGBA.elemSize());
    printf("\n");

    cv::cvtColor(frameNV21, frameRGBA, CV_YUV2RGBA_I420);

    LOG_DEBUG("RubikMemoryInfo",
              "frameRGBA after RGB conv: chanels: %d,\n"
                      "width: %d,\n"
                      "height: %d,\n"
                      "depth: %d,\n"
                      "dataStart: %p,\n"
                      "dataEnd: %p,\n"
                      "dataLimit: %p,\n"
                      "sizeWEnd: %d\n"
                      "sizeWLimit: %d\n"
                      "computedSize: %d",
              frameRGBA.channels(),
              frameRGBA.cols,
              frameRGBA.rows,
              frameRGBA.depth(),
              (void *) frameRGBA.datastart,
              (void *) frameRGBA.dataend,
              (void *) frameRGBA.datalimit,
              frameRGBA.dataend - frameRGBA.datastart,
              frameRGBA.datalimit - frameRGBA.datastart,
              frameRGBA.total() * frameRGBA.elemSize());
    printf("\n");

    cv::cvtColor(frameRGBA, frameRGBA, CV_RGBA2BGR);
    cv::imwrite(
            "../debug_images/processed_nv21.jpg",
            frameRGBA);
    printf("Processing done, image saved!");
    printf("\n");
}
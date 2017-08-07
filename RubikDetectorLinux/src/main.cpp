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
    std::cout << "Finished video processing." << std::endl;
    return 0;
}

void performProcessingOnVideo() {
    cv::VideoCapture cap;
    cap.open(
            "/media/catalin/Data1/Projects/Android/RubikSolver-All/GithubVersion/RubikSolverCmake/RubikDetectorDemo/RubikDetectorLinux/videos/cube1.mp4");
    if (!cap.isOpened()) {
        std::cout << "Caption not opened, return." << std::endl;
        return;
    }
    cv::Mat frame;

    ResultListener resultListener;
    std::shared_ptr<ImageSaver> imageSaver = std::make_shared<ImageSaver>(
            ImageSaver(std::string(
                    "/media/catalin/Data1/Projects/Android/RubikSolver-All/GithubVersion/RubikSolverCmake/RubikDetectorDemo/RubikDetectorLinux/debug_images")));
    CubeDetector rubikDetector(imageSaver);
    rubikDetector.setDebuggable(false);
    rubikDetector.setOnCubeDetectionResultListener(&resultListener);

    while (cap.read(frame)) {
        cv::cvtColor(frame, frame, CV_BGR2RGB);
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
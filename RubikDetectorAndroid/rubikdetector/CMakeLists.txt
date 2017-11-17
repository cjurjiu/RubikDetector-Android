cmake_minimum_required(VERSION 3.4.1)

# OpenCV rubikdetector_native
SET(OpenCV_STATIC ON)
#environmental variable not working it seems...use hardcoded path for now...
SET(OpenCV_DIR "/media/catalin/SSD/DevUtils/opencv-3.2.0-android-sdk/sdk/native/jni/")
FIND_PACKAGE(OpenCV REQUIRED)

#Enables logging with tags to logcat in c++
FIND_LIBRARY(android-log-lib log)

#Select all c++ files
FILE(GLOB_RECURSE JNI_SOURCES FOLLOW_SYMLINKS "src/main/cpp/*")

#Define the rubikdetector library as a shared lib, with the sources selected earlier
ADD_LIBRARY( rubikdetector_native SHARED ${JNI_SOURCES} )

#link opencv & the android logging lib against the rubikdetector native library
TARGET_LINK_LIBRARIES ( rubikdetector_native

                       #android logging in c++
                       ${android-log-lib}

                       #open-cv
                       ${OpenCV_LIBS} )
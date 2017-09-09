cmake_minimum_required(VERSION 3.6)
project(RubikDetector C CXX)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV REQUIRED)
#rubik detector library target
#sources
FILE(GLOB_RECURSE LIBRARY_SOURCES FOLLOW_SYMLINKS "src/rubikdetectorcore/src/*")

#public headers
FILE(GLOB_RECURSE LIBRARY_PUBLIC_HEADERS FOLLOW_SYMLINKS true "src/rubikdetectorcore/include/*")
set(EXCLUDE_DIR "/internal/")
foreach (TMP_PATH ${LIBRARY_PUBLIC_HEADERS})
    string(FIND ${TMP_PATH} ${EXCLUDE_DIR} EXCLUDE_DIR_FOUND)
    if (NOT (${EXCLUDE_DIR_FOUND} EQUAL -1))
        list(REMOVE_ITEM LIBRARY_PUBLIC_HEADERS ${TMP_PATH})
    endif ()
endforeach (TMP_PATH)

#private headers
FILE(GLOB_RECURSE LIBRARY_PRIVATE_HEADERS FOLLOW_SYMLINKS "src/rubikdetectorcore/include/*")
set(DESIRED_DIR "/internal/")
foreach (TMP_PATH ${LIBRARY_PRIVATE_HEADERS})
    string(FIND ${TMP_PATH} ${DESIRED_DIR} DESIRED_DIR_FOUND)
    if (${DESIRED_DIR_FOUND} EQUAL -1)
        list(REMOVE_ITEM LIBRARY_PRIVATE_HEADERS ${TMP_PATH})
    endif ()
endforeach (TMP_PATH)

MESSAGE("Lib source files: " ${LIBRARY_SOURCES})
MESSAGE("Lib public headers: " ${LIBRARY_PUBLIC_HEADERS})
MESSAGE("Lib private headers: " ${LIBRARY_PRIVATE_HEADERS})

set(LIBRARY_PUBLIC_HEADERS_DIR
        "/src/rubikdetectorcore/include/rubikdetector/data/config/"
        "/src/rubikdetectorcore/include/rubikdetector/data/geometry/"
        "/src/rubikdetectorcore/include/rubikdetector/data/processing/"
        "/src/rubikdetectorcore/include/rubikdetector/detectors/colordetector/"
        "/src/rubikdetectorcore/include/rubikdetector/detectors/faceletsdetector/"
        "/src/rubikdetectorcore/include/rubikdetector/detectors/rubikdetector/"
        "/src/rubikdetectorcore/include/rubikdetector/detectors/rubikdetector/builder/"
        "/src/rubikdetectorcore/include/rubikdetector/drawing/"
        "/src/rubikdetectorcore/include/rubikdetector/imagesaver/"
        "/src/rubikdetectorcore/include/rubikdetector/misc/"
        "/src/rubikdetectorcore/include/rubikdetector/processor/"
        "/src/rubikdetectorcore/include/rubikdetector/utils/")

set(LIBRARY_PRIVATE_HEADERS_DIR
        "/src/rubikdetectorcore/include/rubikdetector/data/geometry/internal/"
        "/src/rubikdetectorcore/include/rubikdetector/data/processing/internal/"
        "/src/rubikdetectorcore/include/rubikdetector/detectors/colordetector/internal/"
        "/src/rubikdetectorcore/include/rubikdetector/detectors/faceletsdetector/internal/"
        "/src/rubikdetectorcore/include/rubikdetector/detectors/rubikdetector/internal/")

add_library(rubikdetector SHARED ${LIBRARY_SOURCES} ${LIBRARY_PUBLIC_HEADERS} ${LIBRARY_PRIVATE_HEADERS})
target_include_directories(rubikdetector PUBLIC ${LIBRARY_PUBLIC_HEADERS_DIR} PRIVATE ${LIBRARY_PRIVATE_HEADERS_DIR})
target_link_libraries(rubikdetector ${OpenCV_LIBS})

#end rubik detector library target

#sample application target
add_executable(rubikdetector_desktop "src/main.cpp")
target_link_libraries(rubikdetector_desktop rubikdetector)
#end sample application target

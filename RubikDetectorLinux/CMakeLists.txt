cmake_minimum_required(VERSION 3.6)
project(RubikDetectorDesktop C CXX)

set(CMAKE_CXX_STANDARD 11)

FILE(GLOB_RECURSE PROJ_SOURCES FOLLOW_SYMLINKS "src/*")
set(SOURCE_FILES ${PROJ_SOURCES})

find_package( OpenCV REQUIRED )

add_executable(RubikDetectorDesktop ${SOURCE_FILES})
target_link_libraries( RubikDetectorDesktop ${OpenCV_LIBS} )

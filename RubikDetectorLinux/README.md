## About

This is the desktop version of RubikDetector. 

However, this can be used as a template to build RubikDetector for any arbitrary platform, as long as the OpenCV binaries for that platform are available.

It does two things:
  * Builds RubikDetector as a shared library
  * Uses that shared library in the program in `./src/main.cpp`

## Build requirements
   - [Cmake 3.4.1 or newer](https://cmake.org/download/)
   - [OpenCV 3.2.0](https://github.com/opencv/opencv/releases/tag/3.2.0) installed on the local machine
   
To build:
```
mkdir build
cd build
cmake ../
```
Then just run it with:
```
./rubikdetector_desktop
```
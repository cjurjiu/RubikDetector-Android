# RubikDetector [ ![Download](https://api.bintray.com/packages/cjurjiu/cjurjiu-opensource/rubikdetector-android/images/download.svg) ](https://bintray.com/cjurjiu/cjurjiu-opensource/rubikdetector-android/_latestVersion) [![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://tldrlegal.com/license/apache-license-2.0-(apache-2.0))

Image Processing library capable of detecing a Rubik's Cube in an input frame. Developed for mobile, but 100% portable.

## Folder Structure:

<pre>
.
├── RubikDetectorCore       ->     C++ image processing library (the core)
├── RubikDetectorAndroid    ->     Android Library, FotoApparat Connector, Demo Project
├── RubikDetectorLinux      ->     Desktop CMake project, demo for usage on UNIX systems
├── LICENSE
└── README.md
</pre>

## Support

The image processing algorithm is written in C++ and is fully portable to other platforms (incl. iOS). 

Currently, samples exist for desktop (linux, OSX) and Android. 

The Android library also has a connector to the popular FotoApparat camera library.

## Current state

In development. Project is currently in pre-alpha.

## Performance

~45-~55 Average FPS at 1080p, while processing frames.

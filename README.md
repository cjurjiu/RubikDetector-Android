# RubikDetector
Image Processing library capable of detecing a Rubik's Cube in an input frame.

## Folder Structure:

<pre>
.
├── rubikdetectorcore			C++ image processing library
├── RubikDetectorAndroid		Android Library, FotoApparat Connector, Demo Project
├── RubikDetectorLinux			Desktop, Linux sample
├── LICENSE
└── README.md
</pre>

## Support

The image processing algorithm is written in C++ and should be fully portable to other platforms (incl. iOS). 

Currently, samples exist for desktop (linux, OSX) and Android. 

The Android library also has a connector to the popular FotoApparat camera library.

## Current state

In development. Project is currently in pre-alpha.

## Performance

~45-~55 Average FPS at 1080p, while processing frames.

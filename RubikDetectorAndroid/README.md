## Structure
<pre>
.
└── RubikDetectorAndroid
   ├── app                   ->     Android demo project
   ├── rubikdetector         ->     RubikDetector Android library
   └── fotoapparatconnector  ->     Fotoapparat Connector for the RubikDetector Android library
</pre>

Since the `rubikdetector` module relies on C++ code, you require the following tools to be able to build it:
   - [Cmake 3.4.1 or newer](https://cmake.org/download/)
   - [OpenCV 3.2.0 for Android SDK](https://github.com/opencv/opencv/releases/tag/3.2.0)

To build the `rubikdetector` module, you need first to download and extract the OpenCV Android SDK somewhere on your computer. 

Then, change line 6 of `./rubikdetector/CMakeLists.txt` from:
```cmake
SET(OpenCV_DIR "/media/catalin/SSD/DevUtils/opencv-3.2.0-android-sdk/sdk/native/jni/")
```
to
```cmake
SET(OpenCV_DIR <disk location where you extracted your OpenCV Android SDK>)
```

And you're done! Now you should be able to build the library as well.

## Demo app

The Demo app uses by default the binaries stored on jcenter for the `rubikdetector` & `fotoapparatconnector`.
 
If you however modify locally either `rubikdetector` or `fotoapparatconnector`, just uncomment the following lines, from `./app/build.gradle`:

 ```groovy
     //local module dependencies
 //    compile project(':rubikdetector')
 //    compile project(':fotoapparatconnector')
 ```
 
 



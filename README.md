# RubikDetector <img src="https://github.com/cjurjiu/RubikDetector-Android/blob/master/images/icons/rubikdetector_icon_small.png" width="60px" />  

[ ![Download](https://api.bintray.com/packages/cjurjiu/cjurjiu-opensource/rubikdetector-android/images/download.svg) ](https://bintray.com/cjurjiu/cjurjiu-opensource/rubikdetector-android/_latestVersion) [![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://tldrlegal.com/license/apache-license-2.0-(apache-2.0))

Image Processing library capable of detecing a Rubik's Cube in an input frame. Developed for mobile, but 100% portable.

Drawing found facelets as rectangles | Drawing found facelets as circles
:--: | :--: 
<img src="https://github.com/cjurjiu/RubikDetector-Android/blob/master/images/gifs/2.gif"/> | <img src="https://github.com/cjurjiu/RubikDetector-Android/blob/master/images/gifs/1.gif"/>

The Android library also has a connector to the popular [FotoApparat](https://github.com/Fotoapparat/Fotoapparat) camera library. See more [here](https://github.com/cjurjiu/RubikDetector-Android).

The information available here relates to the Android library version of RubikDetector. For details on the C++ code, and on the desktop demo, see the README available in the respective folders.

## Table of contents
  * [Folder Structure](#folder-structure)
  * [RubikDetector Usage](#rubikdetector-usage)
  * [Showing things on screen](#showing-things-on-screen)
  * [Memory layout](#memory-layout) 
  * [Fotoapparat Connector](#fotoapparat-connector)
  * [How to build](#how-to-build)
  * [Misc](#misc) 
  * [Binaries](#binaries)
    * [RubikDetector](#rubikdetector-binaries)
    * [Fotoapparat Connector](#fotoapparat-connector-binaries)

## Folder Structure:

<pre>
.
├── RubikDetectorCore         ->     C++ image processing library (the core)
├── RubikDetectorAndroid      ->     Android Library, FotoApparat Connector, Demo Project
│   ├── app                   ->     Android demo project
│   ├── rubikdetector         ->     RubikDetector Android library
│   └── fotoapparatconnector  ->     Fotoapparat Connector for the RubikDetector Android library
|
├── RubikDetectorLinux        ->     Desktop CMake project, demo for usage on UNIX systems
├── LICENSE
└── README.md
</pre>

## RubikDetector Usage

Using the RubikDetector library directly gives you, the library client, great control over the implementation. However, setting everything up correctly can be somewhat tedios.

Using RubikDetector via the Fotoapparat Connector makes things significantly easier, but comes at a significant performance cost. ¯\\\_(ツ)\_/¯

To use the RubikDetector library directly, first you need to create your RubikDetector instance:

```java
RubikDetector rubikDetector = new RubikDetector.Builder()
                // the input image will have a resolution of 1080x1920
                .inputFrameSize(1080, 1920)
                // the input image data will be stored in the YUV NV21 format
                .inputFrameFormat(RubikDetector.ImageFormat.YUV_NV21)
                // draw found facelets as colored rectangles
                .drawConfig(DrawConfig.Rectangles())
                // builds the RubikDetector for the given params.
                .build();
```                

If you want to specify the image format as an `android.graphics.ImageFormat`, you can use the `RubikDetectorUtils` class to convert it to the equivalent `RubikDetector.ImageFormat`:

```java
@RubikDetector.ImageFormat int detectorImageFormat = RubikDetectorUtils.convertAndroidImageFormat(android.graphics.ImageFormat.NV21);
```                 
The detector expects a `byte[]` of a specific size, which will contain the input frame. The input frame data is expected
to be placed at the beggining of the `byte[]`.

To perform the detection, just call:

```java
// getImageData() should return a the byte[] which will contain the image data, and which 
// will have the size expected by the detector
byte[] imageData = getImageData();
// searches for the cube in the data present in the "imageData" byte[]
RubikFacelet[][] facelets = rubikDetector.findCube(imageData);
if(facelets != null) {
    // we found the cube in the frame!
} else {
    // the cube was not found
}
```

The `byte[]` which contains the image, and which will be passed to `RubikDetector#findCube(...)`, needs to have a size at least equal to the total memory required by the RubikDetector. This total memory value is computed based on the dimensions and format of the input frame ( 1080 x 1920, YUV NV21, in this example), and will be larger than what is required to store stictly the input frame in memory. 

To ensure the `byte[]` you pass to the `RubikDetector` has the adquate capacity, perform the following:

```java
//get the image data, store it in a byte[]
byte[] image = getImageFromCamera()

// allocate a ByteBuffer of the capacity required by the RubikDetector
ByteBuffer imageDataBuffer = ByteBuffer.allocateDirect(rubikDetector.getRequiredMemory());

// copy the bytes of the image in the created buffer
imageDataBuffer.put(image, 0, image.length);

// call findCube passing the backing array of the ByteBuffer as a parameter to the RubikDetector
final RubikFacelet[][] result = rubikDetector.findCube(imageDataBuffer.array());
```

This would work just fine if you need to find the cube in a single image. However, this approach is not ideal if you want to perform the detection on a live video stream, as it would allocate a new (huge) buffer each frame, and would then copy the current frame to the newly allocated buffer.

Needless to say, both the allocation, and the copy are expensive operations which would seriously affect the framerate, and would cause considerable memory churn.

An implementation better fitted for live processing would imply the following:
  * preallocate buffers of the size required by the RubikDetector, before camera/video frames start coming in
  * ensure the camera or video frames are written into the preallocated buffers. 
  * send the preallocated buffers to the detector for processing
  * once processing has been perform, reuse each preallocated buffer
  
In code, this means the following (Android Camera1 API):

```java
android.hardware.Camera camera = Camera.open(MAIN_CAMERA_ID);

//add 3 buffers to the camera. 3 is just an arbitrary value...pick whatever suits your code best
camera.addCallbackBuffer(ByteBuffer.allocateDirect(rubikDetector.getRequiredMemory()).array());
camera.addCallbackBuffer(ByteBuffer.allocateDirect(rubikDetector.getRequiredMemory()).array());
camera.addCallbackBuffer(ByteBuffer.allocateDirect(rubikDetector.getRequiredMemory()).array());

// set camera params, configure other options
...
// start the camera preview
camera.startPreview();
```

Then in `onPreviewFrame(...)`:
```java
@Override
public void onPreviewFrame(byte[] data, Camera camera) {
    //the byte[] data parameter is one of the buffers preallocated earlier, and has the
    //size required by the RubikDetector
    
    //additionally now it also contains the preview frame! we can just call  findCube(...) on it
    final RubikFacelet[][] result = rubikDetector.findCube(data);
    
    if(facelets != null) {
        // we found the cube in the frame!
    } else {
        // the cube was not found
    }
    
    //finally, add the byte array back as a buffer, so it can be reused.
    camera.addCallbackBuffer(data);
}
```

This technique is used in the Android demo app, in the LiveProcessingActivity. Camera2 sample is also in the works.

## Showing things on screen

### Getting the frame being processed

When searching for facelets within the input frame, the detector creates an RGBA copy of the input frame. This copy is regarded as being the "output frame", and its image data will be stored in the RGBA format, regardless of the data format of the input frame.

It's called the "output frame" because this is a read-to-display, already converted to RGBA, version of the input data. 

This output frame will be written back into the same `byte[]` which also stores the input frame. The output frame is always created, even if the cube is not found in the correspunding input frame.

To prevent the ouput frame overriding the input frame, the output frame will be written at a certain offset in the `byte[]`.

To get the ouput frame as a bitmap:

```java
// get a byte[] with sufficient capacity, which also contains the input frame 
byte[] data = getByteArrayWithInputFrame()
        
// search for the Rubik's Cube in the input frame. regardless whether the cube
// is found or not, this creates the ouput frame.
RubikFacelet[][] facelets = rubikDetector.findCube(data);
        
// allocate a buffer of suficient capacity to store the output frame.
ByteBuffer drawingBuffer = ByteBuffer.allocate(rubikDetector.getResultFrameByteCount());
//copy the output frame data into the buffer
drawingBuffer.put(data, rubikDetector.getResultFrameBufferOffset(), rubikDetector.getResultFrameByteCount());
drawingBuffer.rewind();
        
// create a bitmap for the output frame
Bitmap drawingBitmap = Bitmap.createBitmap(frameWidth, frameHeight, Bitmap.Config.ARGB_8888);
// write the output frame data to the bitmap
drawingBitmap.copyPixelsFromBuffer(drawingBuffer);
```
`BitmapFactory.decoteByteArray(...)` can also be used as an one line alternative. Most likely however, in practice, the bitmap + buffer initialization and data ssignment will take place in different places. 

This make the above code slightly better for demonstration purposes, although longer.

### Drawing the facelets

The detector is capable of drawing the found facelets, as seem below:

<img src="https://github.com/cjurjiu/RubikDetector-Android/blob/master/images/gifs/2.gif"/>

To configure how the frames are drawn, you can supply a `DrawConfig` instance to the builder of the RubikProcessor, or to `RubikDetector#setDrawConfig(...)`. 

Drawing is performed in C++, right after a succesful detection. The result of the drawing will be available in the output frame described at the previous section.

To disable drawing, use `DrawConfig.DoNotDraw()`. If `DrawConfig.DoNotDraw()` is used, but at some point you still want to draw the found facelets, then `RubikDetectorUtils.drawFaceletsAsCircles(...)` & `RubikDetectorUtils.drawFaceletsAsRectangles(...)` can be used.

These methods expect, among others, a canvas to which to draw the facelets. When using these methods, drawing will be performed using the standard Android Canvas drawing API.

Regardless whether drawing is performed in C++ or Java, the original image frame on which the detection ocurred, is left untouched. 

## Memory Layout

TBD
 
## Fotoapparat Connector

TBD
 
## How to build

TBD
   
## Misc

### Support

The image processing algorithm is written in C++ and is fully portable to other platforms (incl. iOS). 

Currently, samples exist for desktop (linux, OSX) and Android. 

The Android library also has a connector to the popular FotoApparat camera library.

### Current state

In development. Project is currently in open beta.

### Performance

~45-~55 Average FPS at 1080p, while processing frames.

## Binaries

### RubikDetector Binaries
Binaries and dependency information for Maven, Ivy, Gradle and others can be found on [jcenter](https://bintray.com/cjurjiu/cjurjiu-opensource/rubikdetector-android).

Example for Gradle:

```groovy
compile 'com.catalinjurjiu:rubikdetector-android:1.0.0-beta2'
```

and for Maven:

```xml
<dependency>
  <groupId>com.catalinjurjiu</groupId>
  <artifactId>rubikdetector-android</artifactId>
  <version>1.0.0-beta2</version>
  <type>pom</type>
</dependency>
```
and for Ivy:

```xml
<dependency org='com.catalinjurjiu' name='rubikdetector-android' rev='1.0.0-beta2'>
  <artifact name='rubikdetector-android' ext='pom' ></artifact>
</dependency>
```
### Fotoapparat Connector Binaries

Complete details available on [jcenter](https://bintray.com/cjurjiu/cjurjiu-opensource/rubikdetector-fotoapparat-connector).

Example for Gradle:

```groovy
compile 'com.catalinjurjiu:rubikdetector-fotoapparat-connector:1.0.0-beta2'
```

and for Maven:

```xml
<dependency>
  <groupId>com.catalinjurjiu</groupId>
  <artifactId>rubikdetector-fotoapparat-connector</artifactId>
  <version>1.0.0-beta2</version>
  <type>pom</type>
</dependency>
```
and for Ivy:

```xml
<dependency org='com.catalinjurjiu' name='rubikdetector-fotoapparat-connector' rev='1.0.0-beta2'>
  <artifact name='rubikdetector-fotoapparat-connector' ext='pom' ></artifact>
</dependency>
```

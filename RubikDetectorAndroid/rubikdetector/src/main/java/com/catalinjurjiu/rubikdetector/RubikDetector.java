package com.catalinjurjiu.rubikdetector;

import android.support.annotation.IntDef;
import android.util.Log;

import com.catalinjurjiu.rubikdetector.config.DrawConfig;
import com.catalinjurjiu.rubikdetector.model.Point2d;
import com.catalinjurjiu.rubikdetector.model.RubikFacelet;

import java.nio.ByteBuffer;

/**
 * Created by catalin on 11.07.2017.
 */

@SuppressWarnings("unused")
public class RubikDetector {

    public static final int NATIVE_DETECTOR_RELEASED = -1;
    public static final int DATA_SIZE = 6;

    static {
        System.loadLibrary("opencv_java3");
        System.loadLibrary("native_processing");
    }

    private long cubeDetectorHandle = -1;
    private int frameHeight;
    private int frameWidth;
    private int requiredMemory;
    private int resultFrameBufferOffset;
    private int resultFrameByteCount;
    private int inputFrameByteCount;
    private int inputFrameBufferOffset;
    private OnCubeDetectionResultListener listener;

    private RubikDetector(ImageProperties properties, DrawConfig drawConfig) {
        this(properties, drawConfig, null);
    }

    private RubikDetector(ImageProperties properties, DrawConfig drawConfig, String storagePath) {
        this.cubeDetectorHandle = createNativeDetector(properties, drawConfig, storagePath);
        this.frameWidth = properties.width;
        this.frameHeight = properties.height;
        syncWithNativeObject();
    }

    private long createNativeDetector(ImageProperties properties, DrawConfig drawConfig, String storagePath) {

        return nativeCreateRubikDetector(properties.width,
                properties.height,
                properties.inputImageFormat,
                drawConfig.getDrawMode(),
                drawConfig.getStrokeWidth(),
                drawConfig.isFillShape(),
                storagePath);
    }

    public void updateImageProperties(ImageProperties properties) {
        applyImageProperties(properties.width, properties.height, properties.inputImageFormat);
    }

    public void updateImageProperties(int width, int height, @ImageFormat int imageFormat) {
        applyImageProperties(width, height, imageFormat);
    }

    private void applyImageProperties(int width, int height, @ImageFormat int imageFormat) {
        if (isActive()) {
            nativeSetImageProperties(cubeDetectorHandle, width, height, imageFormat);
            this.frameWidth = width;
            this.frameHeight = height;
            syncWithNativeObject();
        }
    }

    private void syncWithNativeObject() {
        if (isActive()) {
            this.requiredMemory = nativeGetRequiredMemory(cubeDetectorHandle);
            this.resultFrameBufferOffset = nativeGetRgbaImageOffset(cubeDetectorHandle);
            this.resultFrameByteCount = nativeGetRgbaImageSize(cubeDetectorHandle);
            this.inputFrameByteCount = nativeGetNv21ImageSize(cubeDetectorHandle);
            this.inputFrameBufferOffset = nativeGetNv21ImageOffset(cubeDetectorHandle);
        }
    }

    public void setDebuggable(boolean debuggable) {
        if (isActive()) {
            nativeSetDebuggable(cubeDetectorHandle, debuggable);
        }
    }

    public void releaseResources() {
        Log.d("CATAMEM", "RubikDetector#releaseResources.");
        if (isActive()) {
            Log.d("CATAMEM", "RubikDetector#releaseResources - handle not -1, calling native release.");
            nativeReleaseCubeDetector(cubeDetectorHandle);
            cubeDetectorHandle = NATIVE_DETECTOR_RELEASED;
        }
    }

    public RubikFacelet[][] findCube(byte[] imageData) {
        if (isActive()) {
            int[] nativeResult = nativeFindCubeImageData(cubeDetectorHandle, imageData);
            return decodeResult(nativeResult);
        }
        return null;
    }

    public RubikFacelet[][] findCube(ByteBuffer imageDataBuffer) {
        if (!imageDataBuffer.isDirect()) {
            throw new IllegalArgumentException("The image data buffer needs to be a direct buffer.");
        }
        if (isActive()) {
            int[] nativeResult = nativeFindCubeImageDataBuffer(cubeDetectorHandle, imageDataBuffer);
            return decodeResult(nativeResult);
        }
        return null;
    }

    public boolean isActive() {
        return cubeDetectorHandle != NATIVE_DETECTOR_RELEASED;
    }

    public int getRequiredMemory() {
        return requiredMemory;
    }

    public int getResultFrameBufferOffset() {
        return resultFrameBufferOffset;
    }

    public int getResultFrameByteCount() {
        return resultFrameByteCount;
    }

    public void overrideInputFrameWithResultFrame(byte[] data) {
        if (isActive()) {
            nativeOverrideInputFrameWithResultFrame(cubeDetectorHandle, data);
        }
    }

    public int getInputFrameBufferOffset() {
        return inputFrameBufferOffset;
    }

    public int getInputFrameByteCount() {
        return inputFrameByteCount;
    }

    public int getFrameWidth() {
        return frameWidth;
    }

    public int getFrameHeight() {
        return frameHeight;
    }

    /**
     * Called from c++
     *
     * @param detectionResult
     */
    private void onFacetColorsDetected(int[] detectionResult) {
        Log.d("RubikResult", "####Facelets discovered!!!!!!");
        RubikFacelet[][] result = decodeResult(detectionResult);
        Log.d("RubikResult", "Colors: " + RubikDetectorUtils.getResultColorsAsString(result));
        if (listener != null) {
            listener.onCubeDetectionResult(result);
        }
    }

    private RubikFacelet[][] decodeResult(int[] detectionResult) {
        if (detectionResult == null) {
            return null;
        }

        RubikFacelet[][] result = new RubikFacelet[3][3];
        for (int i = 0; i < detectionResult.length; i += DATA_SIZE) {
            result[i / (3 * DATA_SIZE)][(i % (3 * DATA_SIZE)) / DATA_SIZE] = new RubikFacelet(
                    detectionResult[i],
                    new Point2d(detectionResult[i + 1] / 100000f, detectionResult[i + 2] / 100000f),
                    detectionResult[i + 3] / 100000f,
                    detectionResult[i + 4] / 100000f,
                    detectionResult[i + 5] / 100000f
            );
        }
        return result;
    }

    private native long nativeCreateRubikDetector(int frameWidth,
                                                  int frameHeight,
                                                  int inputImageFormat,
                                                  int drawMode,
                                                  int strokeWidth,
                                                  boolean fillShape,
                                                  String storagePath);

    private native void nativeSetDebuggable(long nativeDetectorRef, boolean debuggable);

    private native void nativeSetDrawFoundFacelets(long nativeDetectorRef, boolean shouldDrawFoundFacelets);

    private native int[] nativeFindCubeImageData(long nativeDetectorRef, byte[] imageData);

    private native int[] nativeFindCubeImageDataBuffer(long nativeDetectorRef, ByteBuffer imageDataBuffer);

    private native void nativeOverrideInputFrameWithResultFrame(long nativeDetectorRef, byte[] data);

    private native void nativeReleaseCubeDetector(long nativeDetectorRef);

    private native void nativeSetImageProperties(long nativeDetectorRef, int width, int height, int imageFormat);

    private native int nativeGetRequiredMemory(long cubeDetectorHandle);

    private native int nativeGetRgbaImageOffset(long cubeDetectorHandle);

    private native int nativeGetRgbaImageSize(long cubeDetectorHandle);

    private native int nativeGetNv21ImageSize(long cubeDetectorHandle);

    private native int nativeGetNv21ImageOffset(long cubeDetectorHandle);

    private void setDrawParams(DrawConfig drawConfig) {

    }

    @IntDef
    public @interface ImageFormat {
        int YUV_NV21 = 0,
                YUV_NV12 = 1,
                YUV_I420 = 2,
                YUV_YV12 = 3,
                ARGB_8888 = 4;
    }

    @IntDef
    public @interface CubeColors {
        int RED = 0,
                ORANGE = 1,
                YELLOW = 2,
                GREEN = 3,
                BLUE = 4,
                WHITE = 5;
    }

    public interface OnCubeDetectionResultListener {
        void onCubeDetectionResult(RubikFacelet facelets[][]);
    }

    public static class ImageProperties {
        public final int width;
        public final int height;
        public final
        @ImageFormat
        int inputImageFormat;

        public ImageProperties(int width, int height, int inputImageFormat) {
            this.width = width;
            this.height = height;
            this.inputImageFormat = inputImageFormat;
        }
    }

    public static class Builder {
        private DrawConfig drawConfig;
        private boolean debuggable;
        private String imageSavePath = null;
        private int inputFrameWidth = 320;
        private int inputFrameHeight = 240;
        @ImageFormat
        private int inputFrameFormat = ImageFormat.YUV_NV21;

        public Builder inputFrameSize(int inputFrameWidth, int inputFrameHeight) {
            this.inputFrameWidth = inputFrameWidth;
            this.inputFrameHeight = inputFrameHeight;
            return this;
        }

        public Builder inputFrameFormat(int inputFrameFormat) {
            this.inputFrameFormat = inputFrameFormat;
            return this;
        }

        public Builder drawConfig(DrawConfig drawConfig) {
            this.drawConfig = drawConfig;
            return this;
        }

        public Builder debuggable(boolean debuggable) {
            this.debuggable = debuggable;
            return this;
        }

        public Builder imageSavePath(String imageSavePath) {
            this.imageSavePath = imageSavePath;
            return this;
        }

        public RubikDetector build() {
            ImageProperties imageProperties = new ImageProperties(inputFrameWidth, inputFrameHeight, inputFrameFormat);
            if (drawConfig == null) {
                drawConfig = DrawConfig.Default();
            }
            RubikDetector rubikDetector = new RubikDetector(imageProperties, drawConfig, imageSavePath);
            rubikDetector.setDebuggable(debuggable);
            return rubikDetector;
        }
    }
}

package com.catalinjurjiu.rubikdetector;

import android.support.annotation.IntDef;
import android.util.Log;

import java.util.Arrays;

/**
 * Created by catalin on 11.07.2017.
 */

public class RubikDetector {

    public static final int NO_OFFSET = 0;
    public static final int NATIVE_DETECTOR_RELEASED = -1;

    static {
        System.loadLibrary("opencv_java3");
        System.loadLibrary("native_processing");
    }

    private OnCubeDetectionResultListener listener;
    private long cubeDetectorHandle = -1;
    private int height;
    private int width;
    private int totalRequiredMemory;
    private int rgbaImageOffset;
    private int rgbaImageSize;
    private int nv21ImageSize;

    public RubikDetector() {
        cubeDetectorHandle = createNativeObject(null);
    }

    public RubikDetector(String storagePath) {
        cubeDetectorHandle = createNativeObject(storagePath);
    }

    public void setImageDimensions(int width, int height) {
        if (cubeDetectorHandle != NATIVE_DETECTOR_RELEASED) {
            this.width = width;
            this.height = height;
            this.totalRequiredMemory = width * height * 4 + width * (height + height / 2);
            this.rgbaImageOffset = width * (height + height / 2);
            this.rgbaImageSize = width * height * 4;
            this.nv21ImageSize = width * (height + height / 2);
            nativeSetImageDimensions(cubeDetectorHandle, width, height);
        }
    }

    private native void nativeSetImageDimensions(long nativeDetectorRef, int width, int height);

    public void setDebuggable(boolean debuggable) {
        if (cubeDetectorHandle != NATIVE_DETECTOR_RELEASED) {
            nativeSetDebuggable(cubeDetectorHandle, debuggable);
        }
    }

    public void releaseResources() {
        Log.d("CATAMEM", "RubikDetector#releaseResources.");
        if (cubeDetectorHandle != NATIVE_DETECTOR_RELEASED) {
            Log.d("CATAMEM", "RubikDetector#releaseResources - handle not -1, calling native release.");
            nativeReleaseCubeDetector(cubeDetectorHandle);
            cubeDetectorHandle = NATIVE_DETECTOR_RELEASED;
        }
    }

    private native void nativeReleaseCubeDetector(long nativeDetectorRef);

    public void setOnCubeDetectionResultListener(OnCubeDetectionResultListener onCubeDetectionResultListener) {
        this.listener = onCubeDetectionResultListener;
    }

    public void findCube(byte[] imageData) {
        if (cubeDetectorHandle != NATIVE_DETECTOR_RELEASED) {
            findCubeNativeImageData(cubeDetectorHandle, imageData);
        }
    }

    /**
     * Called from c++
     *
     * @param faceletColors
     */
    @SuppressWarnings("unused")
    private void onFacetColorsDetected(int[] faceletColors) {
        Log.d("Cata", "####Facelets discovered!!!!!!" + Arrays.toString(faceletColors));
        if (listener != null) {
            listener.onCubeDetectionResult(to2DArray(faceletColors));
        }
    }

    private int[][] to2DArray(int colors[]) {
        int[][] result = new int[3][3];
        for (int i = 0; i < 3; i++) {
            System.arraycopy(colors, i * 3, result[i], 0, 3);
        }
        return result;
    }

    public boolean isActive() {
        return cubeDetectorHandle != NATIVE_DETECTOR_RELEASED;
    }

    public int getTotalRequiredMemory() {
        //width * height * 4 channels for RGBA + width + height*1.5 for NV21
        return totalRequiredMemory;
    }

    public int getRgbaImageOffset() {
        return rgbaImageOffset;
    }

    public int getRgbaImageSize() {
        return rgbaImageSize;
    }

    public int getNv21ImageOffset() {
        return NO_OFFSET;
    }

    public int getNv21ImageSize() {
        return nv21ImageSize;
    }

    private native long createNativeObject(String storagePath);

    private native void nativeSetDebuggable(long nativeDetectorRef, boolean debuggable);

    private native void findCubeNativeImageData(long nativeDetectorRef, byte[] imageData);

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
        void onCubeDetectionResult(int colors[][]);
    }
}

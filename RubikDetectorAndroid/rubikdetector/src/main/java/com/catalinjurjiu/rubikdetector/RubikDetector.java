package com.catalinjurjiu.rubikdetector;

import android.support.annotation.IntDef;
import android.util.Log;

import org.opencv.core.Mat;

import java.util.Arrays;

/**
 * Created by catalin on 11.07.2017.
 */

public class RubikDetector {

    static {
        System.loadLibrary("opencv_java3");
        System.loadLibrary("native_processing");
    }

    private OnCubeDetectionResultListener listener;
    private long cubeDetectorHandle = -1;

    public RubikDetector() {
        cubeDetectorHandle = createNativeObject(null);
    }

    public RubikDetector(String storagePath) {
        cubeDetectorHandle = createNativeObject(storagePath);
    }

    public void setDebuggable(boolean debuggable) {
        if (cubeDetectorHandle != -1) {
            nativeSetDebuggable(cubeDetectorHandle, debuggable);
        }
    }

    public void releaseResources() {
        Log.d("CATAMEM", "RubikDetector#releaseResources.");
        if (cubeDetectorHandle != -1) {
            Log.d("CATAMEM", "RubikDetector#releaseResources - handle not -1, calling native release.");
            nativeReleaseCubeDetector(cubeDetectorHandle);
            //TODO cleanup native part
            cubeDetectorHandle = -1;
        }
    }

    private native void nativeReleaseCubeDetector(long nativeDetectorRef);

    public void setOnCubeDetectionResultListener(OnCubeDetectionResultListener onCubeDetectionResultListener) {
        this.listener = onCubeDetectionResultListener;
    }

    public void findCube(long addrRgba) {
        if (cubeDetectorHandle != -1) {
            findCubeNativeMatAddr(cubeDetectorHandle, addrRgba);
        }
    }

    public void findCube(Mat mat) {
        if (cubeDetectorHandle != -1) {
            findCubeNativeMatAddr(cubeDetectorHandle, mat.getNativeObjAddr());
        }
    }

    //TODO async processing w listener
    public byte[] findCube(byte[] imageData, int width, int height) {
        if (cubeDetectorHandle != -1) {
            return findCubeNativeImageData(cubeDetectorHandle, imageData, width, height);
        }
        return null;
    }

    private native long createNativeObject(String storagePath);

    private native void nativeSetDebuggable(long nativeDetectorRef, boolean debuggable);

    private native void findCubeNativeMatAddr(long nativeDetectorRef, long addrRgba);

    private native byte[] findCubeNativeImageData(long nativeDetectorRef, byte[] imageData, int width, int height);

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
        return cubeDetectorHandle != -1;
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
        void onCubeDetectionResult(int colors[][]);
    }
}

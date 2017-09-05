package com.catalinjurjiu.rubikdetector;

import android.support.annotation.IntDef;
import android.util.Log;

import com.catalinjurjiu.rubikdetector.model.Point2d;
import com.catalinjurjiu.rubikdetector.model.RubikFacelet;

/**
 * Created by catalin on 11.07.2017.
 */

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
    private int totalRequiredMemory;
    private int rgbaImageOffset;
    private int rgbaImageSize;
    private int nv21ImageSize;
    private int nv21ImageOffset;
    private OnCubeDetectionResultListener listener;

    public RubikDetector() {
        this(null);
    }

    public RubikDetector(String storagePath) {
        cubeDetectorHandle = createNativeObject(storagePath);
    }

    public void setImageDimensions(int width, int height) {
        if (isActive()) {
            nativeSetImageDimensions(cubeDetectorHandle, width, height);
            this.frameWidth = width;
            this.frameHeight = height;
            this.totalRequiredMemory = nativeGetTotalRequiredMemory(cubeDetectorHandle);
            this.rgbaImageOffset = nativeGetRgbaImageOffset(cubeDetectorHandle);
            this.rgbaImageSize = nativeGetRgbaImageSize(cubeDetectorHandle);
            this.nv21ImageSize = nativeGetNv21ImageSize(cubeDetectorHandle);
            this.nv21ImageOffset = nativeGetNv21ImageOffset(cubeDetectorHandle);
        }
    }

    public void setDebuggable(boolean debuggable) {
        if (isActive()) {
            nativeSetDebuggable(cubeDetectorHandle, debuggable);
        }
    }

    public void setDrawFoundFacelets(boolean drawFoundFacelets) {
        if (isActive()) {
            nativeSetDrawFoundFacelets(cubeDetectorHandle, drawFoundFacelets);
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
            int[] nativeResult = findCubeNativeImageData(cubeDetectorHandle, imageData);
            return decodeResult(nativeResult);
        }
        return null;
    }

    public boolean isActive() {
        return cubeDetectorHandle != NATIVE_DETECTOR_RELEASED;
    }

    public int getTotalRequiredMemory() {
        return totalRequiredMemory;
    }

    public int getRgbaImageOffset() {
        return rgbaImageOffset;
    }

    public int getRgbaImageSize() {
        return rgbaImageSize;
    }

    public int getNv21ImageOffset() {
        return nv21ImageOffset;
    }

    public int getNv21ImageSize() {
        return nv21ImageSize;
    }

    /**
     * Called from c++
     *
     * @param detectionResult
     */
    @SuppressWarnings("unused")
    private void onFacetColorsDetected(int[] detectionResult) {
        Log.d("RubikResult", "####Facelets discovered!!!!!!");
        RubikFacelet[][] result = decodeResult(detectionResult);
        printResult(result);
        if (listener != null) {
            listener.onCubeDetectionResult(result);
        }
    }

    private void printResult(RubikFacelet[][] result) {
        StringBuilder stringBuilder = new StringBuilder("Colors: {");
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (j == 0) {
                    stringBuilder.append(" {");
                }
                switch (result[i][j].color) {
                    case CubeColors.WHITE:
                        stringBuilder.append("WHITE ");
                        break;
                    case CubeColors.YELLOW:
                        stringBuilder.append("YELLOW ");
                        break;
                    case CubeColors.RED:
                        stringBuilder.append("RED ");
                        break;
                    case CubeColors.BLUE:
                        stringBuilder.append("BLUE ");
                        break;
                    case CubeColors.GREEN:
                        stringBuilder.append("GREEN ");
                        break;
                    case CubeColors.ORANGE:
                        stringBuilder.append("ORANGE ");
                        break;

                }
                stringBuilder.append(", ");
                stringBuilder.append(result[i][j].angle);
                if (j == 2) {
                    stringBuilder.append("} ");
                }
            }
        }
        stringBuilder.append("}");
        Log.d("RubikResult", "Result: " + stringBuilder.toString());
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

    private native long createNativeObject(String storagePath);

    private native void nativeSetDebuggable(long nativeDetectorRef, boolean debuggable);

    private native void nativeSetDrawFoundFacelets(long nativeDetectorRef, boolean shouldDrawFoundFacelets);

    private native int[] findCubeNativeImageData(long nativeDetectorRef, byte[] imageData);

    private native void nativeReleaseCubeDetector(long nativeDetectorRef);

    private native void nativeSetImageDimensions(long nativeDetectorRef, int width, int height);

    private native int nativeGetTotalRequiredMemory(long cubeDetectorHandle);

    private native int nativeGetRgbaImageOffset(long cubeDetectorHandle);

    private native int nativeGetRgbaImageSize(long cubeDetectorHandle);

    private native int nativeGetNv21ImageSize(long cubeDetectorHandle);

    private native int nativeGetNv21ImageOffset(long cubeDetectorHandle);

    public int getFrameWidth() {
        return frameWidth;
    }

    public int getFrameHeight() {
        return frameHeight;
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
}

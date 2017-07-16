package com.catalinjurjiu.rubikdetector;

import android.util.Log;

import org.opencv.core.Mat;

import java.util.Arrays;

/**
 * Created by catalin on 11.07.2017.
 */

public class RubikDetector {

    static {
        System.loadLibrary("native_processing");
    }

    private OnCubeDetectionResultListener listener;

    public RubikDetector() {
        //TODO create native object
    }

    private native void findCubeNative(long addrRgba);

    public void setOnCubeDetectionResultListener(OnCubeDetectionResultListener onCubeDetectionResultListener) {
        this.listener = onCubeDetectionResultListener;
    }

    public void findCube(Mat mat) {
        findCubeNative(mat.getNativeObjAddr());
    }

    public void onFacetColorsDetected(int[] faceletColors) {
        Log.d("Cata", "####Facelets discovered!!!!!!" + Arrays.toString(faceletColors));
    }

    public interface OnCubeDetectionResultListener {
        void onCubeDetectionResult();
    }

}

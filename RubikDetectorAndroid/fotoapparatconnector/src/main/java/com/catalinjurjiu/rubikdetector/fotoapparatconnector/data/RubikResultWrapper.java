package com.catalinjurjiu.rubikdetector.fotoapparatconnector.data;

import com.catalinjurjiu.rubikdetector.model.RubikFacelet;

/**
 * Created by catalin on 01.09.2017.
 */

public class RubikResultWrapper {

    private int frameWidth;
    private int frameHeight;
    private RubikFacelet[][] detectedFacelets;

    public RubikResultWrapper(int frameWidth, int frameHeight, RubikFacelet[][] detectedFacelets) {
        this.frameWidth = frameWidth;
        this.frameHeight = frameHeight;
        this.detectedFacelets = detectedFacelets;
    }

    public int getFrameWidth() {
        return frameWidth;
    }

    public int getFrameHeight() {
        return frameHeight;
    }

    public RubikFacelet[][] getDetectedFacelets() {
        return detectedFacelets;
    }
}

package com.catalinjurjiu.rubikdetector.fotoapparatconnector.data;

import android.support.annotation.Nullable;

import com.catalinjurjiu.rubikdetector.model.RubikFacelet;

/**
 * Immutable wrapped detection result. Detected facelets are available through {@link #getDetectedFacelets()}.
 * <p>
 * Stored facelets can be null, if this {@link RubikFaceletsWrapper} is for a frame in which facelets were not found.
 */
public class RubikFaceletsWrapper {

    private final int frameWidth;
    private final int frameHeight;
    private final RubikFacelet[][] detectedFacelets;

    /**
     * Creates an immutable {@link RubikFaceletsWrapper}.
     *
     * @param detectedFacelets detected facelets, or null if facelets are not known.
     * @param frameWidth       the width of the frame in which the detection/search was performed, in pixels.
     * @param frameHeight      the height of the frame in which the detection/search was performed, in pixels.
     */
    public RubikFaceletsWrapper(@Nullable RubikFacelet[][] detectedFacelets, int frameWidth, int frameHeight) {
        this.detectedFacelets = detectedFacelets;
        this.frameWidth = frameWidth;
        this.frameHeight = frameHeight;
    }

    /**
     * Returns the width of the frame in which the facelets wrapped by this {@link RubikFaceletsWrapper} were detected in.
     *
     * @return the width of the frame, in pixels.
     */
    public int getFrameWidth() {
        return frameWidth;
    }

    /**
     * Returns the height of the frame in which the facelets wrapped by this {@link RubikFaceletsWrapper} were detected in.
     *
     * @return the height of the frame, in pixels.
     */
    public int getFrameHeight() {
        return frameHeight;
    }

    /**
     * Returns the facelets stored by this wrapper. Returns null if this wrapper stores no facelets, or if facelets have not
     * been found in a certain frame.
     *
     * @return the stored {@link RubikFacelet}s, or {@code null} if there are no facelets available.
     */
    @Nullable
    public RubikFacelet[][] getDetectedFacelets() {
        return detectedFacelets;
    }
}

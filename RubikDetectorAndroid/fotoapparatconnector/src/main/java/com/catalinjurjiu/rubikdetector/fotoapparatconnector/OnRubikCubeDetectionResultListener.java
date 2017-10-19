package com.catalinjurjiu.rubikdetector.fotoapparatconnector;

import com.catalinjurjiu.rubikdetector.fotoapparatconnector.data.RubikFaceletsWrapper;

/**
 * Listener notified with the result of a search for the facelets of a Rubik's Cube, in a certain frame.
 *
 * @see OnRubikCubeDetectionResultListener#onRubikCubeDetectionResult(RubikFaceletsWrapper)
 */
public interface OnRubikCubeDetectionResultListener {
    /**
     * Called when the result of a search for the Rubik's Cube facelets is available.
     *
     * @param rubikResult a {@link RubikFaceletsWrapper} representing the result.
     * @see RubikFaceletsWrapper
     */
    void onRubikCubeDetectionResult(RubikFaceletsWrapper rubikResult);
}

package com.catalinjurjiu.rubikdetector.fotoapparatconnector;

import android.os.Handler;
import android.os.Looper;
import android.util.Log;

import com.catalinjurjiu.rubikdetector.RubikDetector;
import com.catalinjurjiu.rubikdetector.RubikDetectorUtils;
import com.catalinjurjiu.rubikdetector.model.RubikFacelet;

import java.nio.ByteBuffer;

import io.fotoapparat.preview.Frame;
import io.fotoapparat.preview.FrameProcessor;

/**
 * Created by catalin on 28.08.2017.
 */

public class RubikDetectorFrameProcessor implements FrameProcessor {

    private final RubikDetector rubikDetector;
    private final ByteBuffer imageDataBuffer;
    private final Handler resultHandler;
    private final OnRubikCubeFaceletsFoundListener listener;

    public RubikDetectorFrameProcessor(RubikDetector rubikDetector, OnRubikCubeFaceletsFoundListener listener) {
        this.rubikDetector = rubikDetector;
        this.imageDataBuffer = ByteBuffer.allocateDirect(rubikDetector.getRequiredMemory());
        this.listener = listener;
        this.resultHandler = new Handler(Looper.getMainLooper());
    }

    public RubikDetectorFrameProcessor(RubikDetector rubikDetector, OnRubikCubeFaceletsFoundListener listener, Looper looper) {
        this.rubikDetector = rubikDetector;
        this.imageDataBuffer = ByteBuffer.allocateDirect(rubikDetector.getRequiredMemory());
        this.resultHandler = new Handler(looper);
        this.listener = listener;
    }

    @Override
    public void processFrame(Frame frame) {
        Log.d("FotoapparatResult", "Found cube, notifying listener.");
        imageDataBuffer.rewind();
        imageDataBuffer.put(frame.image, 0, frame.image.length);
        final RubikFacelet[][] result = rubikDetector.findCube(imageDataBuffer.array());
//        imageDataBuffer.rewind();
//        rubikDetector.overrideInputFrameWithResultFrame(imageDataBuffer.array());
//        imageDataBuffer.get(frame.image, rubikDetector.getInputFrameBufferOffset(), rubikDetector.getInputFrameByteCount());
        if (result != null) {
            RubikDetectorUtils.rescaleResults(result, frame.size.width, frame.size.height, 1560, 1080);
//            RubikDetectorUtils.rotateResults(result, frame.rotation);
        }
        Log.d("FotoapparatResult", "Found cube, notifying listener.");
        resultHandler.post(new Runnable() {
            @Override
            public void run() {
                if (listener != null) {
                    listener.onRubikCubeFaceletsFound(result);
                }
            }
        });
    }

    public interface OnRubikCubeFaceletsFoundListener {
        void onRubikCubeFaceletsFound(RubikFacelet[][] cubeFacelets);
    }
}

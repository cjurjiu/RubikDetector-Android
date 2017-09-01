package com.catalinjurjiu.rubikdetector.fotoapparatconnector;

import android.os.Handler;
import android.os.Looper;
import android.util.Log;

import com.catalinjurjiu.rubikdetector.RubikDetector;
import com.catalinjurjiu.rubikdetector.model.RubikFacelet;

import java.nio.ByteBuffer;

import io.fotoapparat.parameter.Size;
import io.fotoapparat.preview.Frame;
import io.fotoapparat.preview.FrameProcessor;

/**
 * Created by catalin on 28.08.2017.
 */

public class RubikDetectorFrameProcessor implements FrameProcessor, RubikDetectorSizeSelector.SizeSelectionListener {

    private final RubikDetector rubikDetector;
    private final Handler resultHandler;
    private final OnRubikCubeFaceletsFoundListener listener;
    private ByteBuffer imageDataBuffer;

    public RubikDetectorFrameProcessor(RubikDetector rubikDetector, OnRubikCubeFaceletsFoundListener listener) {
        this.rubikDetector = rubikDetector;
        this.listener = listener;
        this.resultHandler = new Handler(Looper.getMainLooper());
    }

    public RubikDetectorFrameProcessor(RubikDetector rubikDetector, OnRubikCubeFaceletsFoundListener listener, Looper looper) {
        this.rubikDetector = rubikDetector;
        this.resultHandler = new Handler(looper);
        this.listener = listener;
    }

    @Override
    public void onSizeSelected(Size size) {
        rubikDetector.setImageProperties(size.width, size.height, RubikDetector.ImageFormat.YUV_NV21);
        this.imageDataBuffer = ByteBuffer.allocateDirect(rubikDetector.getRequiredMemory());
    }

    @Override
    public void processFrame(final Frame frame) {
        Log.d("FotoapparatResult", "Found cube, notifying listener. rotation:" + frame.rotation);
        imageDataBuffer.rewind();
        imageDataBuffer.put(frame.image, 0, frame.image.length);
        final RubikFacelet[][] result = rubikDetector.findCube(imageDataBuffer.array());
        resultHandler.post(new Runnable() {
            @Override
            public void run() {
                if (listener != null) {
                    listener.onRubikCubeFaceletsFound(wrapResult(result, frame.size, frame.rotation));
                }
            }
        });
    }

    @SuppressWarnings("SuspiciousNameCombination")
    private RubikResultWrapper wrapResult(RubikFacelet[][] result, Size size, int rotation) {
        int width = size.width;
        int height = size.height;

        if (result != null) {

            if (rotation > 360) {
                rotation %= 360;
            }

            switch (rotation) {
                case 0:
                case 360:
                    break;
                case 90:
                    for (int i = 0; i < 3; i++) {
                        for (int j = 0; j < 3; j++) {
                            float aux = result[i][j].center.x;
                            result[i][j].center.x = result[i][j].center.y;
                            result[i][j].center.y = size.width - aux;
                        }
                    }
                    width = size.height;
                    height = size.width;
                    break;
                case 180:
                    for (int i = 0; i < 3; i++) {
                        for (int j = 0; j < 3; j++) {
                            result[i][j].center.x = size.width - result[i][j].center.x;
                            result[i][j].center.y = size.height - result[i][j].center.y;
                        }
                    }
                    break;
                case 270:
                    for (int i = 0; i < 3; i++) {
                        for (int j = 0; j < 3; j++) {
                            float aux = result[i][j].center.x;
                            result[i][j].center.x = size.height - result[i][j].center.y;
                            result[i][j].center.y = aux;
                        }
                    }
                    width = size.height;
                    height = size.width;
                    break;
            }
        }
        return new RubikResultWrapper(width, height, result);
    }

    public interface OnRubikCubeFaceletsFoundListener {
        void onRubikCubeFaceletsFound(RubikResultWrapper rubikResult);
    }
}

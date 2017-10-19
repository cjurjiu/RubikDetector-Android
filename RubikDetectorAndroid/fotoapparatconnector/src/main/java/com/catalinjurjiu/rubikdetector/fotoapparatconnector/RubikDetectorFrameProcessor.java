package com.catalinjurjiu.rubikdetector.fotoapparatconnector;

import android.os.Handler;
import android.os.Looper;
import android.support.annotation.IntDef;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;

import com.catalinjurjiu.rubikdetector.RubikDetector;
import com.catalinjurjiu.rubikdetector.fotoapparatconnector.data.RubikFaceletsWrapper;
import com.catalinjurjiu.rubikdetector.model.RubikFacelet;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

import io.fotoapparat.parameter.Size;
import io.fotoapparat.preview.Frame;
import io.fotoapparat.preview.FrameProcessor;

/**
 * FotoApparat {@link FrameProcessor} implementation which uses a {@link RubikDetector} to search for a Rubik's Cube facelets in the frame received in
 * {@link FrameProcessor#processFrame(Frame)}.
 * <p>
 * The processor notifies its listeners of the detection result of each frame. The listeners will be notified even if facelets were not found in a
 * particular frame. When this happens, the received {@link RubikFaceletsWrapper} will have its {@link RubikFaceletsWrapper#detectedFacelets}
 * set to {@code null}.
 * <p>
 * To be notified when facelets are found in the current frame, register a {@link OnRubikCubeDetectionResultListener} using
 * {@link #addOnRubikCubeFaceletsFoundListener(OnRubikCubeDetectionResultListener)}.
 * <p>
 * By default the listeners will be notified on the main thread. To notify the listeners on a different thread, use
 * {@link RubikDetectorFrameProcessor#RubikDetectorFrameProcessor(RubikDetector, Looper)} when creating the processor instance.
 * <p>
 * This class implements {@link RubikDetectorSizeSelectorWrapper.SizeSelectionListener}, and requires its {@link #onSizeSelected(Size)} method to be
 * called before any call to {@link #processFrame(Frame)} occurs. Not doing so will result in a crash.
 * <p>
 * The call to {@link #onSizeSelected(Size)} is necessary in order for this processor to notify its {@link RubikDetector} of  the size of the frames
 * to be processed. Another reason is that this processor needs to pre allocate certain image buffers before processing begins.  To preallocate buffers
 * of the correct capacity, the size of the frames to be processed needs to be known.
 */
@SuppressWarnings({"unused", "SuspiciousNameCombination"})
public class RubikDetectorFrameProcessor implements FrameProcessor, RubikDetectorSizeSelectorWrapper.SizeSelectionListener {

    private static final String TAG = RubikDetectorFrameProcessor.class.getSimpleName();
    /**
     * Handler used to notify listeners on a specific thread, when a detection occurs.
     */
    private final Handler resultHandler;
    /**
     * Detector instance which will perform the actual detection.
     */
    private final RubikDetector rubikDetector;
    /**
     * Listener list.
     */
    private final List<OnRubikCubeDetectionResultListener> listeners = new ArrayList<>(0);
    /**
     * Buffer pre-allocated to the capacity required by the current RubikDetector. Will be reused across frames.
     */
    private ByteBuffer imageDataBuffer = null;

    /**
     * Create a new instance of a {@link RubikDetectorFrameProcessor}.
     *
     * @param rubikDetector the {@link RubikDetector} used internally for searching the facelets of the Rubik's Cube.
     */
    public RubikDetectorFrameProcessor(@NonNull RubikDetector rubikDetector) {
        this(rubikDetector, Looper.getMainLooper());
    }

    /**
     * Create a new instance of a {@link RubikDetectorFrameProcessor}.
     *
     * @param rubikDetector the {@link RubikDetector} used internally for searching the facelets of the Rubik's Cube.
     * @param looper        a {@link Looper} of the thread on which the listeners need to be notified.
     */
    public RubikDetectorFrameProcessor(@NonNull RubikDetector rubikDetector, @NonNull Looper looper) {
        this.rubikDetector = rubikDetector;
        this.resultHandler = new Handler(looper);
    }

    @Override
    public void onSizeSelected(Size size) {
        rubikDetector.updateImageProperties(size.width, size.height, RubikDetector.ImageFormat.YUV_NV21);
        this.imageDataBuffer = ByteBuffer.allocateDirect(rubikDetector.getRequiredMemory());
    }

    @Override
    public void processFrame(final Frame frame) {
        imageDataBuffer.rewind();
        imageDataBuffer.put(frame.image, 0, frame.image.length);
        final RubikFacelet[][] result = rubikDetector.findCube(imageDataBuffer.array());
        notifyListenersOfResult(wrapResult(result, frame.size, frame.rotation));
    }

    /**
     * Registers a listener which will be notified after each frame with the result of the detection within that frame.
     * <p>
     * The listener will be notified on the thread bound to the looper specified when the processor was created, or on the main thread
     * if no looper was specified.
     * <p>
     * Listeners can be added from any thread.
     *
     * @param listener a {@link OnRubikCubeDetectionResultListener} which will be notified when processing completes for each frame
     */
    public void addOnRubikCubeFaceletsFoundListener(OnRubikCubeDetectionResultListener listener) {
        synchronized (listeners) {
            listeners.add(listener);
        }
    }

    /**
     * Removes a previously added listener. This can be safely called from any thread.
     * <p>
     * Does nothing if the listener to be removed was not previously added.
     *
     * @param listener a listener previously registered with {@link #addOnRubikCubeFaceletsFoundListener(OnRubikCubeDetectionResultListener)},
     *                 which now needs to be removed.
     */
    public void removeOnRubikCubeFaceletsFoundListener(OnRubikCubeDetectionResultListener listener) {
        synchronized (listeners) {
            listeners.remove(listener);
        }
    }

    /**
     * Wraps an array of {@link RubikFacelet} objects into an {@link RubikFaceletsWrapper}.
     * <p>
     * This is necessary because the {@link RubikDetector} performing the detection is unaware of the rotation of the frame. It always
     * assumes detection is performed on a frame in landscape. This causes the coordinates of the {@link RubikFacelet}s returned
     * by the {@link RubikDetector} to have correct coordinates when the device is in landscape, but invalid coordinates when
     * the frame will actually be rendered in portrait, reverse landscape or reverse portrait.
     * <p>
     * This method updates the coordinates of the detected {@link RubikFacelet}s to be suitable for the orientation in which they
     * will be displayed. The wrapped result will also contain the correct dimensions of the frame in which they have been detected, when
     * also considering the rotation.
     *
     * @param result   array of {@link RubikFacelet} found in the current frame. Can be null if no facelets were detected in this frame.
     * @param size     a {@link Size} specifying the dimensions of the current frame.
     * @param rotation the current rotation, in degrees. 0 would be landscape, 90 portrait, 180 reverse landscape, etc..
     * @return a {@link RubikFaceletsWrapper} with the facelets postitions updates to match the current rotation of the screen.
     */
    private RubikFaceletsWrapper wrapResult(@Nullable RubikFacelet[][] result, @NonNull Size size, int rotation) {
        int width = size.width;
        int height = size.height;

        if (result != null) {

            if (rotation > Rotation.DEGREES_360) {
                rotation %= Rotation.DEGREES_360;
            }

            switch (rotation) {
                case Rotation.DEGREES_0:
                case Rotation.DEGREES_360:
                    break;
                case Rotation.DEGREES_90:
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
                case Rotation.DEGREES_180:
                    for (int i = 0; i < 3; i++) {
                        for (int j = 0; j < 3; j++) {
                            result[i][j].center.x = size.width - result[i][j].center.x;
                            result[i][j].center.y = size.height - result[i][j].center.y;
                        }
                    }
                    break;
                case Rotation.DEGREES_270:
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
        return new RubikFaceletsWrapper(result, width, height);
    }

    /**
     * Notifies the registered listeners of a detection, on the thread to which {@link #resultHandler} is bound to.
     *
     * @param rubikFaceletsWrapper the {@link RubikFaceletsWrapper} to deliver to the listeners.
     */
    private void notifyListenersOfResult(final RubikFaceletsWrapper rubikFaceletsWrapper) {
        resultHandler.post(new Runnable() {
            @Override
            public void run() {
                synchronized (listeners) {
                    for (OnRubikCubeDetectionResultListener listener : listeners) {
                        listener.onRubikCubeDetectionResult(rubikFaceletsWrapper);
                    }
                }
            }
        });
    }

    /**
     * Simple intdef to represent possible rotations a frame could take.
     */
    @IntDef
    private @interface Rotation {
        int DEGREES_0 = 0,
                DEGREES_90 = 90,
                DEGREES_180 = 180,
                DEGREES_270 = 270,
                DEGREES_360 = 360;
    }

}

package com.catalinjurjiu.rubikdetector;

import android.support.annotation.IntDef;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.util.Log;

import com.catalinjurjiu.rubikdetector.config.DrawConfig;
import com.catalinjurjiu.rubikdetector.model.Point2d;
import com.catalinjurjiu.rubikdetector.model.RubikFacelet;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.nio.ByteBuffer;

/**
 * <p>
 * The RubikDetector is capable of detecting the face of a Rubik's Cube within an image. If a face is found within the provided image,
 * it will return a list of {@link RubikFacelet}s.
 * <p>
 * The {@link RubikDetector} is capable of drawing a graphical representation of the detected facelets, over the
 * original image. The draw mode of the detector can be controlled through {@link DrawConfig} instances - see:
 * {@link RubikDetector.Builder#drawConfig(DrawConfig)}, {@link RubikDetector#setDrawConfig(DrawConfig)}.
 * <p>
 * To detect the facelets, use {@link #findCube(byte[])} or {@link #findCube(ByteBuffer)}. The parameter passed to a
 * <i>findCube</i> method, regardless whether if it's a byte[] or a ByteBuffer, is considered to be an <i>image data array</i>,
 * and needs to contain the image on which processing will occur (i.e. the <i>input image</i>). The data for this image needs to
 * be stored in a format specified by {@link ImageFormat}.
 * <p>
 * For each processed frame, regardless whether detection succeeds or not, the detector will write back into the original image
 * data array the ready-to-display, RGBA8888 version of the input image. This RGBA8888 version of the input image is regarded as
 * being the <i>output frame</i>, or <i>result frame</i>.
 * <p>
 * Effectively, this means that after a call to one of the {@code findCube(...)} methods, the byte[] or ByteBuffer parameter
 * will contain:
 * <ul>
 * <li>the <i>input frame</i>, data format one of {@link ImageFormat} -  on which detection occurred;</li>
 * <li>the <i>result</i>, or <i>output</i> frame - converted to the {@link ImageFormat#ARGB_8888} format. This can then be easily converted to a {@link android.graphics.Bitmap}, and displayed.</li>
 * </ul>
 * If the set {@link DrawConfig} differs from {@link DrawConfig#DoNotDraw()}, then the <i>output frame</i> will also have the
 * drawn facelets.
 * <p>
 * Since the detector writes the <i>output frame</i> back into the same <i>input data array</i> it receives when {@code findCube(...)} is called,
 * the <i>image data array</i> needs to be large enough to contain both the <i>input frame</i> and the <i>output frame</i>. Additionally, the same
 * <i>image data array</i> passed to {@code findCube(...)} is also used internally to store various processing frames, different from the input/output
 * frames mentioned earlier.
 * <p>
 * This implies that the allocated capacity of the <i>image data array</i> passed to {@code findCube(..)} is crucial for the RubikDetector to work
 * as expected. This capacity depends on the active {@link ImageProperties}. Calling {@link #updateImageProperties(ImageProperties)} might change
 * the memory requirements of the detector.
 * <p>
 * To know how much to allocate for the <i>image data array</i>, use {@link #getRequiredMemory()}. The detector will ignore a call to {@code findCube(...)} with an
 * <i>image data array</i> having a capacity smaller than the value returned by {@link #getRequiredMemory()}.
 * <p>
 * To know where to store the input frame in the image data array, use {@link #getInputFrameBufferOffset()} and {@link #getInputFrameByteCount()}. To know
 * from where to read the output frame, from the image data array, after the call to {@code findCube(...)}, use {@link #getResultFrameBufferOffset()} and
 * {@link #getResultFrameByteCount()}.
 * <p>
 * A highly simplified {@code findCube(...)} usage might look similar to the example, using the {@link android.hardware.Camera} api.
 * <p>
 * <b>Example:</b>
 * <p>
 * First, allocate the buffers of the capacity required by the RubikDetector:
 * <pre>{@code
 *      private void allocateAndSetBuffers() {
 *
 *          //allocate 3 callback buffers, of the size expected by the detector
 *          camera.addCallbackBuffer(ByteBuffer.allocateDirect(rubikDetector.getRequiredMemory()).array());
 *          camera.addCallbackBuffer(ByteBuffer.allocateDirect(rubikDetector.getRequiredMemory()).array());
 *          camera.addCallbackBuffer(ByteBuffer.allocateDirect(rubikDetector.getRequiredMemory()).array());
 *          camera.setPreviewCallbackWithBuffer(myCameraPreviewCallback);
 *      }
 * }</pre>
 * Then, in the camera's {@code onPreviewFrame}:
 * <pre>{@code
 *      @Override
 *      public void onPreviewFrame(byte[] data, Camera camera) {
 *          RubikFacelet[] result = rubikDetector.findCube(data);
 *
 *          //check if the facelets have been found.
 *          if(result != null){
 *              //we have detected facelets!
 *          }
 *
 *          //get the output frame as a bitmap
 *          Bitmap outputFrame = BitmapFactory.decodeByteArray(data,
 *                                                             rubikDetector.getResultFrameBufferOffset(),
 *                                                             rubikDetector.getResultFrameByteCount());
 *
 *          //draw the output frame, irrespective whether it contains facelets or not,
 *          //to update the live preview image on screen
 *          drawToCanvas(resultFrame);
 *
 *          //add the array back as a camera buffer
 *          camera.addCallbackBuffer(data);
 *      }
 * }</pre>
 * See the "LiveDetectionActivity" sample for more details.
 * <p>
 * A point worth nothing is that {@link RubikDetector}  has been optimised for detecting facelets within a live image stream. This implies
 * a trade-off in detection rate, for increased performance. While detection rates are high regardless, this component was written with performance on
 * a mobile device in mind, not to have 100% detection rate.
 * <p>
 * To create instances of this class, use the {@link RubikDetector.Builder}.
 * <p>
 * <b>Important:</b> When you have finished using the RubikDetector, always call {@link #releaseResources()}. This ensures the backing native object, and its
 * resources, are freed.
 * <p>
 * <b>Note:</b> This class is not synchronized! If using multiple threads, it is the client's responsibility to ensure {@code findCube(...)} is not
 * called with outdated values, after a {@code updateImageProperties(...)}.
 *
 * @see #findCube(byte[])
 * @see #findCube(ByteBuffer)
 * @see #updateImageProperties(ImageProperties)
 * @see #releaseResources()
 * @see RubikDetector.Builder
 */
@SuppressWarnings("unused")
public class RubikDetector {

    /**
     * Tag of the class, used when logging.
     */
    private static final String TAG = RubikDetector.class.getSimpleName();
    /**
     * Value of the native cube handle, when the native cube has not yet
     * been bound, or has been released.
     */
    private static final int NATIVE_DETECTOR_RELEASED = -1;
    /**
     * The length of one data element, in the array of elements returned by the native side.
     */
    private static final int DATA_SIZE = 6;
    /**
     * native handle to a native native instance of a RubikProcessor (the c++ equivalent of this class), which
     * this objects wraps over.
     */
    private long nativeProcessorRef = NATIVE_DETECTOR_RELEASED;
    /**
     * The width of the input & output frame
     */
    private int frameWidth;
    /**
     * The height of the input & output frame
     */
    private int frameHeight;
    /**
     * The required memory in bytes, required by the native implementation, for the processing to occur.
     *
     * @see #getRequiredMemory()
     */
    private int requiredMemory;
    /**
     * The expected size of the input frame, in bytes.
     *
     * @see #getInputFrameByteCount()
     */
    private int inputFrameByteCount;
    /**
     * The offset at which the input frame is expected to be in the buffer sent to {@link #findCube(byte[])} or {@link #findCube(ByteBuffer)}.
     *
     * @see #getInputFrameBufferOffset()
     */
    private int inputFrameBufferOffset;
    /**
     * The expected size of the output frame, in bytes.
     *
     * @see #getResultFrameByteCount()
     */
    private int resultFrameByteCount;
    /**
     * The offset at which the output frame is expected to be in the buffer sent to {@link #findCube(byte[])} or {@link #findCube(ByteBuffer)}.
     *
     * @see #getResultFrameBufferOffset()
     */
    private int resultFrameBufferOffset;

    static {
        //load the native code
        System.loadLibrary("rubikdetector_native");
    }

    /**
     * Creates a new RubiKDetector given the properties in the parameters.
     * <p>
     * This object will have its storage path set to null. Hence, it will not be able to save any internal processing frames, even
     * if debuggable.
     *
     * @param properties {@link ImageProperties} to use in the new detector
     * @param drawConfig {@link DrawConfig} to be used.
     */
    private RubikDetector(@NonNull ImageProperties properties, @NonNull DrawConfig drawConfig) {
        this(properties, drawConfig, null);
    }

    /**
     * Creates a new RubiKDetector given the properties in the parameters.
     * <p>
     * If the {@code storagePath} parameter will not be null then this RubikDetector will be able to save internal processing frames,
     * when debuggable.
     *
     * @param properties  {@link ImageProperties} to use in the new detector
     * @param drawConfig  {@link DrawConfig} to be used.
     * @param storagePath a {@link String} string representing a file path to a writable location of persistent storage. If {@code nonnull},
     *                    this RubikDetector will be capable of saving internal processing frames, when it is in debug mode.
     *                    This has no effect when debug mode is off.
     */
    private RubikDetector(@NonNull ImageProperties properties, @NonNull DrawConfig drawConfig, @Nullable String storagePath) {
        this.nativeProcessorRef = createNativeDetector(properties, drawConfig, storagePath);
        this.frameWidth = properties.width;
        this.frameHeight = properties.height;
        syncWithNativeObject();
    }

    /**
     * Updates the active {@link ImageProperties}. This triggers a synchronous complete recalculation of the memory requirements, which implies
     * that the following methods might return different values, after this call:
     * <ul>
     * <li>{@link #getRequiredMemory()}</li>
     * <li>{@link #getInputFrameByteCount()}</li>
     * <li>{@link #getInputFrameBufferOffset()}</li>
     * <li>{@link #getResultFrameByteCount()}</li>
     * <li>{@link #getResultFrameBufferOffset()}</li>
     * </ul>
     * <p>
     * Following this call, make sure {@link #findCube(byte[])}(or {@link #findCube(ByteBuffer)} is not called with a buffer allocated for the previously
     * set properties.
     *
     * @param properties updated {@link ImageProperties}
     */
    public void updateImageProperties(ImageProperties properties) {
        applyImageProperties(properties.width, properties.height, properties.inputImageFormat);
    }

    /**
     * Updates the active {@link ImageProperties}. This triggers a synchronous complete recalculation of the memory requirements, which implies
     * that the following methods might return different values, after this call:
     * <ul>
     * <li>{@link #getRequiredMemory()}</li>
     * <li>{@link #getInputFrameByteCount()}</li>
     * <li>{@link #getInputFrameBufferOffset()}</li>
     * <li>{@link #getResultFrameByteCount()}</li>
     * <li>{@link #getResultFrameBufferOffset()}</li>
     * </ul>
     * <p>
     * Following this call, make sure {@link #findCube(byte[])}(or {@link #findCube(ByteBuffer)} is not called with a buffer allocated for the previously
     * set properties.
     *
     * @param width       width of the new input & output frames, in pixels
     * @param height      height of the new input & output frames, in pixels
     * @param imageFormat width of the new input & output frames, in pixels
     */
    public void updateImageProperties(int width, int height, @ImageFormat int imageFormat) {
        applyImageProperties(width, height, imageFormat);
    }

    /**
     * Sets the debuggable mode of this component on/off.
     *
     * @param debuggable {@code true} to enable debug mode, {@code false} to disable it.
     */
    public void setDebuggable(boolean debuggable) {
        if (isActive()) {
            nativeSetDebuggable(nativeProcessorRef, debuggable);
        }
    }

    /**
     * Releases the native resources associated with this detector. Always call this when the {@link RubikDetector} is no longer needed.
     * <p>
     * Make sure to not call this while {@link #findCube(byte[])} or {@link #findCube(ByteBuffer)} are executing.
     * <p>
     * Following this call, future calls to this {@link RubikDetector}'s methods will have no effect, and the {@link RubikDetector} will
     * be unusable.
     */
    public void releaseResources() {
        Log.d(TAG, "#releaseResources. this:" + hashCode());
        if (isActive()) {
            Log.d(TAG, "#releaseResources - handle not -1, calling native release.");
            nativeReleaseCubeDetector(nativeProcessorRef);
            nativeProcessorRef = NATIVE_DETECTOR_RELEASED;
        }
    }

    /**
     * Performs a synchronous search for a Rubik's Cube face. If a face is found, then its facelets are returned as a 3x3 array of
     * {@link RubikFacelet} objects.
     * <p>
     * Given the <b>in/out</b> byte data array parameter, expects the input image data to be at offset {@link #getInputFrameBufferOffset()}, and to have
     * a size equal to {@link #getInputFrameByteCount()}. The input image data is considered to be stored in the {@link ImageFormat} specified
     * through the currently set {@link ImageProperties}.
     * <p>
     * Regardless whether a Rubik's Cube face is found or not, following this call the byte data array received as parameter will contain an
     * additional image, called the <i>output frame</i>. This output frame is the {@link ImageFormat#ARGB_8888} format, ready-to-display, version of
     * the input frame. The output frame will always have the same resolution as the input frame.
     * <p>
     * Additionally, if a draw mode other than {@link DrawConfig.DrawMode#DO_NOT_DRAW} is active, and facelets are found, the output frame
     * will also contain the drawn facelets.
     * <p>
     * The output frame will be written at offset {@link #getResultFrameBufferOffset()} in the byte array parameter. It will have a length equal to
     * {@link #getResultFrameByteCount()}.
     * <p>
     * For example, one could read the output frame in the following way:
     * <pre>{@code
     *          RubikFacelet[] result = rubikDetector.findCube(data);
     *
     *          //check if the facelets have been found.
     *          if(result != null){
     *              //we have detected facelets!
     *          }
     *
     *          //get the output frame as a bitmap
     *          Bitmap outputFrame = BitmapFactory.decodeByteArray(data,
     *                                                             rubikDetector.getResultFrameBufferOffset(),
     *                                                             rubikDetector.getResultFrameByteCount());
     * }</pre>
     * <p>
     * Ideally the byte[] parameter would be the backing array of a {@link ByteBuffer} allocated with {@link ByteBuffer#allocateDirect(int)}.
     *
     * @param imageData byte array used as an <b>in/out</b> parameter. This needs to contain the input frame when calling this method. After
     *                  the method call, it will additionally contain the output frame.
     * @return a 3x3 {@link RubikFacelet} array, representing the found facelets, if a Rubik's Cube is detected in the current frame,
     * or {@code null} if a cube is not detected.
     */
    public RubikFacelet[][] findCube(@NonNull byte[] imageData) {
        if (isActive() && imageData.length >= requiredMemory) {
            int[] nativeResult = nativeFindCube(nativeProcessorRef, imageData);
            return decodeResult(nativeResult);
        }
        return null;
    }

    /**
     * Performs a synchronous search for a Rubik's Cube face. If a face is found, then its facelets are returned as a 3x3 array of
     * {@link RubikFacelet} objects.
     * <p>
     * Given the <b>in/out</b> {@link ByteBuffer} array parameter, expects the input image data to be at offset {@link #getInputFrameBufferOffset()} in
     * its backing byte array. It expects the input data to have a size equal to {@link #getInputFrameByteCount()}. The input image data is considered
     * to be stored in the {@link ImageFormat} specified through the currently set {@link ImageProperties}.
     * <p>
     * Regardless whether a Rubik's Cube face is found or not, following this call the ByteBuffer received as parameter will contain an
     * additional image, called the <i>output frame</i>. This output frame is the {@link ImageFormat#ARGB_8888} format, ready-to-display, version of
     * the input frame. The output frame will always have the same resolution as the input frame.
     * <p>
     * Additionally, if a draw mode other than {@link DrawConfig.DrawMode#DO_NOT_DRAW} is active, and facelets are found, the output frame
     * will also contain the drawn facelets.
     * <p>
     * The output frame will be written at offset {@link #getResultFrameBufferOffset()} in the buffer parameter's backing array. It will have a length equal
     * to {@link #getResultFrameByteCount()}.
     * <p>
     * For example, one could read the output frame in the following way:
     * <pre>{@code
     *          RubikFacelet[] result = rubikDetector.findCube(data);
     *
     *          //check if the facelets have been found.
     *          if(result != null){
     *              //we have detected facelets!
     *          }
     *
     *          //get the output frame as a bitmap
     *          Bitmap outputFrame = BitmapFactory.decodeByteArray(data.array(),
     *                                                             rubikDetector.getResultFrameBufferOffset(),
     *                                                             rubikDetector.getResultFrameByteCount());
     * }</pre>
     * <p>
     * The {@link ByteBuffer} parameter needs to be allocated with {@link ByteBuffer#allocateDirect(int)}.
     *
     * @param imageDataBuffer {@link ByteBuffer} used as an <b>in/out</b> parameter. This needs to contain the input frame when calling this method. After
     *                        the method call, it will additionally contain the output frame.
     * @return a 3x3 {@link RubikFacelet} array, representing the found facelets, if a Rubik's Cube is detected in the current frame,
     * or {@code null} if a cube is not detected.
     */
    public RubikFacelet[][] findCube(@NonNull ByteBuffer imageDataBuffer) {
        if (!imageDataBuffer.isDirect()) {
            throw new IllegalArgumentException("The image data buffer needs to be a direct buffer.");
        }
        if (isActive() && imageDataBuffer.capacity() >= requiredMemory) {
            int[] nativeResult = nativeFindCubeDataBuffer(nativeProcessorRef, imageDataBuffer);
            return decodeResult(nativeResult);
        }
        return null;
    }

    /**
     * Checks whether the detector is capable of reacting to requests and searching for facelets.
     * <p>
     * If inactive, it could mean two things:
     * <ul>
     * <li>{@link #releaseResources()} has been called;</li>
     * <li>an error occurred during initialization.</li>
     * </ul>
     * <p>
     * Several methods have no effect if called while the detector is inactive (i.e. {@link #findCube(byte[])}, {@link #updateImageProperties(ImageProperties)}, etc).
     *
     * @return {@code true} if the detector is active, {@code false} otherwise.
     */
    public boolean isActive() {
        return nativeProcessorRef != NATIVE_DETECTOR_RELEASED;
    }

    /**
     * Returns the required memory capacity of the byte array or {@link ByteBuffer} passed to one of the {@code findCube(...)} methods.
     * <p>
     * This value is recomputed with each call to {@link #updateImageProperties(ImageProperties)} or {@link #updateImageProperties(int, int, int)}
     *
     * @return a integer representing the required memory when searching for a cube, in a given frame.
     * @see RubikDetector
     * @see RubikDetector#findCube(byte[])
     * @see RubikDetector#updateImageProperties(ImageProperties)
     */
    public int getRequiredMemory() {
        return requiredMemory;
    }

    /**
     * Given a byte array which contains, among others, the data for the input image (on which detection will be performed),
     * this returns the offset at which the input image data is stored within that byte array.
     * <p>
     * This value is recomputed with each call to {@link #updateImageProperties(ImageProperties)} or {@link #updateImageProperties(int, int, int)}
     *
     * @return the offset, in bytes, at which the input image data starts, within its byte array container.
     * @see RubikDetector
     * @see RubikDetector#findCube(byte[])
     * @see RubikDetector#updateImageProperties(ImageProperties)
     */
    public int getInputFrameBufferOffset() {
        return inputFrameBufferOffset;
    }

    /**
     * Gives the size of the input image data, in bytes.
     * <p>
     * This value is recomputed with each call to {@link #updateImageProperties(ImageProperties)} or {@link #updateImageProperties(int, int, int)}
     *
     * @return the size, in bytes, of the input image data.
     * @see RubikDetector
     * @see RubikDetector#findCube(byte[])
     * @see RubikDetector#updateImageProperties(ImageProperties)
     */
    public int getInputFrameByteCount() {
        return inputFrameByteCount;
    }

    /**
     * Given a byte array which contains, among others, the output image data, this returns the offset at which the output image data
     * is stored within that byte array.
     * <p>
     * This value is recomputed with each call to {@link #updateImageProperties(ImageProperties)} or {@link #updateImageProperties(int, int, int)}
     *
     * @return the offset, in bytes, at which the output image data starts, within its byte array container.
     * @see RubikDetector
     * @see RubikDetector#findCube(byte[])
     * @see RubikDetector#updateImageProperties(ImageProperties)
     */
    public int getResultFrameBufferOffset() {
        return resultFrameBufferOffset;
    }

    /**
     * Gives the size of the output image data, in bytes.
     * <p>
     * This value is recomputed with each call to {@link #updateImageProperties(ImageProperties)} or {@link #updateImageProperties(int, int, int)}
     *
     * @return the size, in bytes, of the output image data.
     * @see RubikDetector
     * @see RubikDetector#findCube(byte[])
     * @see RubikDetector#updateImageProperties(ImageProperties)
     */
    public int getResultFrameByteCount() {
        return resultFrameByteCount;
    }

    /**
     * Given the byte array parameter, which contains both the input & output frames, this method overrides the
     * data at the input frame data with the data stored in the array as the output frame.
     *
     * @param data an array which contains both frames.
     */
    public void overrideInputFrameWithResultFrame(byte[] data) {
        if (isActive()) {
            nativeOverrideInputFrameWithResultFrame(nativeProcessorRef, data);
        }
    }

    /**
     * Returns the frame's width, in pixels.
     *
     * @return width of the frame, in pixels.
     */
    public int getFrameWidth() {
        return frameWidth;
    }

    /**
     * Returns the frame's height, in pixels.
     *
     * @return height of the frame, in pixels.
     */
    public int getFrameHeight() {
        return frameHeight;
    }

    /**
     * Updates the {@link DrawConfig} of the current detector.
     *
     * @param drawConfig the new {@link DrawConfig}
     */
    public void setDrawConfig(DrawConfig drawConfig) {
        if (isActive()) {
            nativeSetDrawConfig(nativeProcessorRef,
                    drawConfig.getDrawMode(),
                    drawConfig.getStrokeWidth(),
                    drawConfig.isFillShape());
        }
    }

    /**
     * Creates the native detector object, configured for the given parameters, and returns an identifier to the created native object.
     *
     * @param properties  {@link ImageProperties} object which specifies the frame resolution and format
     * @param drawConfig  {@link DrawConfig} instance which specifies how to draw the facelets, if found
     * @param storagePath string to a persistent storage location, with write access.
     * @return a long int, representing an identifier which can be used to reference the same instance of the native detector
     * created here, in future actions.
     */
    private long createNativeDetector(ImageProperties properties, DrawConfig drawConfig, String storagePath) {

        return nativeCreateRubikDetector(properties.width,
                properties.height,
                properties.inputImageFormat,
                drawConfig.getDrawMode(),
                drawConfig.getStrokeWidth(),
                drawConfig.isFillShape(),
                storagePath);
    }

    /**
     * Applies the new image properties on the native detector, then updates the memory requirements known by this Java object,
     * with the new values of the native object.
     *
     * @param width       new frame width, in pixels
     * @param height      new frame height, in pixels
     * @param imageFormat an {@link ImageFormat} representing the format in which the input frame data is represented.
     */
    private void applyImageProperties(int width, int height, @ImageFormat int imageFormat) {
        if (isActive()) {
            nativeSetImageProperties(nativeProcessorRef, width, height, imageFormat);
            this.frameWidth = width;
            this.frameHeight = height;
            syncWithNativeObject();
        }
    }

    /**
     * Updates what this java object knows about the memory requirements of the c++ object is wraps.
     */
    private void syncWithNativeObject() {
        if (isActive()) {
            this.requiredMemory = nativeGetRequiredMemory(nativeProcessorRef);
            this.resultFrameBufferOffset = nativeGetResultImageOffset(nativeProcessorRef);
            this.resultFrameByteCount = nativeGetResultImageSize(nativeProcessorRef);
            this.inputFrameByteCount = nativeGetInputImageSize(nativeProcessorRef);
            this.inputFrameBufferOffset = nativeGetInputImageOffset(nativeProcessorRef);
        }
    }

    /**
     * Parses the int array returned by the native object when the facelets are found, and returns a 3x3 array of {@link RubikFacelet} instances,
     * representing the detected facelets.
     *
     * @param detectionResult an int[] which contains marshaled data, regarding the found facelets. typically returned by the c++ detector
     * @return a 3x3 {@link RubikFacelet}[][], representing the parsed data in the int[] parameter.
     */
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

    /*
     * #############################################################################################
     * #############################################################################################
     * ##################################     NATIVE METHODS     ###################################
     * #############################################################################################
     * #############################################################################################
     */

    /**
     * Creates the native detector object for the given parameters. Returns a long which represents
     * an identifier for the created native detector instance.
     * <p>
     * This identifier can be used in future native calls to ensure the actions are performed with/on the
     * native object instance created here.
     *
     * @param frameWidth       the width of the input & output frames, in pixels
     * @param frameHeight      the height of the input & output frames, in pixels
     * @param inputImageFormat {@link ImageFormat} representing the format in which the binary image data is represented.
     * @param drawMode         {@link DrawConfig.DrawMode} which controls how the facelets are drawn, if found
     * @param strokeWidth      the stroke width, in pixels, used when drawing the found facelets.
     * @param fillShape        {@code true} if the facelets need to be drawn as filled shapes, {@code false} otherwise.
     * @param storagePath      {@link String} representing an absolute path to a writable, persistent storage location.
     * @return a long which represents an identifier which can be later used to reference the native object created here.
     */
    private native long nativeCreateRubikDetector(int frameWidth,
                                                  int frameHeight,
                                                  @ImageFormat int inputImageFormat,
                                                  @DrawConfig.DrawMode int drawMode,
                                                  int strokeWidth,
                                                  boolean fillShape,
                                                  @Nullable String storagePath);

    /**
     * Sets the debuggable mode on/off of the native RubikProcessor identified by the {@code nativeProcessorRef} parameter.
     *
     * @param nativeProcessorRef a long identifier which represents a native RubikProcessor.
     * @param debuggable         {@code true} to enable debug mode on the native object, {@code false} otherwise
     */
    private native void nativeSetDebuggable(long nativeProcessorRef, boolean debuggable);

    /**
     * Searches for Rubik's Cube facelets in the frame stored in the byte[] parameter.
     * <p>
     * Returns a marshaled int[] containing the found facelets, if the search is successful, or {@code null}
     * if nothing is found. The int[] result can be decoded with {@link #decodeResult(int[])}.
     *
     * @param nativeProcessorRef a long identifier which represents a native RubikProcessor.
     * @param imageData          byte[] which contains the input frame data, and has a length equal to the value returned by {@link #getRequiredMemory()}.
     * @return an int[] of marshaled data, representing the found facelets, or {@code null} if nothing is found.
     */
    private native int[] nativeFindCube(long nativeProcessorRef, byte[] imageData);

    /**
     * Searches for Rubik's Cube facelets in the frame stored in the {@link ByteBuffer} parameter.
     * <p>
     * Returns a marshaled int[] containing the found facelets, if the search is successful, or {@code null}
     * if nothing is found. The int[] result can be decoded with {@link #decodeResult(int[])}.
     *
     * @param nativeProcessorRef a long identifier which represents a native RubikProcessor.
     * @param imageDataBuffer    {@link ByteBuffer} allocated in native memory, which contains the input frame data, and a capacity equal
     *                           to the value returned by {@link #getRequiredMemory()}.
     * @return an int[] of marshaled data, representing the found facelets, or {@code null} if nothing is found.
     */
    private native int[] nativeFindCubeDataBuffer(long nativeProcessorRef, ByteBuffer imageDataBuffer);

    /**
     * Overrides the bytes which contain the input frame data, with the data associated with the output frame.
     *
     * @param nativeProcessorRef a long identifier which represents a native RubikProcessor.
     * @param data               byte[] which contains the input frame data, and has a length equal to the value returned by {@link #getRequiredMemory()}.
     */
    private native void nativeOverrideInputFrameWithResultFrame(long nativeProcessorRef, byte[] data);

    /**
     * Frees the native memory associated with the native RubikProcessor instance identified by the {@code nativeProcessorRef}.
     *
     * @param nativeProcessorRef long identifier which represents a native RubikProcessor.
     */
    private native void nativeReleaseCubeDetector(long nativeProcessorRef);

    /**
     * Updates the active image properties on the native object. This causes the native object to recompute its memory requirements.
     *
     * @param nativeProcessorRef long identifier which represents a native RubikProcessor.
     * @param width              new width, in pixels
     * @param height             new height, in pixels
     * @param imageFormat        new image format, as a {@link ImageFormat}
     */
    private native void nativeSetImageProperties(long nativeProcessorRef, int width, int height, @ImageFormat int imageFormat);

    /**
     * Updates the active draw config on the native object.
     *
     * @param nativeProcessorRef long identifier which represents a native RubikProcessor.
     * @param drawMode           the new draw mode
     * @param strokeWidth        the shape stroke width
     * @param fillShape          {@code true} to fill the shape, {@code false} otherwise
     */
    private native void nativeSetDrawConfig(long nativeProcessorRef, @DrawConfig.DrawMode int drawMode, int strokeWidth, boolean fillShape);

    /**
     * Reads the memory required by the native RubikProcessor identified with {#code nativeProcessorRef}.
     *
     * @param nativeProcessorRef long identifier which represents a native RubikProcessor.
     * @return the required memory, in bytes
     */
    private native int nativeGetRequiredMemory(long nativeProcessorRef);

    /**
     * Reads from the native RubikProcessor the expected image size of the output image.
     *
     * @param nativeProcessorRef long identifier which represents a native RubikProcessor.
     * @return the expected size
     */
    private native int nativeGetResultImageSize(long nativeProcessorRef);

    /**
     * Reads the offset at which the output image will be written into, in the byte array or buffer passed to one of the findCube methods.
     *
     * @param nativeProcessorRef long identifier which represents a native RubikProcessor.
     * @return the offset of the output frame, in bytes
     */
    private native int nativeGetResultImageOffset(long nativeProcessorRef);

    /**
     * Reads from the native RubikProcessor the expected image size of the input image.
     *
     * @param nativeProcessorRef long identifier which represents a native RubikProcessor.
     * @return the expected size
     */
    private native int nativeGetInputImageSize(long nativeProcessorRef);

    /**
     * Reads the offset at which the input image will be written into, in the byte array or buffer passed to one of the findCube methods.
     *
     * @param nativeProcessorRef long identifier which represents a native RubikProcessor.
     * @return the offset of the input frame, in bytes
     */
    private native int nativeGetInputImageOffset(long nativeProcessorRef);

    /*
     * #############################################################################################
     * #############################################################################################
     * ################################     END NATIVE METHODS     #################################
     * #############################################################################################
     * #############################################################################################
     */

    /**
     * Represents the supported image formats. These are:
     * <ul>
     * <li>{@link ImageFormat#YUV_NV21}</li>
     * <li>{@link ImageFormat#YUV_NV12}</li>
     * <li>{@link ImageFormat#YUV_I420}</li>
     * <li>{@link ImageFormat#YUV_YV12}</li>
     * <li>{@link ImageFormat#ARGB_8888}</li>
     * </ul>
     */
    @IntDef
    @Retention(RetentionPolicy.SOURCE)
    public @interface ImageFormat {
        /**
         * YCrCb format used for images, which uses the NV21 encoding format.
         */
        int YUV_NV21 = 0,
        /**
         * YCrCb format used for images, which uses the NV12 encoding format.
         */
        YUV_NV12 = 1,
        /**
         * YCrCb format used for images, which uses the YUV_I420 encoding format.
         */
        YUV_I420 = 2,
        /**
         * YCrCb format used for images, which uses the YUV_YV12 encoding format.
         */
        YUV_YV12 = 3,
        /**
         * Generic RGBA format, with 8 bits per color sample.
         */
        ARGB_8888 = 4;
    }

    /**
     * Immutable class which specifies the properties of the input & output frames processed by the {@link RubikDetector}.
     * <p>
     * The properties declared by this class are the following:
     * - input & output frames size (width & height)
     * - input frame format, as a {@link RubikDetector.ImageFormat}
     * - output frame format, as a {@link RubikDetector.ImageFormat}
     * <p>
     * Currently only the input frame format can be specified, since at the time being the only supported output frame format is
     * as a {@link RubikDetector.ImageFormat#ARGB_8888}.
     */
    public static class ImageProperties {

        /**
         * Width of both the input & output frames, in pixels.
         */
        public final int width;

        /**
         * Height of both the input & output frames, in pixels.
         */
        public final int height;

        /**
         * Image format of the input frame.
         */
        @ImageFormat
        public final int inputImageFormat;

        /**
         * Image format of the output frame.
         */
        @ImageFormat
        public final int outputImageFormat;

        /**
         * Creates a new immutable ImageProperties object with the properties mentioned in the parameters, and with the output frame format set to {@link RubikDetector.ImageFormat#ARGB_8888}.
         *
         * @param width            input & output frame width, in pixels
         * @param height           input & output frame width, in pixels
         * @param inputImageFormat - the format of the input frame, as a {@link RubikDetector.ImageFormat}.
         */
        public ImageProperties(int width, int height, int inputImageFormat) {
            this.width = width;
            this.height = height;
            this.inputImageFormat = inputImageFormat;
            this.outputImageFormat = ImageFormat.ARGB_8888;
        }
    }

    /**
     * Builder class for the {@link RubikDetector}. A new instance of {@link RubikDetector} can be created just with:
     * <pre>
     * RubikDetector rubikDetector = new RubikDetector.Builder().build();
     * </pre>
     * This creates a RubikProcessor with the following defaults:
     * <ul>
     * <li>input frame size: 320 x 240;</li>
     * <li>input frame format: {@link ImageFormat#YUV_NV21};</li>
     * <li>{@link DrawConfig}: {@link DrawConfig#Default()}</li>
     * <li>imageSavePath: null</li>
     * <li>debuggable: false</li>
     * </ul>
     * <p>
     * See various methods customizing the properties before building the desired {@link RubikDetector}.
     */
    public static class Builder {
        private DrawConfig drawConfig;
        private boolean debuggable;
        private String imageSavePath = null;
        private int inputFrameWidth = 320;
        private int inputFrameHeight = 240;
        @ImageFormat
        private int inputFrameFormat = ImageFormat.YUV_NV21;

        /**
         * <p>
         * Set the resolution of the input & output frames.
         * </p>
         * <p>
         * This can later be changed through RubikProcessor::updateImageProperties().
         * </p>
         *
         * @param width  in pixels
         * @param height in pixels
         * @return this {@link RubikDetector.Builder} instance to allow chaining.
         */
        public Builder inputFrameSize(int width, int height) {
            this.inputFrameWidth = width;
            this.inputFrameHeight = height;
            return this;
        }

        /**
         * <p>
         * Defines the image format used by the input frame.
         * </p>
         * <p>
         * This can later be changed through {@link RubikDetector#updateImageProperties(ImageProperties)}.
         * </p>
         *
         * @param inputFrameFormat a {@link RubikDetector.ImageFormat} representing the format in which the input frame is stored.
         * @return this {@link RubikDetector.Builder} instance to allow chaining.
         */
        public Builder inputFrameFormat(@ImageFormat int inputFrameFormat) {
            this.inputFrameFormat = inputFrameFormat;
            return this;
        }

        /**
         * <p>
         * Specifies the {@link DrawConfig}. Drawing only occurs when the facelets are found. If you wish to not draw even
         * if the facelets are found, use {@link DrawConfig#DoNotDraw()}.
         * </p>
         * <p>
         * This can later be changed through {@link RubikDetector#setDrawConfig(DrawConfig)}.
         * </p>
         *
         * @param drawConfig the desired {@link DrawConfig}
         * @return this {@link RubikDetector.Builder} instance to allow chaining.
         */
        public Builder drawConfig(DrawConfig drawConfig) {
            this.drawConfig = drawConfig;
            return this;
        }

        /**
         * <p>
         * Pass true to enable logging various debugging information, false otherwise.
         * </p>
         * <p>
         * Default is false. This can be updated through RubikProcessor::setDebuggable()
         * </p>
         *
         * @param debuggable {@code true} to set debugging on, {@code false} otherwise.
         * @return this {@link RubikDetector.Builder} instance to allow chaining.
         */
        public Builder debuggable(boolean debuggable) {
            this.debuggable = debuggable;
            return this;
        }

        /**
         * If this is {@code nonnull}, then the {@link RubikDetector} will be able to save internal processing frames,
         * when debuggable.
         *
         * @param imageSavePath a {@link String} string representing a file path to a writable location of persistent storage.
         * @return this {@link RubikDetector.Builder} instance to allow chaining.
         */
        public Builder imageSavePath(@Nullable String imageSavePath) {
            this.imageSavePath = imageSavePath;
            return this;
        }

        /**
         * Builds a {@link RubikDetector} given the properties specified by this builder instance. Any unset properties will take their default values.
         *
         * @return a {@link RubikDetector} instance, ready to be used.
         * @see RubikDetector.Builder
         */
        public RubikDetector build() {
            ImageProperties imageProperties = new ImageProperties(inputFrameWidth, inputFrameHeight, inputFrameFormat);
            if (drawConfig == null) {
                drawConfig = DrawConfig.Default();
            }
            RubikDetector rubikDetector = new RubikDetector(imageProperties, drawConfig, imageSavePath);
            rubikDetector.setDebuggable(debuggable);
            return rubikDetector;
        }
    }
}

package com.catalinjurjiu.rubikdetectorfotoapparatconnector;

import android.support.annotation.NonNull;
import android.support.annotation.Nullable;

import com.catalinjurjiu.rubikdetector.RubikDetector;
import com.catalinjurjiu.rubikdetector.config.DrawConfig;
import com.catalinjurjiu.rubikdetectorfotoapparatconnector.view.RubikDetectorResultView;

import java.util.Collection;

import io.fotoapparat.Fotoapparat;
import io.fotoapparat.FotoapparatBuilder;
import io.fotoapparat.parameter.Size;
import io.fotoapparat.parameter.selector.SelectorFunction;
import io.fotoapparat.parameter.selector.SizeSelectors;
import io.fotoapparat.preview.FrameProcessor;
import io.fotoapparat.view.CameraView;

/**
 * The methods of this connector configure an existing {@link FotoapparatBuilder} to enable the resulting {@link Fotoapparat}
 * instance to use a {@link RubikDetector}. The resulting {@link Fotoapparat} will be capable of detecting the Rubik's Cube facelets
 * in the preview frames from the camera, and will be capable of drawing on screen the detected facelets, through a {@link RubikDetectorResultView}.
 * <p>
 * At a minimum, the connector requires 3 components:
 * <ul>
 * <li>a {@link FotoapparatBuilder} - which will be enhanced with the capabilities mentioned above;</li>
 * <li>a configured {@link RubikDetector} - used to perform the detection;</li>
 * <li>a {@link RubikDetectorResultView} - used to display the detected facelets.</li>
 * </ul>
 * <p>
 * When using the {@link FotoApparatConnector}, the {@link DrawConfig} set on the {@link RubikDetector} is ignored, and is recommended to be set to
 * {@link DrawConfig#DoNotDraw()}. Instead, specify the desired draw mode on the {@link RubikDetectorResultView}, through
 * {@link RubikDetectorResultView#setDrawConfig(DrawConfig)}, or through XML.
 * <p>
 * Besides the components mentioned above, which are mandatory, a {@link OnRubikCubeDetectionResultListener} and a custom
 * {@link SelectorFunction}<{@link Collection}<{@link Size}>, {@link Size}> can be specified. The listener will be notified on the main
 * thread whenever the processor has finished processing a frame. The {@link SelectorFunction}<{@link Collection}<{@link Size}>, {@link Size}> allows
 * you to specify a custom size selector function, which will be used instead of the default {@link SizeSelectors#biggestSize()} selector.
 * <p>
 * Concretely, the methods of this connector set the following components on the {@link FotoapparatBuilder}:
 * <ul>
 * <li>a {@link RubikDetectorFrameProcessor} as the active {@link FrameProcessor};</li>
 * <li>a {@link RubikDetectorSizeSelectorWrapper} as the size selection function;</li>
 * <li>the {@link CameraView} of a {@link RubikDetectorResultView}, as the camera view.</li>
 * </ul>
 * If the {@link FrameProcessor}, {@link CameraView} or {@link SelectorFunction} are already set on the {@link FotoapparatBuilder},
 * they will be overwritten by the connector methods.
 * <p>
 * The {@link RubikDetectorSizeSelectorWrapper} in particular is designed to wrap an already existing size selection function. This
 * means it will wrap a custom size selection function, if one is provided. By default, it wraps the {@link SizeSelectors#biggestSize()}
 * selector.
 * <p>
 * While this description had to be long to explain in detail what this connector does, it doesn't actually do <i>that</i> much. It just ensures
 * the right connections are set to enable a {@link Fotoapparat} instance to talk to a {@link RubikDetector} instance. Another option would be
 * to configure the {@link FotoapparatBuilder} yourself.
 *
 * @see RubikDetectorResultView
 * @see RubikDetectorFrameProcessor
 * @see RubikDetectorSizeSelectorWrapper
 * @see OnRubikCubeDetectionResultListener
 * @see RubikDetector
 */
@SuppressWarnings({"unused", "WeakerAccess"})
public class FotoApparatConnector {

    /**
     * Configures a {@link FotoapparatBuilder} to communicate with a {@link RubikDetector}, to detect Rubik's Cube facelets in the preview frames,
     * and to draw the facelets on the device's screen.
     * <p>
     * <b>Note:</b> If a {@link FrameProcessor}, {@link CameraView} or {@link SelectorFunction} are already set on the given {@link FotoapparatBuilder},
     * they will be overwritten by the this method. The selector function will be set to: {@link SizeSelectors#biggestSize()}.
     * <p>
     * <b>Note:</b> See the documentation of the {@link FotoApparatConnector} for more details about side effects and inner workings.
     *
     * @param fotoApparatBuilder      the {@link FotoapparatBuilder} to configure.
     * @param rubikDetector           the {@link RubikDetector} instance which will perform the detection.
     * @param rubikDetectorResultView the {@link RubikDetectorResultView} which will draw the camera preview and detected facelets on screen.
     * @return a {@link FotoapparatBuilder} configured to detect Rubik's Cube facelets and to draw them on screen.
     * @see FotoApparatConnector
     */
    @NonNull
    public static FotoapparatBuilder configure(@NonNull FotoapparatBuilder fotoApparatBuilder,
                                               @NonNull RubikDetector rubikDetector,
                                               @NonNull RubikDetectorResultView rubikDetectorResultView) {

        return configure(fotoApparatBuilder, rubikDetector, rubikDetectorResultView, SizeSelectors.biggestSize());
    }

    /**
     * Configures a {@link FotoapparatBuilder} to communicate with a {@link RubikDetector}, to detect Rubik's Cube facelets in the preview frames,
     * and to draw the facelets on the device's screen.
     * <p>
     * This method allows you to specify the desired {@link SelectorFunction}<{@link Collection}<{@link Size}>, {@link Size}>. This selector function
     * will be used when choosing the preview frame size, instead of the default {@link SizeSelectors#biggestSize()}.
     * <p>
     * <b>Note:</b> If a {@link FrameProcessor}, {@link CameraView} or {@link SelectorFunction} are already set on the given {@link FotoapparatBuilder},
     * they will be overwritten by the this method.
     * <p>
     * <b>Note:</b> See the documentation of the {@link FotoApparatConnector} for more details about side effects and inner workings.
     *
     * @param fotoApparatBuilder      the {@link FotoapparatBuilder} to configure.
     * @param rubikDetector           the {@link RubikDetector} instance which will perform the detection.
     * @param rubikDetectorResultView the {@link RubikDetectorResultView} which will draw the camera preview and detected facelets on screen.
     * @param sizeSelectorFunction    the desired {@link SelectorFunction}<{@link Collection}<{@link Size}>, {@link Size}> to be used.
     * @return a {@link FotoapparatBuilder} configured to detect Rubik's Cube facelets and to draw them on screen.
     * @see FotoApparatConnector
     */
    @NonNull
    public static FotoapparatBuilder configure(@NonNull FotoapparatBuilder fotoApparatBuilder,
                                               @NonNull RubikDetector rubikDetector,
                                               @NonNull RubikDetectorResultView rubikDetectorResultView,
                                               @Nullable SelectorFunction<Collection<Size>, Size> sizeSelectorFunction) {

        return configure(fotoApparatBuilder, rubikDetector, rubikDetectorResultView, sizeSelectorFunction, null);
    }

    /**
     * Configures a {@link FotoapparatBuilder} to communicate with a {@link RubikDetector}, to detect Rubik's Cube facelets in the preview frames,
     * and to draw the facelets on the device's screen.
     * <p>
     * <b>Note:</b> If a {@link FrameProcessor}, {@link CameraView} or {@link SelectorFunction} are already set on the given {@link FotoapparatBuilder},
     * they will be overwritten by the this method.
     * <p>
     * <b>Note:</b> See the documentation of the {@link FotoApparatConnector} for more details about side effects and inner workings.
     *
     * @param fotoApparatBuilder      the {@link FotoapparatBuilder} to configure.
     * @param rubikDetector           the {@link RubikDetector} instance which will perform the detection.
     * @param rubikDetectorResultView the {@link RubikDetectorResultView} which will draw the camera preview and detected facelets on screen.
     * @param listener                a {@link OnRubikCubeDetectionResultListener} which will be notified on the main thread after a frame has been processed.
     * @return a {@link FotoapparatBuilder} configured to detect Rubik's Cube facelets and to draw them on screen.
     * @see FotoApparatConnector
     */
    @NonNull
    public static FotoapparatBuilder configure(@NonNull FotoapparatBuilder fotoApparatBuilder,
                                               @NonNull RubikDetector rubikDetector,
                                               @NonNull RubikDetectorResultView rubikDetectorResultView,
                                               @Nullable OnRubikCubeDetectionResultListener listener) {

        return configure(fotoApparatBuilder, rubikDetector, rubikDetectorResultView, SizeSelectors.biggestSize(), listener);
    }

    /**
     * Configures a {@link FotoapparatBuilder} to communicate with a {@link RubikDetector}, to detect Rubik's Cube facelets in the preview frames,
     * and to draw the facelets on the device's screen.
     * <p>
     * This method allows you to specify the desired {@link SelectorFunction}<{@link Collection}<{@link Size}>, {@link Size}>. This selector function
     * will be used when choosing the preview frame size, instead of the default {@link SizeSelectors#biggestSize()}.
     * <p>
     * <b>Note:</b> If a {@link FrameProcessor}, {@link CameraView} or {@link SelectorFunction} are already set on the given {@link FotoapparatBuilder},
     * they will be overwritten by the this method.
     * <p>
     * <b>Note:</b> See the documentation of the {@link FotoApparatConnector} for more details about side effects and inner workings.
     *
     * @param fotoApparatBuilder      the {@link FotoapparatBuilder} to configure.
     * @param rubikDetector           the {@link RubikDetector} instance which will perform the detection.
     * @param rubikDetectorResultView the {@link RubikDetectorResultView} which will draw the camera preview and detected facelets on screen.
     * @param sizeSelectorFunction    the desired {@link SelectorFunction}<{@link Collection}<{@link Size}>, {@link Size}> to be used.
     * @param listener                a {@link OnRubikCubeDetectionResultListener} which will be notified on the main thread after a frame has been processed.
     * @return a {@link FotoapparatBuilder} configured to detect Rubik's Cube facelets and to draw them on screen.
     * @see FotoApparatConnector
     */
    @NonNull
    public static FotoapparatBuilder configure(@NonNull FotoapparatBuilder fotoApparatBuilder,
                                               @NonNull RubikDetector rubikDetector,
                                               @NonNull RubikDetectorResultView rubikDetectorResultView,
                                               @Nullable SelectorFunction<Collection<Size>, Size> sizeSelectorFunction,
                                               @Nullable OnRubikCubeDetectionResultListener listener) {

        RubikDetectorFrameProcessor rubikFrameProcessor = new RubikDetectorFrameProcessor(rubikDetector);
        rubikFrameProcessor.addOnRubikCubeFaceletsFoundListener(rubikDetectorResultView);

        //when using FotoApparat we cannot display the frame with the results drawn in C++. we can use display the original
        //frame from the camera. hence, drawing in C++ is redundant, so we just disable it to save some CPU cycles.
        rubikDetector.setDrawConfig(DrawConfig.DoNotDraw());

        if (listener != null) {
            rubikFrameProcessor.addOnRubikCubeFaceletsFoundListener(listener);
        }

        if (sizeSelectorFunction == null) {
            sizeSelectorFunction = SizeSelectors.biggestSize();
        }

        RubikDetectorSizeSelectorWrapper rubikDetectorSizeSelectorWrapper = new RubikDetectorSizeSelectorWrapper(sizeSelectorFunction, rubikFrameProcessor);

        return fotoApparatBuilder.into(rubikDetectorResultView.getCameraView())
                .previewSize(rubikDetectorSizeSelectorWrapper)
                .frameProcessor(rubikFrameProcessor);
    }
}

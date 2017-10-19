package com.catalinjurjiu.rubikdetector.fotoapparatconnector.view;

import android.content.Context;
import android.content.res.TypedArray;
import android.os.Handler;
import android.os.Looper;
import android.support.annotation.AttrRes;
import android.support.annotation.Dimension;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import com.catalinjurjiu.rubikdetector.RubikDetector;
import com.catalinjurjiu.rubikdetector.config.DrawConfig;
import com.catalinjurjiu.rubikdetector.fotoapparatconnector.OnRubikCubeDetectionResultListener;
import com.catalinjurjiu.rubikdetector.fotoapparatconnector.R;
import com.catalinjurjiu.rubikdetector.fotoapparatconnector.RubikDetectorFrameProcessor;
import com.catalinjurjiu.rubikdetector.fotoapparatconnector.data.RubikFaceletsWrapper;

import io.fotoapparat.Fotoapparat;
import io.fotoapparat.view.CameraView;

/**
 * View capable of displaying a live camera preview, and of drawing on top of the rendered camera preview the
 * Rubik's Cube facelets found with a {@link RubikDetector}.
 * <p>
 * Use {@link #getCameraView()} to obtain the {@link CameraView} instance managed by this {@link RubikDetectorResultView}, when setting up
 * your {@link Fotoapparat} instance.
 * <p>
 * To draw a set of detected facelets, use {@link #drawFacelets(RubikFaceletsWrapper)}.
 * <p>
 * This view also implements {@link OnRubikCubeDetectionResultListener}, so it can be registered as a listener to a {@link RubikDetectorFrameProcessor}. When
 * the processor will call {@link #onRubikCubeDetectionResult(RubikFaceletsWrapper)}, the view will know to update the drawn facelets.
 * <p>
 * To control how to draw the found facelets, use {@link #setDrawConfig(DrawConfig)}. The drawing can also be controlled via the following attributes in XML:
 * <ul>
 * <li>{@code faceletsDrawMode}, valid values: {@code filledCircles}, {@code circles}, {@code rectangles} and {@code doNotDraw};</li>
 * <li>{@code strokeWidth}, valid values are Android dimension values, and XML references to dimension values.</li>
 * </ul>
 * @see RubikDetector
 * @see DrawConfig
 */
@SuppressWarnings("unused")
public class RubikDetectorResultView extends FrameLayout implements OnRubikCubeDetectionResultListener {

    private static final String FILLED_CIRCLES = "filledCircles";
    private static final String CIRCLES = "circles";
    private static final String RECTANGLES = "rectangles";
    private static final String DO_NOT_DRAW = "doNotDraw";
    @Dimension
    private static final int STROKE_NOT_SET = -2;
    private final Handler mainThreadHandler = new Handler(Looper.getMainLooper());
    private CameraView cameraView;
    private FaceletsView faceletsView;

    public RubikDetectorResultView(@NonNull Context context) {
        super(context);
        initView(context, null);
    }

    public RubikDetectorResultView(@NonNull Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        initView(context, attrs);
    }

    public RubikDetectorResultView(@NonNull Context context, @Nullable AttributeSet attrs, @AttrRes int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        initView(context, attrs);
    }

    /**
     * Returns the child {@link CameraView} of this view.
     *
     * @return the child {@link CameraView} of this view.
     */
    public CameraView getCameraView() {
        return cameraView;
    }

    /**
     * Updates the {@link DrawConfig} used when drawing the facelets. This can be called from any thread.
     *
     * @param drawConfig the desired {@link DrawConfig}
     */
    public void setDrawConfig(final DrawConfig drawConfig) {
        if (isOnMainThread()) {
            updateFaceletsViewDrawConfig(drawConfig);
        } else {
            mainThreadHandler.post(new Runnable() {
                @Override
                public void run() {
                    updateFaceletsViewDrawConfig(drawConfig);
                }
            });
        }
    }

    /**
     * Request facelets to be drawn. Calling this with a {@link RubikFaceletsWrapper} that has no facelets will cause the
     * any currently visible facelets to be cleared.
     * <p>
     * Can be called from any thread.
     *
     * @param rubikResult the {@link RubikFaceletsWrapper} which contains the facelets to be drawn.
     */
    public void drawFacelets(@NonNull final RubikFaceletsWrapper rubikResult) {
        drawFaceletsInternal(rubikResult);
    }

    @Override
    public void onRubikCubeDetectionResult(final RubikFaceletsWrapper rubikResult) {
        drawFaceletsInternal(rubikResult);
    }

    @Override
    protected void onLayout(boolean changed, int left, int top, int right, int bottom) {
        final View textureView = findTextureView(cameraView);

        cameraView.layout(left, top, right, bottom);
        faceletsView.layout(
                textureView.getLeft(),
                textureView.getTop(),
                textureView.getRight(),
                textureView.getBottom()
        );
    }

    private void initView(@NonNull Context context, @Nullable AttributeSet attrs) {
        this.cameraView = new CameraView(context);
        cameraView.setLayoutParams(new FrameLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
        addView(cameraView);

        TypedArray attributes = context.obtainStyledAttributes(attrs, R.styleable.RubikDetectorResultView);
        DrawConfig drawConfig = createDrawConfig(attributes);
        attributes.recycle();

        this.faceletsView = new FaceletsView(context, drawConfig);
        faceletsView.setLayoutParams(new FrameLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
        addView(faceletsView);
    }

    @NonNull
    private DrawConfig createDrawConfig(TypedArray attributes) {
        String faceletsDrawMode = attributes.getString(R.styleable.RubikDetectorResultView_faceletsDrawMode);
        int strokeWidthInPixels = attributes.getDimensionPixelSize(R.styleable.RubikDetectorResultView_strokeWidth, STROKE_NOT_SET);

        DrawConfig drawConfig;
        if (faceletsDrawMode == null) {
            drawConfig = DrawConfig.Default();
        } else {
            switch (faceletsDrawMode) {
                case FILLED_CIRCLES:
                    drawConfig = DrawConfig.FilledCircles();
                    break;
                case CIRCLES:
                    drawConfig = DrawConfig.Circles(strokeWidthInPixels);
                    break;
                case RECTANGLES:
                    drawConfig = DrawConfig.Rectangles(strokeWidthInPixels);
                    break;
                case DO_NOT_DRAW:
                    drawConfig = DrawConfig.DoNotDraw();
                    break;
                default:
                    drawConfig = DrawConfig.Default();
            }
        }
        return drawConfig;
    }

    private void drawFaceletsInternal(final RubikFaceletsWrapper rubikResult) {
        if (isOnMainThread()) {
            faceletsView.drawFacelets(rubikResult);
        } else {
            mainThreadHandler.post(new Runnable() {
                @Override
                public void run() {
                    faceletsView.drawFacelets(rubikResult);
                }
            });
        }
    }

    private void updateFaceletsViewDrawConfig(final DrawConfig drawConfig) {
        faceletsView.setDrawConfig(drawConfig);
        faceletsView.invalidate();
    }

    private boolean isOnMainThread() {
        return Looper.getMainLooper() != Looper.myLooper();
    }

    private View findTextureView(View view) {
        View queryView = view;
        while (queryView instanceof ViewGroup) {
            queryView = ((ViewGroup) queryView).getChildAt(0);

            if (queryView instanceof TextureView) {
                return queryView;
            }
        }

        throw new IllegalStateException("Can't find TextureView");
    }
}

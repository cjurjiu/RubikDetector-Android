package com.catalinjurjiu.rubikdetector.fotoapparatconnector.view;

import android.content.Context;
import android.content.res.TypedArray;
import android.support.annotation.AttrRes;
import android.support.annotation.Dimension;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import com.catalinjurjiu.rubikdetector.config.DrawConfig;
import com.catalinjurjiu.rubikdetector.fotoapparatconnector.R;
import com.catalinjurjiu.rubikdetector.fotoapparatconnector.RubikDetectorFrameProcessor;
import com.catalinjurjiu.rubikdetector.fotoapparatconnector.data.RubikResultWrapper;

import io.fotoapparat.view.CameraView;

public class RubikDetectorResultView extends FrameLayout implements RubikDetectorFrameProcessor.OnRubikCubeFaceletsFoundListener {

    public static final String FILLED_CIRCLES = "filledCircles";
    public static final String CIRCLES = "circles";
    public static final String RECTANGLES = "rectangles";
    public static final String DO_NOT_DRAW = "doNotDraw";
    @Dimension
    private static final int STROKE_NOT_SET = -2;
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

    public CameraView getCameraView() {
        return cameraView;
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

    @Override
    public void onRubikCubeFaceletsFound(RubikResultWrapper rubikResult) {
        faceletsView.displayFacelets(rubikResult);
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

package com.catalinjurjiu.rubikdetector.fotoapparatconnector.view;

import android.content.Context;
import android.support.annotation.AttrRes;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import com.catalinjurjiu.rubikdetector.fotoapparatconnector.RubikDetectorFrameProcessor;
import com.catalinjurjiu.rubikdetector.fotoapparatconnector.data.RubikResultWrapper;

import io.fotoapparat.view.CameraView;

/**
 * Created by catalin on 29.08.2017.
 */

public class RubikDetectorResultView extends FrameLayout implements RubikDetectorFrameProcessor.OnRubikCubeFaceletsFoundListener {

    private CameraView cameraView;

    private FaceletsView faceletsView;

    public RubikDetectorResultView(@NonNull Context context) {
        super(context);
        initView(context);
    }

    public RubikDetectorResultView(@NonNull Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        initView(context);
    }

    public RubikDetectorResultView(@NonNull Context context, @Nullable AttributeSet attrs, @AttrRes int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        initView(context);
    }

    public CameraView getCameraView() {
        return cameraView;
    }

    private void initView(Context context) {
        this.cameraView = new CameraView(context);
        cameraView.setLayoutParams(new FrameLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
        addView(cameraView);

        this.faceletsView = new FaceletsView(context);
        faceletsView.setLayoutParams(new FrameLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
        addView(faceletsView);
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

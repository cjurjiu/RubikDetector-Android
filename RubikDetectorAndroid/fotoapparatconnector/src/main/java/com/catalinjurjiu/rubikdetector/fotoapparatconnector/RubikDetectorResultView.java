package com.catalinjurjiu.rubikdetector.fotoapparatconnector;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.os.Looper;
import android.support.annotation.AttrRes;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.util.Log;
import android.widget.FrameLayout;

import com.catalinjurjiu.rubikdetector.RubikDetectorUtils;
import com.catalinjurjiu.rubikdetector.model.RubikFacelet;

import io.fotoapparat.view.CameraView;

/**
 * Created by catalin on 29.08.2017.
 */

public class RubikDetectorResultView extends FrameLayout implements RubikDetectorFrameProcessor.OnRubikCubeFaceletsFoundListener {

    private CameraView cameraView;

    private RubikFacelet[][] facelets;

    private Paint paint;

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

    @Override
    protected void dispatchDraw(Canvas canvas) {
        super.dispatchDraw(canvas);
        if (facelets != null) {
//            RubikDetectorUtils.rescaleResults(facelets, 1600, 1200, 1560, 1080);
            RubikDetectorUtils.drawFaceletsAsCircles(facelets, canvas, paint);
        }
    }

    private void initView(Context context) {
        this.cameraView = new CameraView(context);
        cameraView.setLayoutParams(new FrameLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
        addView(cameraView);

        this.paint = new Paint();
        paint.setStrokeCap(Paint.Cap.ROUND);
        paint.setStrokeWidth(5);
        paint.setStyle(Paint.Style.STROKE);

        setWillNotDraw(false);
    }

    @Override
    public void onRubikCubeFaceletsFound(RubikFacelet[][] cubeFacelets) {
        if (Looper.myLooper() != Looper.getMainLooper()) {
            throw new IllegalStateException("This should only be called from the main thread");
        }
        this.facelets = cubeFacelets;
        invalidate();
    }
}

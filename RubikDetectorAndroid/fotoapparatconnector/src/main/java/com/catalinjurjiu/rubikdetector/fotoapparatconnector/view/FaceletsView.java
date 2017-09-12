package com.catalinjurjiu.rubikdetector.fotoapparatconnector.view;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.os.Looper;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.view.View;

import com.catalinjurjiu.rubikdetector.fotoapparatconnector.data.RubikResultWrapper;
import com.catalinjurjiu.rubikdetector.model.RubikFacelet;

import static com.catalinjurjiu.rubikdetector.RubikDetectorUtils.getAndroidColor;

/**
 * Created by catalin on 01.09.2017.
 */
class FaceletsView extends View {
    private RubikResultWrapper detectionResult;
    private Paint paint;
    private int resultFrameWidth;
    private int resultFrameHeight;
    private int resultFrameSmallSide;

    public FaceletsView(Context context) {
        super(context);
        init();
    }

    public FaceletsView(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public FaceletsView(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    private void init() {
        paint = new Paint();
        paint.setAntiAlias(true);
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeWidth(10);
        paint.setColor(Color.DKGRAY);
    }

    public void displayFacelets(@NonNull RubikResultWrapper rubikResult) {
        ensureMainThread();
        this.detectionResult = rubikResult;
        invalidate();
    }


    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        if (detectionResult != null && detectionResult.getDetectedFacelets() != null) {

            if (resultFrameWidth != detectionResult.getFrameWidth() || resultFrameHeight != detectionResult.getFrameHeight()) {
                resultFrameWidth = detectionResult.getFrameWidth();
                resultFrameHeight = detectionResult.getFrameHeight();
                resultFrameSmallSide = Math.min(resultFrameWidth, resultFrameHeight);
            }

            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    RubikFacelet facelet = detectionResult.getDetectedFacelets()[i][j];
                    paint.setColor(getAndroidColor(facelet));

                    float xRatio = facelet.center.x / resultFrameWidth;
                    float yRatio = facelet.center.y / resultFrameHeight;
                    float radiusRatio = Math.min(facelet.width, facelet.height) / resultFrameSmallSide;

                    canvas.drawCircle(
                            xRatio * getWidth(),
                            yRatio * getHeight(),
                            (radiusRatio * Math.min(getHeight(), getWidth())) / 2f,
                            paint
                    );
                }
            }

        }
    }

    private void ensureMainThread() {
        if (Looper.getMainLooper() != Looper.myLooper()) {
            throw new IllegalThreadStateException("This method must be called from the main thread");
        }
    }
}

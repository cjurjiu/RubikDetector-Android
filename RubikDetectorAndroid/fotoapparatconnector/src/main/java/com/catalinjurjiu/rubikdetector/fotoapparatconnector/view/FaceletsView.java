package com.catalinjurjiu.rubikdetector.fotoapparatconnector.view;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.os.Looper;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.view.View;

import com.catalinjurjiu.rubikdetector.RubikDetectorUtils;
import com.catalinjurjiu.rubikdetector.config.DrawConfig;
import com.catalinjurjiu.rubikdetector.fotoapparatconnector.data.RubikResultWrapper;
import com.catalinjurjiu.rubikdetector.model.Point2d;
import com.catalinjurjiu.rubikdetector.model.RubikFacelet;

/**
 * Created by catalin on 01.09.2017.
 */
class FaceletsView extends View {
    private RubikResultWrapper detectionResult;
    private Paint paint;
    private DrawConfig drawConfig;
    private int resultFrameWidth;
    private int resultFrameHeight;
    private int resultFrameSmallSide;
    private Path path;

    public FaceletsView(Context context, DrawConfig drawConfig) {
        super(context);
        init(drawConfig);
    }

    public FaceletsView(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        init(DrawConfig.Default());
    }

    public FaceletsView(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init(DrawConfig.Default());
    }

    private void init(DrawConfig drawConfig) {
        this.drawConfig = drawConfig;
        if (drawConfig.getDrawMode() == DrawConfig.DrawMode.DRAW_RECTANGLES) {
            path = new Path();
        }
        paint = new Paint();
        paint.setAntiAlias(true);
        paint.setStyle(drawConfig.isFillShape() ? Paint.Style.FILL_AND_STROKE : Paint.Style.STROKE);
        paint.setStrokeWidth(drawConfig.getStrokeWidth());
        paint.setColor(Color.DKGRAY);
    }

    public void displayFacelets(@NonNull RubikResultWrapper rubikResult) {

        if (Looper.getMainLooper() != Looper.myLooper()) {
            throw new IllegalThreadStateException("This method must be called from the main thread");
        }

        if (drawConfig.getDrawMode() == DrawConfig.DrawMode.DO_NOT_DRAW) {
            //do nothing if the user doesn't want to draw the result
            return;
        }

        this.detectionResult = rubikResult;
        invalidate();
    }


    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        if (detectionResult == null || detectionResult.getDetectedFacelets() == null || drawConfig.getDrawMode() == DrawConfig.DrawMode.DO_NOT_DRAW) {
            return;
        }
        if (drawConfig.getDrawMode() == DrawConfig.DrawMode.DRAW_RECTANGLES) {
            drawFaceletsAsRectangles(canvas);
        } else {
            drawFaceletsAsCircles(canvas);
        }
    }

    private void drawFaceletsAsCircles(Canvas canvas) {
        if (resultFrameWidth != detectionResult.getFrameWidth() || resultFrameHeight != detectionResult.getFrameHeight()) {
            resultFrameWidth = detectionResult.getFrameWidth();
            resultFrameHeight = detectionResult.getFrameHeight();
            resultFrameSmallSide = Math.min(resultFrameWidth, resultFrameHeight);
        }

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                RubikFacelet facelet = detectionResult.getDetectedFacelets()[i][j];
                paint.setColor(RubikDetectorUtils.getAndroidColor(facelet));
                float radiusRatio = Math.min(facelet.width, facelet.height) / resultFrameSmallSide;

                canvas.drawCircle(
                        getScaledXValue(facelet.center.x),
                        getScaledYValue(facelet.center.y),
                        (radiusRatio * Math.min(getHeight(), getWidth())) / 2f,
                        paint);
            }
        }
    }

    private void drawFaceletsAsRectangles(Canvas canvas) {
        if (resultFrameWidth != detectionResult.getFrameWidth() || resultFrameHeight != detectionResult.getFrameHeight()) {
            resultFrameWidth = detectionResult.getFrameWidth();
            resultFrameHeight = detectionResult.getFrameHeight();
            resultFrameSmallSide = Math.min(resultFrameWidth, resultFrameHeight);
        }

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                RubikFacelet facelet = detectionResult.getDetectedFacelets()[i][j];
                Point2d[] points = detectionResult.getDetectedFacelets()[i][j].getPoints();

                paint.setColor(RubikDetectorUtils.getAndroidColor(facelet));
                path.reset();

                path.moveTo(getScaledXValue(points[0].x), getScaledYValue(points[0].y));
                path.lineTo(getScaledXValue(points[1].x), getScaledYValue(points[1].y));

                path.moveTo(getScaledXValue(points[1].x), getScaledYValue(points[1].y));
                path.lineTo(getScaledXValue(points[2].x), getScaledYValue(points[2].y));

                path.moveTo(getScaledXValue(points[2].x), getScaledYValue(points[2].y));
                path.lineTo(getScaledXValue(points[3].x), getScaledYValue(points[3].y));

                path.moveTo(getScaledXValue(points[3].x), getScaledYValue(points[3].y));
                path.lineTo(getScaledXValue(points[0].x), getScaledYValue(points[0].y));

                canvas.drawPath(path, paint);
            }
        }
    }

    private float getScaledXValue(float xValue) {
        return xValue / resultFrameWidth * getWidth();
    }

    private float getScaledYValue(float yValue) {
        return yValue / resultFrameHeight * getHeight();
    }
}

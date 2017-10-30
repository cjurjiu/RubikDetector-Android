package com.catalinjurjiu.rubikdetectorfotoapparatconnector.view;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.view.View;

import com.catalinjurjiu.rubikdetector.RubikDetectorUtils;
import com.catalinjurjiu.rubikdetector.config.DrawConfig;
import com.catalinjurjiu.rubikdetectorfotoapparatconnector.data.RubikFaceletsWrapper;
import com.catalinjurjiu.rubikdetector.model.Point2d;
import com.catalinjurjiu.rubikdetector.model.RubikFacelet;

/**
 * View internally used by the {@link RubikFaceletsWrapper} to draw a set of facelets contained in a {@link RubikFaceletsWrapper}.
 * <p>
 * This is not in the public API. Anything here can change in future versions.
 */
class FaceletsView extends View {
    private RubikFaceletsWrapper rubikFaceletsWrapper;
    private Paint paint;
    private DrawConfig drawConfig;
    private int resultFrameWidth;
    private int resultFrameHeight;
    private int resultFrameSmallSide;
    private Path path;

    public FaceletsView(Context context) {
        super(context);
        setDrawConfig(DrawConfig.Default());
    }

    public FaceletsView(Context context, DrawConfig drawConfig) {
        super(context);
        setDrawConfig(drawConfig);
    }

    public FaceletsView(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        setDrawConfig(DrawConfig.Default());
    }

    public FaceletsView(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        setDrawConfig(DrawConfig.Default());
    }

    void drawFacelets(@NonNull RubikFaceletsWrapper rubikFaceletsWrapper) {
        if (drawConfig.getDrawMode() == DrawConfig.DrawMode.DO_NOT_DRAW) {
            //do nothing if the user doesn't want to draw the result
            return;
        }

        this.rubikFaceletsWrapper = rubikFaceletsWrapper;
        invalidate();
    }

    void setDrawConfig(DrawConfig drawConfig) {
        this.drawConfig = drawConfig;
        if (drawConfig.getDrawMode() == DrawConfig.DrawMode.DRAW_RECTANGLES) {
            path = new Path();
        } else {
            path = null;
        }
        paint = new Paint();
        paint.setAntiAlias(true);
        paint.setStyle(drawConfig.isFillShape() ? Paint.Style.FILL_AND_STROKE : Paint.Style.STROKE);
        paint.setStrokeWidth(drawConfig.getStrokeWidth());
        paint.setColor(Color.DKGRAY);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        if (rubikFaceletsWrapper == null || rubikFaceletsWrapper.getDetectedFacelets() == null || drawConfig.getDrawMode() == DrawConfig.DrawMode.DO_NOT_DRAW) {
            return;
        }
        if (drawConfig.getDrawMode() == DrawConfig.DrawMode.DRAW_RECTANGLES) {
            drawFaceletsAsRectangles(canvas);
        } else {
            drawFaceletsAsCircles(canvas);
        }
    }

    @SuppressWarnings("ConstantConditions")
    private void drawFaceletsAsCircles(Canvas canvas) {
        if (resultFrameWidth != rubikFaceletsWrapper.getFrameWidth() || resultFrameHeight != rubikFaceletsWrapper.getFrameHeight()) {
            resultFrameWidth = rubikFaceletsWrapper.getFrameWidth();
            resultFrameHeight = rubikFaceletsWrapper.getFrameHeight();
            resultFrameSmallSide = Math.min(resultFrameWidth, resultFrameHeight);
        }

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                RubikFacelet facelet = rubikFaceletsWrapper.getDetectedFacelets()[i][j];
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

    @SuppressWarnings("ConstantConditions")
    private void drawFaceletsAsRectangles(Canvas canvas) {
        if (resultFrameWidth != rubikFaceletsWrapper.getFrameWidth() || resultFrameHeight != rubikFaceletsWrapper.getFrameHeight()) {
            resultFrameWidth = rubikFaceletsWrapper.getFrameWidth();
            resultFrameHeight = rubikFaceletsWrapper.getFrameHeight();
            resultFrameSmallSide = Math.min(resultFrameWidth, resultFrameHeight);
        }

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                RubikFacelet facelet = rubikFaceletsWrapper.getDetectedFacelets()[i][j];
                Point2d[] points = rubikFaceletsWrapper.getDetectedFacelets()[i][j].corners();

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

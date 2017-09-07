package com.catalinjurjiu.rubikdetector.config;

import android.support.annotation.IntDef;

/**
 * Created by catalin on 07.09.2017.
 */

@SuppressWarnings("unused")
public class DrawConfig {

    private final int strokeWidth;
    private final boolean fillShape;
    @DrawMode
    private final int drawMode;

    private DrawConfig() {
        //private default c-tor
        drawMode = DrawMode.DO_NOT_DRAW;
        strokeWidth = 0;
        fillShape = false;
    }

    private DrawConfig(int drawMode, int strokeWidth, boolean fillShape) {
        this.drawMode = drawMode;
        this.strokeWidth = strokeWidth;
        this.fillShape = false;
    }

    public static DrawConfig Default() {
        return new DrawConfig(DrawMode.DRAW_CIRCLES, 2, false);
    }

    public static DrawConfig DoNotDraw() {
        return new DrawConfig();
    }

    public static DrawConfig Circles(int strokeWidth) {
        return new DrawConfig(DrawMode.DRAW_CIRCLES, strokeWidth, false);
    }

    public static DrawConfig FilledCircles() {
        return new DrawConfig(DrawMode.DRAW_CIRCLES, 0, true);
    }

    public static DrawConfig Rectangles(int strokeWidth) {
        return new DrawConfig(DrawMode.DRAW_RECTANGLES, strokeWidth, false);
    }

    public int getStrokeWidth() {
        return strokeWidth;
    }

    public boolean isFillShape() {
        return fillShape;
    }

    public int getDrawMode() {
        return drawMode;
    }

    @IntDef
    public @interface DrawMode {
        int DO_NOT_DRAW = 0,
                DRAW_RECTANGLES = 1,
                DRAW_CIRCLES = 2;
    }
}
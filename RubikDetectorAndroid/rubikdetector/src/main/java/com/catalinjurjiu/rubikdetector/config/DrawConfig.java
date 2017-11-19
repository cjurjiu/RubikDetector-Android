package com.catalinjurjiu.rubikdetector.config;

import android.support.annotation.IntDef;
import android.support.annotation.NonNull;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

/**
 * Immutable class which specifies how the facelets will be drawn by the {@link com.catalinjurjiu.rubikdetector.RubikDetector}.
 * <p>
 * Static convenience methods are available, as in:
 * <ul>
 * <li>{@link DrawConfig#Default()};</li>
 * <li>{@link DrawConfig#DoNotDraw()};</li>
 * <li>{@link DrawConfig#Circles(int)};</li>
 * <li>{@link DrawConfig#FilledCircles()};</li>
 * <li>{@link DrawConfig#Rectangles(int)};</li>
 * </ul>
 * <p>
 * Drawing filled rectangles is currently unsupported.
 */
@SuppressWarnings("unused")
public class DrawConfig {

    private final int strokeWidth;
    private final boolean fillShape;
    @DrawMode
    private final int drawMode;

    /**
     * Private default constructor. Creates a new immutable {@link DrawConfig} instance with the following properties:
     * <p>
     * <ul>
     * <li>{@link DrawConfig.DrawMode}: {@link DrawConfig.DrawMode#DO_NOT_DRAW}</li>
     * <li>strokeWidth: 0 pixels</li>
     * <li>fillShape: false</li>
     * </ul>
     */
    private DrawConfig() {
        //private default c-tor
        drawMode = DrawMode.DO_NOT_DRAW;
        strokeWidth = 0;
        fillShape = false;
    }

    /**
     * Creates a new immutable {@link DrawConfig} instance with the specified properties.
     * <p>
     * If <b>fillShape</b> is set to <i>true</i>, then the value of <b>strokeWidth</b> is ignored.
     *
     * @param drawMode    a DrawConfig::DrawMode
     * @param strokeWidth the width of the rectangle/circle contour, in pixels.
     * @param fillShape   <i>true</i> if the shape should be filled, <i>false</i> otherwise.
     */
    private DrawConfig(int drawMode, int strokeWidth, boolean fillShape) {
        this.drawMode = drawMode;
        this.strokeWidth = strokeWidth;
        this.fillShape = fillShape;
    }

    /**
     * Creates an immutable {@link DrawConfig} with default values. This implies:
     * <ul>
     * <li>{@link DrawConfig.DrawMode}: {@link DrawConfig.DrawMode#DRAW_CIRCLES}</li>
     * <li>strokeWidth: 2 pixels</li>
     * <li>fillShape: false</li>
     * </ul>
     *
     * @return a {@link DrawConfig} with default values.
     */
    @NonNull
    public static DrawConfig Default() {
        return new DrawConfig(DrawMode.DRAW_CIRCLES, 2, false);
    }

    /**
     * Creates an immutable DrawConfig that does not draw, even when facelets are found.
     *
     * @return a {@link DrawConfig} that does not draw.
     */
    @NonNull
    public static DrawConfig DoNotDraw() {
        return new DrawConfig();
    }

    /**
     * Creates an immutable DrawConfig that draws facelets as circles.
     * <p>
     * The stroke width needs to be specified in the parameter.
     *
     * @param strokeWidth the desired stroke width, in pixels.
     * @return a {@link DrawConfig} that draws facelets as circles.
     */
    @NonNull
    public static DrawConfig Circles(int strokeWidth) {
        return new DrawConfig(DrawMode.DRAW_CIRCLES, strokeWidth, false);
    }

    /**
     * Creates an immutable DrawConfig that draws facelets as filled circles.
     *
     * @return a {@link DrawConfig} that draws facelets as filled circles.
     */
    @NonNull
    public static DrawConfig FilledCircles() {
        return new DrawConfig(DrawMode.DRAW_CIRCLES, 0, true);
    }

    /**
     * Creates an immutable DrawConfig that draws facelets as rectangles.
     * <p>
     * The stroke width needs to be specified in the parameter.
     *
     * @param strokeWidth the desired stroke width, in pixels.
     * @return a {@link DrawConfig} that draws facelets as rectangles.
     */
    @NonNull
    public static DrawConfig Rectangles(int strokeWidth) {
        return new DrawConfig(DrawMode.DRAW_RECTANGLES, strokeWidth, false);
    }

    /**
     * Returns the width of the contour set for this instance.
     *
     * @return the contour width, in pixels.
     */
    public int getStrokeWidth() {
        return strokeWidth;
    }

    /**
     * Can be used to check whether the shape will be filled or not, when drawn.
     *
     * @return <i>true</i> if the shape is filled, <i>false</i> otherwise.
     */
    public boolean isFillShape() {
        return fillShape;
    }

    /**
     * Returns the {@link DrawConfig.DrawMode} set for this instance.
     *
     * @return current {@link DrawConfig.DrawMode}.
     */
    public int getDrawMode() {
        return drawMode;
    }

    /**
     * Specifies the currently available DrawModes.
     */
    @IntDef
    @Retention(RetentionPolicy.SOURCE)
    public @interface DrawMode {
        /**
         * Disables drawing, even if facelets are found.
         */
        int DO_NOT_DRAW = 0,
        /**
         * Draws facelets as rectangles.
         */
        DRAW_RECTANGLES = 1,
        /**
         * Draws facelets as circles.
         */
        DRAW_CIRCLES = 2;
    }
}
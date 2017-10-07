package com.catalinjurjiu.rubikdetector.model;

/**
 * Simple data class representing a 2D Point.
 */
public class Point2d {
    /**
     * x of the point, offset in pixels from origin.
     */
    public float y;

    /**
     * y of the point, offset in pixels from origin.
     */
    public float x;

    /**
     * Creates a new {@link Point2d} at (0,0).
     */
    public Point2d() {
        this(0f, 0f);
    }

    /**
     * Creates a new {@link Point2d} at the specified position.
     *
     * @param x the X coordinate of the point.
     * @param y the Y coordinate of the point.
     */
    public Point2d(float x, float y) {
        this.x = x;
        this.y = y;
    }
}

package com.catalinjurjiu.rubikdetector.model;


import android.support.annotation.IntDef;

/**
 * Data class representing a detected Rubik's Cube facelet.
 * <p>
 * This class holds the color & the location of a particular cube facelet within the input frame in which it was detected.
 * <p>
 * The position of the facelet within the input frame is stored as a rotated rectangle, for which the centre of mass,
 * angle (radians), width & height are known.
 * <p>
 * The four corners of the facelets can be obtained as an array of {@link Point2d} through {@link RubikFacelet#corners()}. Additionally the radius of its
 * inner circle can also be obtained through {@link RubikFacelet#innerCircleRadius()}.
 * <p>
 * The color of the facelet is specified through a {@link RubikFacelet.Color}. The default color of any facelet is {@link RubikFacelet.Color#WHITE}.
 */
public class RubikFacelet {
    /**
     * A {@link RubikFacelet.Color} representing the color of the facelet.
     */
    @RubikFacelet.Color
    public int color;
    /**
     * Center of mass of the facelet.
     */
    public Point2d center;
    /**
     * Width of the facelet, in pixels.
     */
    public float width;
    /**
     * Height of the facelet, in pixels.
     */
    public float height;
    /**
     * Rotation angle of the facelet, in radians.
     */
    public float angle;

    /**
     * Default constructor. The created RubikFacelet will have the following properties:
     * <ul>
     * <li>color: {@link RubikFacelet.Color#WHITE}.</li>
     * <li>centre: Point2d(0, 0);</li>
     * <li>width: 0;</li>
     * <li>height: 0;</li>
     * <li>angle: 0;</li>
     * </ul>
     */
    public RubikFacelet() {
        this(RubikFacelet.Color.WHITE, new Point2d(0, 0), 0, 0, 0);
    }

    /**
     * Constructor.
     *
     * @param color  {@link RubikFacelet.Color} representing the color of the facelet;
     * @param center {@link Point2d} representing the location of centre of mass of the new RubikFacelet, in the input frame it was detected;
     * @param width  Width of the RubikFacelet in pixels;
     * @param height Height of the RubikFacelet in pixels;
     * @param angle  Angle of the RubikFacelet in radians;
     */
    public RubikFacelet(@RubikFacelet.Color int color, Point2d center, float width, float height, float angle) {
        this.color = color;
        this.center = center;
        this.width = width;
        this.height = height;
        this.angle = angle;
    }

    /**
     * Creates a shallow copy of the {@link RubikFacelet} parameter.
     *
     * @param rubikFacelet a {@link RubikFacelet} which will be copied.
     */
    public RubikFacelet(RubikFacelet rubikFacelet) {
        this.color = rubikFacelet.color;
        this.center = rubikFacelet.center;
        this.width = rubikFacelet.width;
        this.height = rubikFacelet.height;
        this.angle = rubikFacelet.angle;
    }

    /**
     * Computes the 4 corners of the facelet in the input frame, given the known properties of the facelet, and returns them in a {@link Point2d} array.
     *
     * @return a 4 element {@link Point2d} array containing, which represents the 4 corners of the facelet.
     */
    public Point2d[] corners() {
        Point2d[] result = new Point2d[4];
        double angleSinHalf = Math.sin(angle) * 0.5f;
        double angleCosHalf = Math.cos(angle) * 0.5f;

        result[0] = new Point2d(Math.round((float) (center.x - angleSinHalf * height - angleCosHalf * width)),
                Math.round((float) (center.y + angleCosHalf * height - angleSinHalf * width)));

        result[1] = new Point2d(Math.round((float) ((center.x + angleSinHalf * height - angleCosHalf * width))),
                Math.round((float) (center.y - angleCosHalf * height - angleSinHalf * width)));

        result[2] = new Point2d(2 * center.x - result[0].x, 2 * center.y - result[0].y);

        result[3] = new Point2d(2 * center.x - result[1].x, 2 * center.y - result[1].y);
        return result;
    }

    /**
     * Computes the radius of the circle centered in the facelet's center of mass, and touches at least two opposite sides of the facelet.
     *
     * @return a float that can be used as the radius of the facelet's inner circle.
     */
    float innerCircleRadius() {
        return Math.min(width, height) / 2;
    }

    /**
     * Defines the possible colors a RubikFacelet can take.
     */
    @IntDef
    public @interface Color {
        int RED = 0,
                ORANGE = 1,
                YELLOW = 2,
                GREEN = 3,
                BLUE = 4,
                WHITE = 5;
    }
}

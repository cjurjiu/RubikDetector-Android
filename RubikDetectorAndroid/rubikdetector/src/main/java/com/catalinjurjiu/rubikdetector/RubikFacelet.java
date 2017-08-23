package com.catalinjurjiu.rubikdetector;

/**
 * Created by catalin on 19.08.2017.
 */

public class RubikFacelet {
    public int color;
    public Point2d center;
    public float width;
    public float height;
    public float angle;

    public RubikFacelet() {
        this(0, new Point2d(0, 0), 0, 0, 0);
    }

    public RubikFacelet(int color, Point2d center, float width, float height, float angle) {
        this.color = color;
        this.center = center;
        this.width = width;
        this.height = height;
        this.angle = angle;
    }

    public Point2d[] getPoints() {
        Point2d[] result = new Point2d[4];
        double angleSinHalf = Math.sin(angle) * 0.5f;
        double angleCosHalf = Math.cos(angle) * 0.5f;

        result[0] = new Point2d(Math.round((float) (center.x - angleSinHalf * height - angleCosHalf * width)),
                Math.round((float) (center.y + angleCosHalf * height - angleSinHalf * width)));

        result[1] = new Point2d(Math.round((float) ((center.x + angleSinHalf * height - angleCosHalf * width))),
                Math.round((float) (center.y - angleCosHalf * height - angleSinHalf * width)));

        result[2] = new Point2d(2 * center.x - result[0].x,
                2 * center.y - result[0].y);

        result[3] = new Point2d(2 * center.x - result[1].x,
                2 * center.y - result[1].y);
        return result;
    }
}

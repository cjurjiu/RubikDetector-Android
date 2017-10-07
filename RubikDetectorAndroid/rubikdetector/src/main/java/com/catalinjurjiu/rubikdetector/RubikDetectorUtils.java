package com.catalinjurjiu.rubikdetector;

import android.graphics.Canvas;
import android.graphics.ImageFormat;
import android.graphics.Paint;
import android.graphics.Path;
import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;

import com.catalinjurjiu.rubikdetector.model.Point2d;
import com.catalinjurjiu.rubikdetector.model.RubikFacelet;

@SuppressWarnings("unused")
public class RubikDetectorUtils {

    /**
     * Rescales the values & position of the detected facelets to match the new resolution.
     *
     * @param result         3x3 {@link RubikFacelet} array of previously detected facelets
     * @param originalWidth  original image width, for which the facelets were detected
     * @param originalHeight original image height, for which the facelets were detected
     * @param newWidth       new image width
     * @param newHeight      new image height
     * @return a 3x3 array of scaled {@link RubikFacelet} objects that match the new desired image resolution.
     */
    public static RubikFacelet[][] rescaleResults(RubikFacelet[][] result, int originalWidth, int originalHeight, int newWidth, int newHeight) {

        if (originalWidth > originalHeight != newWidth > newHeight) {
            throw new IllegalArgumentException("Largest side cannot differ between original frame size, and new frame size. In original: " +
                    "frameWidth > frameHeight = " + (originalWidth > originalHeight) +
                    ", whereas in requested size: " +
                    "width > height = " + (newWidth > newHeight));
        }

        if (newWidth <= 0 || newHeight <= 0) {
            throw new IllegalArgumentException("New dimensions need to be positive numbers.");
        }

        RubikFacelet[][] rescaledResults = new RubikFacelet[3][3];
        int largestDimension = newWidth > newHeight ? newWidth : newHeight;
        int largestFrameDimension = originalWidth > originalHeight ? originalWidth : originalHeight;
        float ratio = (float) largestDimension / largestFrameDimension;

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                RubikFacelet facelet = new RubikFacelet(result[i][j]);

                facelet.center.x *= ratio;
                facelet.center.y *= ratio;
                facelet.width *= ratio;
                facelet.height *= ratio;

                rescaledResults[i][j] = facelet;
            }
        }
        return rescaledResults;
    }

    /**
     * Draws the facelets as empty rectangles, on the received canvas, with the specified paint.
     * <p>
     * For each facelet, the paint color is overwritten with the facelet's color.
     * <p>
     * This internally allocates a path.
     *
     * @param facelets a 3x3 array of {@link RubikFacelet}s which need to be drawn
     * @param canvas   the {@link Canvas} in which the drawing will be performed
     * @param paint    the {@link Paint} used to draw the facelets. Its color will be overwritten by each facelet,
     *                 s.t. each facelet is drawn with its correct color.
     */
    public static void drawFaceletsAsRectangles(RubikFacelet[][] facelets, Canvas canvas, Paint paint) {
        Path path = new Path();
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                paint.setColor(getAndroidColor(facelets[i][j]));
                Point2d[] points = facelets[i][j].corners();
                path.reset();

                path.moveTo(points[0].x, points[0].y);
                path.lineTo(points[1].x, points[1].y);

                path.moveTo(points[1].x, points[1].y);
                path.lineTo(points[2].x, points[2].y);

                path.moveTo(points[2].x, points[2].y);
                path.lineTo(points[3].x, points[3].y);

                path.moveTo(points[3].x, points[3].y);
                path.lineTo(points[0].x, points[0].y);

                canvas.drawPath(path, paint);
            }
        }
    }

    /**
     * Draws the facelets as empty or filled circles, on the received canvas, with the specified paint. To draw the facelets
     * as filled circles, specify {@link Paint.Style#FILL_AND_STROKE} or {@link Paint.Style#FILL} as the paint's style.
     * <p>
     * For each facelet, the paint color is overwritten with the facelet's color.
     * <p>
     *
     * @param facelets a 3x3 array of {@link RubikFacelet}s which need to be drawn
     * @param canvas   the {@link Canvas} in which the drawing will be performed
     * @param paint    the {@link Paint} used to draw the facelets. Its color will be overwritten by each facelet,
     *                 s.t. each facelet is drawn with its correct color.
     */
    public static void drawFaceletsAsCircles(RubikFacelet[][] facelets, Canvas canvas, Paint paint) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                RubikFacelet facelet = facelets[i][j];
                paint.setColor(getAndroidColor(facelet));
                canvas.drawCircle(facelet.center.x, facelet.center.y, Math.min(facelet.width, facelet.height) / 2, paint);
            }
        }
    }

    /**
     * Translates the color of a {@link RubikFacelet} to an Android {@link android.graphics.Color}.
     *
     * @param rubikFacelet the {@link RubikFacelet} whose color needs to be translated.
     * @return a {@link android.graphics.Color} equivalent of the {@link RubikFacelet} color.
     */
    @ColorInt
    public static int getAndroidColor(RubikFacelet rubikFacelet) {
        switch (rubikFacelet.color) {
            case RubikFacelet.Color.WHITE:
                return android.graphics.Color.WHITE;
            case RubikFacelet.Color.RED:
                return android.graphics.Color.RED;
            case RubikFacelet.Color.GREEN:
                return android.graphics.Color.GREEN;
            case RubikFacelet.Color.BLUE:
                return android.graphics.Color.BLUE;
            case RubikFacelet.Color.YELLOW:
                return android.graphics.Color.YELLOW;
            case RubikFacelet.Color.ORANGE:
                return android.graphics.Color.argb(255, 255, 127, 0);
            default:
                return android.graphics.Color.WHITE;
        }
    }

    /**
     * Given an Android {@link ImageFormat}, returns a RubikDetector equivalent {@link RubikDetector.ImageFormat}.
     *
     * @param imageFormatAndroid an Android {@link ImageFormat}
     * @return the equivalent {@link RubikDetector.ImageFormat}.
     */
    @RubikDetector.ImageFormat
    public static int convertAndroidImageFormat(int imageFormatAndroid) {
        switch (imageFormatAndroid) {
            case ImageFormat.NV21:
                return RubikDetector.ImageFormat.YUV_NV21;
            case ImageFormat.YUV_420_888:
                return RubikDetector.ImageFormat.YUV_I420;
            case ImageFormat.YV12:
                return RubikDetector.ImageFormat.YUV_YV12;
            case ImageFormat.FLEX_RGBA_8888:
                return RubikDetector.ImageFormat.ARGB_8888;
            default:
                throw new IllegalArgumentException("Unsupported image format requested.");
        }
    }

    /**
     * Utility method to print the colors of the detected facelets as a formatted string.
     * <p>
     * Useful for printing.
     *
     * @param result 3x3 array of {@link RubikFacelet} objects
     * @return a formatted {@link String} describing the colors of the found facelets.
     */
    public static String getResultColorsAsString(@NonNull RubikFacelet[][] result) {
        StringBuilder stringBuilder = new StringBuilder("Colors: {");
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (j == 0) {
                    stringBuilder.append(" {");
                }
                switch (result[i][j].color) {
                    case RubikFacelet.Color.WHITE:
                        stringBuilder.append("WHITE ");
                        break;
                    case RubikFacelet.Color.YELLOW:
                        stringBuilder.append("YELLOW ");
                        break;
                    case RubikFacelet.Color.RED:
                        stringBuilder.append("RED ");
                        break;
                    case RubikFacelet.Color.BLUE:
                        stringBuilder.append("BLUE ");
                        break;
                    case RubikFacelet.Color.GREEN:
                        stringBuilder.append("GREEN ");
                        break;
                    case RubikFacelet.Color.ORANGE:
                        stringBuilder.append("ORANGE ");
                        break;

                }
                stringBuilder.append(", ");
                stringBuilder.append(result[i][j].angle);
                if (j == 2) {
                    stringBuilder.append("} ");
                }
            }
        }
        stringBuilder.append("}");
        return stringBuilder.toString();
    }
}

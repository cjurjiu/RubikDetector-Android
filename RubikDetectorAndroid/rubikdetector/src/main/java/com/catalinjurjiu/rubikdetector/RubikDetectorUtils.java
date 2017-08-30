package com.catalinjurjiu.rubikdetector;

import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.ImageFormat;
import android.graphics.Paint;
import android.graphics.Path;
import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;

import com.catalinjurjiu.rubikdetector.model.Point2d;
import com.catalinjurjiu.rubikdetector.model.RubikFacelet;

/**
 * Created by catalin on 25.08.2017.
 */
public class RubikDetectorUtils {

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

    public static void drawFaceletsAsRectangles(RubikFacelet[][] facelets, Canvas canvas, Paint paint) {
        Path path = new Path();
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                paint.setColor(getAndroidColor(facelets[i][j]));
                Point2d[] points = facelets[i][j].getPoints();
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

    public static void drawFaceletsAsCircles(RubikFacelet[][] facelets, Canvas canvas, Paint paint) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                RubikFacelet facelet = facelets[i][j];
                paint.setColor(getAndroidColor(facelet));
                canvas.drawCircle(facelet.center.x, facelet.center.y, Math.min(facelet.width, facelet.height) / 2, paint);
            }
        }
    }

    @ColorInt
    public static int getAndroidColor(RubikFacelet rubikFacelet) {
        switch (rubikFacelet.color) {
            case RubikDetector.CubeColors.WHITE:
                return Color.WHITE;
            case RubikDetector.CubeColors.RED:
                return Color.RED;
            case RubikDetector.CubeColors.GREEN:
                return Color.GREEN;
            case RubikDetector.CubeColors.BLUE:
                return Color.BLUE;
            case RubikDetector.CubeColors.YELLOW:
                return Color.YELLOW;
            case RubikDetector.CubeColors.ORANGE:
                return Color.argb(255, 255, 127, 0);
            default:
                return Color.WHITE;
        }
    }

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

    public static String getResultColorsAsString(@NonNull RubikFacelet[][] result) {
        StringBuilder stringBuilder = new StringBuilder("Colors: {");
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (j == 0) {
                    stringBuilder.append(" {");
                }
                switch (result[i][j].color) {
                    case RubikDetector.CubeColors.WHITE:
                        stringBuilder.append("WHITE ");
                        break;
                    case RubikDetector.CubeColors.YELLOW:
                        stringBuilder.append("YELLOW ");
                        break;
                    case RubikDetector.CubeColors.RED:
                        stringBuilder.append("RED ");
                        break;
                    case RubikDetector.CubeColors.BLUE:
                        stringBuilder.append("BLUE ");
                        break;
                    case RubikDetector.CubeColors.GREEN:
                        stringBuilder.append("GREEN ");
                        break;
                    case RubikDetector.CubeColors.ORANGE:
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

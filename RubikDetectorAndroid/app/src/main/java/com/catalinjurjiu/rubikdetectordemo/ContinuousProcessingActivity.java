package com.catalinjurjiu.rubikdetectordemo;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.ImageFormat;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.SurfaceTexture;
import android.graphics.YuvImage;
import android.hardware.Camera;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.catalinjurjiu.rubikdetector.RubikDetector;
import com.catalinjurjiu.rubikdetector.config.DrawConfig;
import com.catalinjurjiu.rubikdetector.model.RubikFacelet;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;

public class ContinuousProcessingActivity extends Activity implements SurfaceHolder.Callback {

//    public static final int PREVIEW_WIDTH = 320;
//    public static final int PREVIEW_HEIGHT = 240;
//
//    public static final int PREVIEW_WIDTH = 640;
//    public static final int PREVIEW_HEIGHT = 480;

//    public static final int PREVIEW_WIDTH = 720;
//    public static final int PREVIEW_HEIGHT = 480;

//    public static final int PREVIEW_WIDTH = 1280;
//    public static final int PREVIEW_HEIGHT = 960;

    public static final int PREVIEW_WIDTH = 1920;
    public static final int PREVIEW_HEIGHT = 1080;

//    public static final int PREVIEW_WIDTH = 1024;
//    public static final int PREVIEW_HEIGHT = 768;

//    public static final int PREVIEW_WIDTH = 800;
//    public static final int PREVIEW_HEIGHT = 600;

    private RubikDetector rubikDetector;

    private Camera camera;

    private SurfaceView surfaceView;

    private SurfaceHolder surfaceHolder;

    private ProcessingThread processingThread;
    private ByteBuffer preallocatedBuffer;
    private Bitmap preallocatedBitmap;
    private Paint paint;

    private int androidImageFormat = ImageFormat.NV21;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.layout_activity_continuous_processing);
//        rubikDetector = new RubikDetector("/storage/emulated/0/RubikResults");
        rubikDetector = new RubikDetector.Builder()
                .debuggable(true)
                .drawConfig(DrawConfig.FilledCircles())
                .inputFrameFormat(androidImageFormat)
                .inputFrameSize(PREVIEW_WIDTH, PREVIEW_HEIGHT)
                .build();

        preallocatedBuffer = ByteBuffer.allocate(rubikDetector.getResultFrameByteCount());
        preallocatedBitmap = Bitmap.createBitmap(PREVIEW_WIDTH, PREVIEW_HEIGHT, Bitmap.Config.ARGB_8888);
        paint = new Paint();
        paint.setAntiAlias(true);
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeWidth(10);
        paint.setColor(Color.DKGRAY);
        processingThread = new ProcessingThread("RubikProcessingThread");
        processingThread.start();
        surfaceView = (SurfaceView) findViewById(R.id.camera_surface_view);
        surfaceHolder = surfaceView.getHolder();
        surfaceHolder.addCallback(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.w("Cata", "surfaceCreated");
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        Log.d("Cata", "surfaceChanged");
        processingThread.openCamera();
        processingThread.startCamera();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        processingThread.stopCameraAndRendering();
        Log.d("Cata", "Done performing cleanup in surface destroyed.");
    }

    @Override
    protected void onDestroy() {
        Log.d("RubikJniPart.cpp", "onDestroy - cleanup.");
        surfaceHolder.removeCallback(this);
        processingThread.quit();
        try {
            processingThread.join();
        } catch (InterruptedException e) {
            Log.d("RubikJniPart.cpp", "onDestroy - exception when waiting for the processing thread to finish.", e);
        } finally {
            rubikDetector.releaseResources();
        }
        super.onDestroy();
    }

    private class ProcessingThread extends HandlerThread implements Camera.PreviewCallback {

        final static int OPEN_CAMERA = 0;
        final static int START_CAMERA = 1;
        final static int STOP_CAMERA_AND_RENDERING = 2;

        private Handler processingHandler;
        private SurfaceTexture surfaceTexture;

        public ProcessingThread(String name) {
            super(name);
        }

        @Override
        protected void onLooperPrepared() {
            super.onLooperPrepared();
            this.processingHandler = new Handler(ProcessingThread.this.getLooper()) {
                @Override
                public void handleMessage(Message msg) {
                    super.handleMessage(msg);
                    switch (msg.what) {
                        case OPEN_CAMERA:
                            openCameraInternal();
                            break;
                        case START_CAMERA:
                            startCameraInternal();
                            break;
                        case STOP_CAMERA_AND_RENDERING:
                            stopCameraAndRenderingInternal();
                            break;
                        default:
                            Log.d("Cata", "Handler default case:" + msg.what);
                    }
                }
            };
        }

        public void openCamera() {
            processingHandler.sendEmptyMessage(OPEN_CAMERA);
        }

        public void startCamera() {
            processingHandler.sendEmptyMessage(START_CAMERA);
        }

        public void stopCameraAndRendering() {
            processingHandler.sendEmptyMessage(STOP_CAMERA_AND_RENDERING);
        }

        private void openCameraInternal() {
            camera = Camera.open(0);
            Camera.Parameters cameraParameters = camera.getParameters();
            cameraParameters.setPreviewSize(PREVIEW_WIDTH, PREVIEW_HEIGHT);
            cameraParameters.setPreviewFormat(androidImageFormat);
            camera.setParameters(cameraParameters);

            camera.addCallbackBuffer(ByteBuffer.allocateDirect(rubikDetector.getRequiredMemory()).array());
            camera.addCallbackBuffer(ByteBuffer.allocateDirect(rubikDetector.getRequiredMemory()).array());
            camera.addCallbackBuffer(ByteBuffer.allocateDirect(rubikDetector.getRequiredMemory()).array());
            camera.setPreviewCallbackWithBuffer(this);
            try {
                surfaceTexture = new SurfaceTexture(13242);
                camera.setPreviewTexture(surfaceTexture);
            } catch (IOException e) {
                Log.w("Cata", "error creating the texture", e);
            }
        }

        private void renderFrameInternal(byte[] data) {
            Log.w("Cata", "renderFrameInternal");
            Canvas canvas = surfaceHolder.lockCanvas();
            if (canvas == null) {
                return;
            }

            Rect srcRect = new Rect(0, 0, PREVIEW_WIDTH, PREVIEW_HEIGHT);
            Rect destRect = new Rect(0, 0, surfaceView.getWidth(), (int) (surfaceView.getWidth() * (PREVIEW_HEIGHT / (float) PREVIEW_WIDTH)));

            RubikFacelet[][] facelets = rubikDetector.findCube(data);

            preallocatedBuffer.rewind();
            preallocatedBuffer.put(data, rubikDetector.getResultFrameBufferOffset(), rubikDetector.getResultFrameByteCount());
            preallocatedBuffer.rewind();
            Log.d("RubikMemoryInfo", "preallocatedBuffer capacity: " + preallocatedBuffer.capacity() + " buffer is direct: " + preallocatedBuffer.isDirect() + " remaining:" + preallocatedBuffer.remaining());
            preallocatedBitmap.copyPixelsFromBuffer(preallocatedBuffer);

            try {
                canvas.drawBitmap(preallocatedBitmap, srcRect, destRect, null);
//                if (facelets != null) {
//                    facelets = RubikDetectorUtils.rescaleResults(facelets,
//                            rubikDetector.getFrameWidth(),
//                            rubikDetector.getFrameHeight(),
//                            destRect.width(),
//                            destRect.height());
//                    Log.d("RubikResult", "drawing facelets!");
////                    RubikDetectorUtils.drawFaceletsAsRectangles(facelets, canvas, paint);
//                    RubikDetectorUtils.drawFaceletsAsCircles(facelets, canvas, paint);
//                } else {
//                    Log.d("RubikResult", "facelets are null!");
//                }
            } catch (Exception e) {
                Log.w("Cata", "Exception while rendering", e);
            } finally {
                surfaceHolder.unlockCanvasAndPost(canvas);
            }
        }

        private Bitmap getAndroidBitmapFromNV21(byte[] data) {
            ByteArrayOutputStream out = new ByteArrayOutputStream();
            YuvImage yuvImage = new YuvImage(data, ImageFormat.NV21, camera.getParameters().getPreviewSize().width, camera.getParameters().getPreviewSize().height, null);
            yuvImage.compressToJpeg(new Rect(0, 0, camera.getParameters().getPreviewSize().width, camera.getParameters().getPreviewSize().height), 50, out);
            byte[] imageBytes = out.toByteArray();
            return BitmapFactory.decodeByteArray(imageBytes, 0, imageBytes.length);
        }

        private void startCameraInternal() {
            camera.startPreview();
        }

        private void temporaryStopCameraInternal() {
            camera.stopPreview();
        }

        private void stopCameraAndRenderingInternal() {
            camera.setPreviewCallback(null);
            camera.stopPreview();
            camera.release();
        }

        @Override
        public void onPreviewFrame(byte[] data, Camera camera) {
            Log.d("RubikMemoryInfo", "onPreviewFrame, data buffer size: " + data.length);
            if (rubikDetector.isActive()) {
                renderFrameInternal(data);
            }
            camera.addCallbackBuffer(data);
        }
    }
}

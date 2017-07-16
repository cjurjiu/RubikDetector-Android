package com.catalinjurjiu.rubikdetectordemo;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.ImageFormat;
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

import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.imgproc.Imgproc;

import java.io.ByteArrayOutputStream;
import java.io.IOException;

public class MainActivity extends Activity implements SurfaceHolder.Callback {

    static {
        System.loadLibrary("opencv_java");
    }

    private RubikDetector rubikDetector;

    private Camera camera;

    private SurfaceView surfaceView;

    private SurfaceHolder surfaceHolder;

    private ProcessingThread processingThread;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        rubikDetector = new RubikDetector();
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
        surfaceHolder.removeCallback(this);
        processingThread.quit();
        super.onDestroy();
    }

    private class ProcessingThread extends HandlerThread implements Camera.PreviewCallback {

        final static int OPEN_CAMERA = 0;
        final static int START_CAMERA = 1;
        final static int RENDER_FRAME = 2;
        final static int TEMPORARY_STOP_CAMERA = 3;
        final static int STOP_CAMERA_AND_RENDERING = 4;
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
                        case RENDER_FRAME:
//                            renderFrameInternal();
                            break;
                        case TEMPORARY_STOP_CAMERA:
                            temporaryStopCameraInternal();
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

        public void renderFrame() {
            processingHandler.sendEmptyMessage(RENDER_FRAME);
        }

        public void startCamera() {
            processingHandler.sendEmptyMessage(START_CAMERA);
        }

        public void temporaryStopCamera() {
            processingHandler.sendEmptyMessage(TEMPORARY_STOP_CAMERA);
        }

        public void stopCameraAndRendering() {
            processingHandler.sendEmptyMessage(STOP_CAMERA_AND_RENDERING);
        }

        private void openCameraInternal() {
            camera = Camera.open(0);
            Camera.Parameters cameraParameters = camera.getParameters();
//            Camera.Size size = cameraParameters.getSupportedPreviewSizes().get(10);
//            cameraParameters.setPreviewSize(size.width, size.height);
            cameraParameters.setPreviewSize(320, 240);
            cameraParameters.setPreviewFormat(ImageFormat.NV21);
            camera.setParameters(cameraParameters);
            camera.setPreviewCallback(this);
            try {
                surfaceTexture = new SurfaceTexture(13242);
                camera.setPreviewTexture(surfaceTexture);
            } catch (IOException e) {
                Log.w("Cata", "error creating the texture", e);
            }
        }

        private Bitmap getOpenCVBitmap(byte[] data) {
            Mat mat = new Mat((int) (camera.getParameters().getPreviewSize().height * 1.5f), camera.getParameters().getPreviewSize().width, CvType.CV_8UC1);
            mat.put(0, 0, data);
            Imgproc.cvtColor(mat, mat, Imgproc.COLOR_YUV2RGB_NV21);
            rubikDetector.findCube(mat);
            Bitmap image = Bitmap.createBitmap(camera.getParameters().getPreviewSize().width, camera.getParameters().getPreviewSize().height, Bitmap.Config.ARGB_8888);
            Utils.matToBitmap(mat, image);
            return image;
        }

        private Bitmap getAndroidBitmap(byte[] data) {
            ByteArrayOutputStream out = new ByteArrayOutputStream();
            YuvImage yuvImage = new YuvImage(data, ImageFormat.NV21, camera.getParameters().getPreviewSize().width, camera.getParameters().getPreviewSize().height, null);
            yuvImage.compressToJpeg(new Rect(0, 0, camera.getParameters().getPreviewSize().width, camera.getParameters().getPreviewSize().height), 50, out);
            byte[] imageBytes = out.toByteArray();
            return BitmapFactory.decodeByteArray(imageBytes, 0, imageBytes.length);
        }

        private void renderFrameInternal(byte[] data) {
            Log.w("Cata", "renderFrameInternal");
            Canvas canvas = surfaceHolder.lockCanvas();
            if (canvas == null) {
                return;
            }
            Rect srcRect = new Rect(0, 0, camera.getParameters().getPreviewSize().width, camera.getParameters().getPreviewSize().height);
            Rect destRect = new Rect(0, 0, surfaceView.getWidth(), surfaceView.getHeight());
            Bitmap image = getOpenCVBitmap(data);
            try {
                canvas.drawBitmap(image, srcRect, destRect, null);
            } catch (Exception e) {
                Log.w("Cata", "Exception while rendering", e);
            } finally {
                surfaceHolder.unlockCanvasAndPost(canvas);
//                image.recycle();
            }
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
            renderFrameInternal(data);
        }
    }
}

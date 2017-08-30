package com.catalinjurjiu.rubikdetectordemo;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

import com.catalinjurjiu.rubikdetector.RubikDetector;
import com.catalinjurjiu.rubikdetector.RubikDetectorUtils;
import com.catalinjurjiu.rubikdetector.fotoapparatconnector.RubikDetectorFrameProcessor;
import com.catalinjurjiu.rubikdetector.fotoapparatconnector.RubikDetectorResultView;
import com.catalinjurjiu.rubikdetector.model.RubikFacelet;

import java.util.Collection;

import io.fotoapparat.Fotoapparat;
import io.fotoapparat.parameter.Size;
import io.fotoapparat.parameter.selector.SelectorFunction;

public class FotoApparatActivity extends Activity {

    private Fotoapparat fotoapparat;

    private int CAMERA_WIDTH = 1600;
    private int CAMERA_HEIGHT = 1200;
    private RubikDetector rubikDetector;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_foto_apparat);

        rubikDetector = new RubikDetector(null);
//        rubikDetector = new RubikDetector("/storage/emulated/0/RubikResults");
        rubikDetector.setImageProperties(CAMERA_WIDTH, CAMERA_HEIGHT, RubikDetector.ImageFormat.YUV_NV21);
        rubikDetector.setDrawFoundFacelets(true);
        rubikDetector.setDebuggable(true);

        RubikDetectorResultView rubikDetectorResultView = (RubikDetectorResultView) findViewById(R.id.rubik_results_view);
        RubikDetectorFrameProcessor rubikFrameProcessor = new RubikDetectorFrameProcessor(rubikDetector, rubikDetectorResultView);

        fotoapparat = Fotoapparat.with(this.getBaseContext())
                .into(rubikDetectorResultView.getCameraView())
                .previewSize(new SelectorFunction<Size>() {
                    @Override
                    public Size select(Collection<Size> collection) {
                        return new Size(CAMERA_WIDTH, CAMERA_HEIGHT);
                    }
                })
                .frameProcessor(rubikFrameProcessor)
                .build();

    }

    @Override
    protected void onStart() {
        super.onStart();
        fotoapparat.start();
    }

    @Override
    protected void onStop() {
        super.onStop();
        fotoapparat.stop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        rubikDetector.releaseResources();
    }
}

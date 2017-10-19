package com.catalinjurjiu.rubikdetectordemo;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

import com.catalinjurjiu.rubikdetector.RubikDetector;
import com.catalinjurjiu.rubikdetector.RubikDetectorUtils;
import com.catalinjurjiu.rubikdetector.config.DrawConfig;
import com.catalinjurjiu.rubikdetector.fotoapparatconnector.FotoApparatConnector;
import com.catalinjurjiu.rubikdetector.fotoapparatconnector.OnRubikCubeDetectionResultListener;
import com.catalinjurjiu.rubikdetector.fotoapparatconnector.data.RubikFaceletsWrapper;
import com.catalinjurjiu.rubikdetector.fotoapparatconnector.view.RubikDetectorResultView;
import com.catalinjurjiu.rubikdetector.model.RubikFacelet;

import io.fotoapparat.Fotoapparat;

public class FotoApparatActivity extends Activity implements OnRubikCubeDetectionResultListener {

    private static final String TAG = FotoApparatActivity.class.getSimpleName();
    private Fotoapparat fotoapparat;
    private RubikDetector rubikDetector;

    @Override
    public void onRubikCubeDetectionResult(RubikFaceletsWrapper rubikResult) {
        RubikFacelet[][] facelets = rubikResult.getDetectedFacelets();
        if (facelets != null) {
            Log.d(TAG, "Detected: " + RubikDetectorUtils.getResultColorsAsString(rubikResult.getDetectedFacelets()));
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_foto_apparat);

        rubikDetector = new RubikDetector.Builder()
                .drawConfig(DrawConfig.FilledCircles())
                .debuggable(true)
                .build();
        RubikDetectorResultView rubikDetectorResultView = findViewById(R.id.rubik_results_view);

        fotoapparat = FotoApparatConnector.configure(Fotoapparat.with(this.getBaseContext()), rubikDetector, rubikDetectorResultView, this)
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

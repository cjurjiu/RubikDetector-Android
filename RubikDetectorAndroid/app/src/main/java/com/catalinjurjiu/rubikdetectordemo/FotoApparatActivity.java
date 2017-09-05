package com.catalinjurjiu.rubikdetectordemo;

import android.app.Activity;
import android.os.Bundle;

import com.catalinjurjiu.rubikdetector.RubikDetector;
import com.catalinjurjiu.rubikdetector.fotoapparatconnector.RubikDetectorFrameProcessor;
import com.catalinjurjiu.rubikdetector.fotoapparatconnector.RubikDetectorResultView;
import com.catalinjurjiu.rubikdetector.fotoapparatconnector.RubikDetectorSizeSelector;

import io.fotoapparat.Fotoapparat;

import static io.fotoapparat.parameter.selector.SizeSelectors.biggestSize;

public class FotoApparatActivity extends Activity {

    private Fotoapparat fotoapparat;
    private RubikDetector rubikDetector;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_foto_apparat);

        rubikDetector = new RubikDetector.Builder()
                .imageSavePath(null)
//                .imageSavePath("/storage/emulated/0/RubikResults")
                .debuggable(true)
                .build();

        RubikDetectorResultView rubikDetectorResultView = (RubikDetectorResultView) findViewById(R.id.rubik_results_view);
        RubikDetectorFrameProcessor rubikFrameProcessor = new RubikDetectorFrameProcessor(rubikDetector, rubikDetectorResultView);
        RubikDetectorSizeSelector rubikDetectorSizeSelector = new RubikDetectorSizeSelector(biggestSize(), rubikFrameProcessor);

        fotoapparat = Fotoapparat.with(this.getBaseContext())
                .into(rubikDetectorResultView.getCameraView())
                .previewSize(rubikDetectorSizeSelector)
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

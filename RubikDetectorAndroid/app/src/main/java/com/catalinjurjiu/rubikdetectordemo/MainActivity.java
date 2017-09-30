package com.catalinjurjiu.rubikdetectordemo;

import android.Manifest;
import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.view.View;

public class MainActivity extends Activity implements View.OnClickListener {

    public static final String TAG = "RubikDetectorSampleApp";
    private static final int PERMISSION_REQUEST_CODE = 1283;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        findViewById(R.id.button_open_continuous_activity).setOnClickListener(this);
        findViewById(R.id.button_open_fotoapparat_activity).setOnClickListener(this);
        findViewById(R.id.button_open_picture_processing).setOnClickListener(this);

        if (isPermissionGranted(Manifest.permission.WRITE_EXTERNAL_STORAGE) ||
                isPermissionGranted(Manifest.permission.CAMERA)) {
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.CAMERA},
                    PERMISSION_REQUEST_CODE);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == PERMISSION_REQUEST_CODE) {
            for (int i = 0; i < grantResults.length; i++) {
                if (grantResults[i] != PackageManager.PERMISSION_GRANTED) {
                    Log.d(TAG, "Permission \"" + permissions[i] + "\" denied. Using its associated features will not be possible!");
                    return;
                }
            }
        }
        Log.d(TAG, "All permissions accepted!");
    }

    @Override
    public void onClick(View view) {
        Intent openActivityIntent;
        switch (view.getId()) {
            case R.id.button_open_continuous_activity:
                openActivityIntent = new Intent(getBaseContext(), LiveDetectionActivity.class);
                break;
            case R.id.button_open_fotoapparat_activity:
                openActivityIntent = new Intent(getBaseContext(), FotoApparatActivity.class);
                break;
            case R.id.button_open_picture_processing:
                openActivityIntent = new Intent(getBaseContext(), SinglePhotoActivity.class);
                break;
            default:
                Log.d(TAG, "Unknown view clicked. Click ignored.");
                return;
        }
        startActivity(openActivityIntent);
    }

    private boolean isPermissionGranted(String permissionId) {
        return ContextCompat.checkSelfPermission(getBaseContext(), permissionId) != PackageManager.PERMISSION_GRANTED;
    }
}

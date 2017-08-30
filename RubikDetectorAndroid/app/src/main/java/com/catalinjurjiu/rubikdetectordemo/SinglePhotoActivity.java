package com.catalinjurjiu.rubikdetectordemo;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.support.v4.content.FileProvider;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;

import com.catalinjurjiu.rubikdetector.RubikDetector;
import com.catalinjurjiu.rubikdetector.model.RubikFacelet;

import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.text.SimpleDateFormat;
import java.util.Date;

public class SinglePhotoActivity extends Activity {
    private static final int CAMERA_REQUEST = 1888;
    private ImageView imageView;
    private RubikDetector rubikDetector;
    private String mCurrentPhotoPath;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.layout_activity_single_photo);
        this.imageView = (ImageView) this.findViewById(R.id.image_view_photo_display);
        Button photoButton = (Button) this.findViewById(R.id.button_take_picture);
        photoButton.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                Intent cameraIntent = new Intent(android.provider.MediaStore.ACTION_IMAGE_CAPTURE);
                File photoFile = createImageFile();
                if (photoFile != null) {
                    Uri imageUri = FileProvider.getUriForFile(SinglePhotoActivity.this,
                            "com.example.android.fileprovider",
                            photoFile);
                    cameraIntent.putExtra(MediaStore.EXTRA_OUTPUT, imageUri);
                }
                startActivityForResult(cameraIntent, CAMERA_REQUEST);
            }
        });
        rubikDetector = new RubikDetector();
        rubikDetector.setDebuggable(true);
        rubikDetector.setDrawFoundFacelets(true);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        rubikDetector.releaseResources();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == CAMERA_REQUEST && resultCode == Activity.RESULT_OK) {
            if (mCurrentPhotoPath != null) {

                Bitmap photoBitmap = BitmapFactory.decodeFile(mCurrentPhotoPath);

                rubikDetector.setImageProperties(photoBitmap.getWidth(), photoBitmap.getHeight(), RubikDetector.ImageFormat.ARGB_8888);

                ByteBuffer byteBuffer = ByteBuffer.allocateDirect(rubikDetector.getRequiredMemory());
                photoBitmap.copyPixelsToBuffer(byteBuffer);
                RubikFacelet[][] result = rubikDetector.findCube(byteBuffer.array());
                byteBuffer.rewind();
                photoBitmap.copyPixelsFromBuffer(byteBuffer);
                imageView.setImageBitmap(photoBitmap);
                if (result != null) {
                    Log.d("Cata", "Cube found!");
                }
            }
        }
    }

    private File createImageFile() {
        // Create an image file name
        String imageFileName = "JPEG_" + new SimpleDateFormat("yyyyMMdd_HHmmss").format(new Date()) + "_";
        File storageDir = getExternalFilesDir(Environment.DIRECTORY_PICTURES);
        File image = null;
        try {
            image = File.createTempFile(
                    imageFileName,  /* prefix */
                    ".jpg",         /* suffix */
                    storageDir      /* directory */
            );
            mCurrentPhotoPath = image.getAbsolutePath();
        } catch (IOException e) {
            Log.d("PhotoActivity", "Could not create image file.", e);
        }
        return image;
    }
}
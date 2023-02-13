package com.wyc.utils;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.TextureView;
import android.widget.FrameLayout;

import java.io.IOException;

public class SurfaceTextureActivity extends AppCompatActivity implements TextureView.SurfaceTextureListener {
    private static final String TAG = "AndCore.View";
    private Camera mCamera;
    private TextureView mTextureView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mTextureView = new TextureView(this);
        mTextureView.setSurfaceTextureListener(this);
        setContentView(mTextureView);
    }

    @Override
    public void onSurfaceTextureAvailable(@NonNull SurfaceTexture surfaceTexture, int width, int height) {
        ActivityCompat.requestPermissions(SurfaceTextureActivity.this, new String[]{ Manifest.permission.CAMERA }, 5);
        try {
            mCamera = Camera.open();
            mTextureView.setLayoutParams(new FrameLayout.LayoutParams(
                    width, height, Gravity.CENTER));
            mCamera.setPreviewTexture(surfaceTexture);
        } catch (IOException t) {
            Log.d(TAG, "onSurfaceTextureAvailable", t);
        }
        mCamera.startPreview();
        Log.d(TAG, "onSurfaceTextureAvailable");
    }

    @Override
    public void onSurfaceTextureSizeChanged(@NonNull SurfaceTexture surfaceTexture, int i, int i1) {
        Log.d(TAG, "onSurfaceTextureSizeChanged");
    }

    @Override
    public boolean onSurfaceTextureDestroyed(@NonNull SurfaceTexture surfaceTexture) {
        Log.d(TAG, "onSurfaceTextureDestroyed");
        mCamera.stopPreview();
        mCamera.release();
        return false;
    }

    @Override
    public void onSurfaceTextureUpdated(@NonNull SurfaceTexture surfaceTexture) {
        Log.d(TAG, "onSurfaceTextureUpdated");
    }
}

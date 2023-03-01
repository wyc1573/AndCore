package com.wyc.utils.test;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import com.wyc.utils.R;

public class UtilsGraphicsActivity extends AppCompatActivity {
    Button surfaceViewBtn;
    Button glSurfaceViewBtn;
    Button textureBtn;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.utils_graphics);
        surfaceViewBtn = findViewById(R.id.utils_ui_surfaceview);
        glSurfaceViewBtn = findViewById(R.id.utils_ui_glsurfaceview);
        textureBtn = findViewById(R.id.utils_ui_texture);
        surfaceViewBtn.setOnClickListener(new SurfaceViewClickListener());
        glSurfaceViewBtn.setOnClickListener(new GlSurfaceViewClickListener());
        textureBtn.setOnClickListener(new TextureClickListener());
    }

    class SurfaceViewClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            startActivity(new Intent(UtilsGraphicsActivity.this, UtilsSurfaceViewActivity.class));
        }
    }

    class GlSurfaceViewClickListener implements View.OnClickListener {

        @Override
        public void onClick(View view) {
            startActivity(new Intent(UtilsGraphicsActivity.this, UtilsGlSurfaceViewActivity.class));
        }
    }

    class TextureClickListener implements View.OnClickListener {

        @Override
        public void onClick(View view) {
            startActivity(new Intent(UtilsGraphicsActivity.this, UtilsSurfaceTextureActivity.class));
        }
    }
}
package com.wyc.utils;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class GraphicsActivity extends AppCompatActivity {
    Button surfaceViewBtn;
    Button glSurfaceViewBtn;
    Button textureBtn;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_graphics);
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
            startActivity(new Intent(GraphicsActivity.this, SurfaceViewActivity.class));
        }
    }

    class GlSurfaceViewClickListener implements View.OnClickListener {

        @Override
        public void onClick(View view) {
            startActivity(new Intent(GraphicsActivity.this, GlSurfaceViewActivity.class));
        }
    }

    class TextureClickListener implements View.OnClickListener {

        @Override
        public void onClick(View view) {
            startActivity(new Intent(GraphicsActivity.this, SurfaceTextureActivity.class));
        }
    }
}
package com.wyc.utils;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;

import com.wyc.utils.ui.DemoGlSurfaceView;

public class GlSurfaceViewActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        DemoGlSurfaceView glSurfaceView = new DemoGlSurfaceView(this);
        setContentView(glSurfaceView);
    }
}

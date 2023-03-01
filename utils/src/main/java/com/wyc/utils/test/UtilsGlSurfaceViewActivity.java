package com.wyc.utils.test;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;

import com.wyc.utils.test.ui.DemoGlSurfaceView;

public class UtilsGlSurfaceViewActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        DemoGlSurfaceView glSurfaceView = new DemoGlSurfaceView(this);
        setContentView(glSurfaceView);
    }
}

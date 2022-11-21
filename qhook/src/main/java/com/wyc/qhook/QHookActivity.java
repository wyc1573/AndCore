package com.wyc.qhook;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class QHookActivity extends AppCompatActivity {
    public static final String TAG = "AndCore.QHookActivity";

    Button start;
    Button stop;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_qhook);
        JNI.init();
        start = (Button) findViewById(R.id.start_hook);
        stop = (Button) findViewById(R.id.stop_hook);
        start.setOnClickListener(new StartClickListener());
        stop.setOnClickListener(new StopClickListener());
    }

    public static class StartClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            JNI.nativeStartHook(0);
        }
    }

    public static class StopClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            JNI.nativeStopHook(0);
        }
    }
}

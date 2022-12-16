package com.wyc.qhook;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class QHookActivity extends AppCompatActivity {
    public static final String TAG = "AndCore.QHookActivity";

    Button start;
    Button stop;
    Button dlInit;
    Button action;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_qhook);
        JNI.init();
        start = (Button) findViewById(R.id.start_hook);
        stop = (Button) findViewById(R.id.stop_hook);
        dlInit = (Button) findViewById(R.id.dl_init);
        action = (Button) findViewById(R.id.action);
        start.setOnClickListener(new StartClickListener());
        stop.setOnClickListener(new StopClickListener());
        dlInit.setOnClickListener(new InitClickListener());
        action.setOnClickListener(new ActionClickListener());
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

    public static class InitClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            JNI.nativeDlInit(0);
        }
    }

    public static class ActionClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            JNI.nativeAction(0);
        }
    }
}

package com.wyc.utils;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

public class UtilsMainActivity extends AppCompatActivity {
    public static final String TAG = "AndCore.UtilsMainActivity";
    Button generic_test;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.utils_main);
        generic_test = findViewById(R.id.utils_test);
        generic_test.setOnClickListener(new GenericClickListener());
        JNI.setPath(this.getFilesDir().getAbsolutePath());
    }

    class GenericClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            Log.d(TAG, "click generic test btn.");
            JNI.test(Cmd.GENERIC);
            Toast.makeText(UtilsMainActivity.this,
                    "adb logcat -s AndCore", Toast.LENGTH_LONG).show();
        }
    }
}
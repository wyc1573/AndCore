package com.wyc.utils.test;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import com.wyc.utils.R;

public class UtilsUnixActivity extends AppCompatActivity {
    Button btnSignalFd;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.utils_unix);
        btnSignalFd = findViewById(R.id.utils_unix);
        btnSignalFd.setOnClickListener(new SignalFdClickListener());
    }

    class SignalFdClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {

        }
    }
}

package com.wyc.utils.test;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import com.wyc.utils.JNI;
import com.wyc.utils.R;

public class UtilsUnixActivity extends AppCompatActivity {
    public static final String TAG = "AndCore.UnixActivity";

    Button btnSignalFd;
    Button btnMmap;
    Button btnElf;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.utils_unix);
        btnSignalFd = findViewById(R.id.utils_unix);
        btnSignalFd.setOnClickListener(new SignalFdClickListener());

        btnMmap = findViewById(R.id.utils_mmap);
        btnMmap.setOnClickListener(new MmapClickListener());

        btnElf = findViewById(R.id.utils_elf);
        btnElf.setOnClickListener(new ElfClickListener());
    }

    static {
        try {
            System.loadLibrary("ElfDemo");
        } catch (Throwable t) {
            Log.d(TAG, "", t);
        }
    }

    class SignalFdClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {

        }
    }

    class MmapClickListener implements View.OnClickListener {

        @Override
        public void onClick(View v) {
            Log.d(TAG, "mmap btn clicked.");
            JNI.mmapThenDel();
        }
    }

    class ElfClickListener implements View.OnClickListener {
        @Override
        public void onClick(View v) {
            Log.d(TAG, "elf btn clicked.");
            JNI.testElf();
        }
    }
}

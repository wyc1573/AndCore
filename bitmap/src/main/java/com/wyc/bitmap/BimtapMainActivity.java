package com.wyc.bitmap;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;


public class BimtapMainActivity extends AppCompatActivity {
    public static final String TAG = "AndCore.Bitmap";
    Button bitmap_start_hook;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bimtap_main);
        bitmap_start_hook = findViewById(R.id.bitmap_start);
        bitmap_start_hook.setOnClickListener(new NativeBitmapClickListener(BimtapMainActivity.this));
    }

    class NativeBitmapClickListener implements View.OnClickListener {
        public NativeBitmapClickListener(Context context) { this.context = context; }

        @Override
        public void onClick(View view) {
            if (Build.VERSION.SDK_INT > Build.VERSION_CODES.M) {
                Toast.makeText(context, "Illegal! must below android N", Toast.LENGTH_SHORT).show();
                return;
            }
            JNI.init();
            JNI.start();
            Toast.makeText(context, "start opt...", Toast.LENGTH_SHORT).show();
        }
        private Context context;
    }
}
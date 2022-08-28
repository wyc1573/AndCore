package com.wyc.home;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import com.wyc.bitmap.BimtapMainActivity;
import com.wyc.utils.UtilsMainActivity;

public class HomeActivity extends AppCompatActivity {
    public static final String TAG = "AndCore.HomeActivity";
    Button btn_native_bitmap;
    Button btn_utils;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_home);
        btn_native_bitmap = findViewById(R.id.home_native_bitmap);
        btn_native_bitmap.setOnClickListener(new BitmapClickListener());
        btn_utils = findViewById(R.id.home_utils);
        btn_utils.setOnClickListener(new UtilsClickListener());
    }

    class BitmapClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            Log.d(TAG, "click native bitmap button.");
            startActivity(new Intent(HomeActivity.this, BimtapMainActivity.class));
        }
    }

    class UtilsClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            Log.d(TAG, "click utils button.");
            startActivity(new Intent(HomeActivity.this, UtilsMainActivity.class));
        }
    }
}
package com.wyc.utils.test;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import com.wyc.utils.JNI;
import com.wyc.utils.R;

public class UtilsCppActivity extends AppCompatActivity {
    Button btn_objModel;
    Button btn_cppConcurrency;
    Button btn_misc;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.utils_cpp);
        btn_objModel = findViewById(R.id.utils_cpp_obj);
        btn_objModel.setOnClickListener(new ObjModelClickListener());
        btn_misc = findViewById(R.id.utils_cpp_misc);
        btn_misc.setOnClickListener(new MiscClickListener());
        btn_cppConcurrency = findViewById(R.id.utils_cpp_concurrency);
        btn_cppConcurrency.setOnClickListener(new CppConcurrencyClickListener());
    }

    class ObjModelClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            JNI.cppObjModel();
        }
    }

    class MiscClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            JNI.cppMisc();
        }
    }

    class CppConcurrencyClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            JNI.cppConcurrency();
        }
    }
}
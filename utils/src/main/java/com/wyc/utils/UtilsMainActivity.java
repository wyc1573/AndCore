package com.wyc.utils;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.wyc.utils.dexopt.DexOptUtils;

public class UtilsMainActivity extends AppCompatActivity {
    public static final String TAG = "AndCore.Utils";
    Button generic;
    Button dex2oat;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.utils_main);
        generic = findViewById(R.id.utils_test);
        dex2oat = findViewById(R.id.utils_dex2oat);
        generic.setOnClickListener(new GenericClickListener());
        dex2oat.setOnClickListener(new Dex2oatClickListener());
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

    class Dex2oatClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            Log.d(TAG, "Dex2Oat test btn.");
            Context context = getBaseContext();
            try {
                String path = context.getPackageManager().getApplicationInfo(context.getPackageName(), 0).sourceDir;
                Log.d(TAG, "path = " + apkToOatPath(path));
//                DexOptUtils.performDexOptSecondary(UtilsMainActivity.this.getBaseContext(), apkToOatPath(path));
                DexOptUtils.dexOpt(context, "quicken", apkToOatPath(path));
            } catch (PackageManager.NameNotFoundException e) {
                Log.d(TAG, "", e);
            }
        }
    }

    /**
     * @param path eg. /data/app/com.wyc.andcore-swEXFEdZCjv3_HUahATKWg==/base.apk
     */
    static String apkToOatPath(String path) {
        if (path == null) {
            return null;
        }
        int pos = path.lastIndexOf('/');
        if (pos < 0) {
            return null;
        }
        String[] abis = Build.SUPPORTED_ABIS;
        if (abis == null || abis.length < 1 || abis[0] == null) {
            return null;
        }
        String abi = abis[0].startsWith("arm64") ? "arm64" : "arm";
        StringBuilder sb = new StringBuilder();
        sb.append(path.substring(0, pos))
                .append('/')
                .append("oat")
                .append("/")
                .append(abi)
                .append("/")
                .append("base.odex");
        return sb.toString();
    }
}
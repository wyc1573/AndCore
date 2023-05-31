package com.wyc.utils;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.wyc.utils.art.ArtUtils;
import com.wyc.utils.base.BaseUtils;
import com.wyc.utils.dexopt.DexOptUtils;
import com.wyc.utils.test.BayesUtil;
import com.wyc.utils.test.UtilsCppActivity;
import com.wyc.utils.test.UtilsIPCActivity;
import com.wyc.utils.tools.FileUtils;

public class UtilsMainActivity extends AppCompatActivity {
    public static final String TAG = "AndCore.Utils";
    Button generic;
    Button dex2oat;
    Button pid;
    Button ipc;

    Button bayes;
    Button statistics;

    Button art;

    Button build;

    Button maps;

    Button anr;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.utils_main);
        generic = findViewById(R.id.utils_test);
        dex2oat = findViewById(R.id.utils_dex2oat);
        pid = findViewById(R.id.utils_mypid);
        ipc = findViewById(R.id.utils_ipc_activity);
        bayes = findViewById(R.id.utils_bayes);
        statistics = findViewById(R.id.utils_statistics);
        generic.setOnClickListener(new GenericClickListener());
        dex2oat.setOnClickListener(new Dex2oatClickListener());
        pid.setOnClickListener(new MyPidClickListener());
        ipc.setOnClickListener(new IpcClickListener());
        bayes.setOnClickListener(new BayesClickListener());
        statistics.setOnClickListener(new StatisticsClickListener());
        JNI.setPath(this.getFilesDir().getAbsolutePath());
        art  = findViewById(R.id.utils_art);
        art.setOnClickListener(new ArtClickListener());
        build = findViewById(R.id.utils_build);
        build.setOnClickListener(new BuildClickListener());

        maps = findViewById(R.id.utils_maps);
        maps.setOnClickListener(new MapsClickListener());

        anr = findViewById(R.id.utils_anr);
        anr.setOnClickListener(new ANRClickListener());
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

    class MyPidClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            Log.d(TAG, "click pid btn!");
            int pid = android.os.Process.myPid();
            Toast.makeText(UtilsMainActivity.this, "pid = " + pid, Toast.LENGTH_SHORT).show();
        }
    }

    class IpcClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            Log.d(TAG, "click ipc btn.");
            startActivity(new Intent(UtilsMainActivity.this, UtilsIPCActivity.class));
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

    class BayesClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            BayesUtil.mock();
        }
    }

    class StatisticsClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            JNI.statistics();
        }
    }

    class ArtClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            Log.d(TAG, "click ClassLoader button.");
            ArtUtils.testDelegateLastClassLoader("demo.apk", "descriptor", "keyword");
        }
    }

    class BuildClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            Log.d(TAG, "click build button.");
            BaseUtils.printBuild();
        }
    }

    class MapsClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            Log.d(TAG, "click maps button.");
            StringBuilder sb = new StringBuilder();
            FileUtils.forEachLine("/proc/self/maps", new FileUtils.IFilter() {
            }, new FileUtils.IParser() {
                @Override
                public void parse(String line) {
                    sb.append(line).append("\n");
                }
            });
            Log.d(TAG, sb.toString());
        }
    }

    class ANRClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            Log.d(TAG, "click ANR button.");
            try {
                Thread.sleep(1000*1000);
            } catch (Throwable t) {

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
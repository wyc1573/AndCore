package com.wyc.utils;

import android.app.Application;
import android.util.Log;
import android.os.Process;
public class AndCoreRuntime {
    public static final String TAG = "AndCore.Runtime";
    private static volatile AndCoreRuntime instance;

    private Application application;
    private AndCoreRuntime() {}

    public static AndCoreRuntime instance() {
        if (instance == null) {
            synchronized (AndCoreRuntime.class) {
                if (instance == null) {
                    instance = new AndCoreRuntime();
                }
            }
        }
        return instance;
    }

    public void init(Application application) {
        if (application == null) {
            Log.d(TAG, "application == null, exit!");
            Process.killProcess(Process.myPid());
        }
        this.application = application;
    }

    public Application application() {
        if (application == null) {
            Log.d(TAG, "must init, before use.");
            Process.killProcess(Process.myPid());
        }
        return application;
    }
}

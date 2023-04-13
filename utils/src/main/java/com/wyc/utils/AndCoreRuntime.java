package com.wyc.utils;

import android.app.Application;
import android.util.Log;
import android.os.Process;
public class Runtime {
    public static final String TAG = "AndCore.Runtime";
    private static volatile Runtime instance;

    private Application application;
    private Runtime() {}

    public static Runtime instance() {
        if (instance == null) {
            synchronized (Runtime.class) {
                if (instance == null) {
                    instance = new Runtime();
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

    Application application() {
        if (application == null) {
            Log.d(TAG, "must init, before use.");
            Process.killProcess(Process.myPid());
        }
        return application;
    }
}

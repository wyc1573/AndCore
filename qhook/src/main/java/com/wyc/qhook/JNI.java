package com.wyc.qhook;

import android.util.Log;

import com.bytedance.shadowhook.ShadowHook;

public class JNI {
    public static final String TAG = "AndCore.QHookActivity";
    static {
        try {
            System.loadLibrary("qhook");
        } catch(Throwable t) {
            Log.d(TAG, "");
        }
    }

    public static void init() {
        ShadowHook.init(new ShadowHook.ConfigBuilder()
                .setMode(ShadowHook.Mode.UNIQUE)
                .build());
    }

    public static native void nativeStartHook(int mode);
    public static native void nativeStopHook(int mode);
    public static native void nativeDlInit(int mode);
    public static native void nativeAction(int mode);
}

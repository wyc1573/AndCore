package com.wyc.bitmap;

import android.util.Log;

import com.bytedance.shadowhook.ShadowHook;

/**
 * <description>
 *
 * @author wyc
 * @date 2022/8/6
 */
public class JNI {
    public static final String TAG = "AndCore.Bitmap";
    static {
        try {
            System.loadLibrary("NativeBitmap");
        } catch (Throwable t) {
            Log.d(TAG, t.getMessage());
        }
    }

    public static void init() {
        ShadowHook.init(new ShadowHook.ConfigBuilder()
                .setMode(ShadowHook.Mode.UNIQUE)
                .build());
    }

    public static native void start();
    public static native void stop();
}

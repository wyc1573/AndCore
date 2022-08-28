package com.wyc.utils;

/**
 * <description>
 *
 * @author wyc
 * @date 2022/8/6
 */
public class JNI {
    static {
        System.loadLibrary("Utils");
    }

    public static native void test(int mode);
    public static native void setPath(String path);
}

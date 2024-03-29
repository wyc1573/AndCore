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
    public static native void socketNormal();
    public static native void socketDiagram();
    public static native void socketMsg();
    public static native void socketPair();
    public static native void socketPipe();
    public static native void eventFd();
    public static native void epoll();

    public static native void cppObjModel();
    public static native void cppConcurrency();
    public static native void cppMisc();
    public static native void statistics();

    public static native void testElf();
    public static native void mmapThenDel();

    public static native void tempTest();
}

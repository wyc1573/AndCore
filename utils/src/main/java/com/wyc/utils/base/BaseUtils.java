package com.wyc.utils.base;

import android.os.Build;
import android.util.Log;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.lang.reflect.Method;

public class BaseUtils {
    public static final String TAG = "AndCore.BaseUtils";
    public static String getProp(String key, String def) {
        if (key == null) {
            return def;
        }
        try {
            Class<?> cls = Class.forName("android.os.SystemProperties");
            Method getMethod = cls.getMethod("get", String.class, String.class);
            return (String) getMethod.invoke(null, key, def);
        } catch (Throwable t) {
            Log.d(TAG, "getProp", t);
        }
        return def;
    }

    public static void printBuild() {
        try {
            Log.d(TAG, "Build.BOARD = " + Build.BOARD);
            Log.d(TAG, "Build.DEVICE = " + Build.DEVICE);
            Log.d(TAG, "Build.VERSION.SDK_INT = " + Build.VERSION.SDK_INT);
            Log.d(TAG, "Build.VERSION.BASE_OS = " + Build.VERSION.BASE_OS);
            Log.d(TAG, "Build.VERSION.CODENAME = " + Build.VERSION.CODENAME);
            Log.d(TAG, "Build.VERSION.INCREMENTAL" + Build.VERSION.INCREMENTAL);
            Log.d(TAG, "Build.VERSION.SECURITY_PATCH = " + Build.VERSION.SECURITY_PATCH);

            for (String abi : Build.SUPPORTED_ABIS) {
                Log.d(TAG, "Build.SUPPORTED_ABIS = " + abi);
            }

            Log.d(TAG, "Build.BRAND = " + Build.BRAND);
            Log.d(TAG, "Build.HARDWARE = " + Build.HARDWARE);
            Log.d(TAG, "Build.DISPLAY = " + Build.DISPLAY);
            Log.d(TAG, "Build.FINGERPRINT = " + Build.FINGERPRINT);

            Log.d(TAG, "Build.BOOTLOADER = " + Build.BOOTLOADER);
            Log.d(TAG, "Build.HOST = " + Build.HOST);
            Log.d(TAG, "Build.ID = " + Build.ID);
            Log.d(TAG, "Build.MANUFACTURER = " + Build.MANUFACTURER);
            Log.d(TAG, "Build.MODEL = " + Build.MODEL);
            Log.d(TAG, "Build.PRODUCT = " + Build.PRODUCT);
            Log.d(TAG, "Build.TAGS = " + Build.TAGS);
            Log.d(TAG, "Build.TYPE = " + Build.TYPE);
            Log.d(TAG, "Build.USER = " + Build.USER);
            Log.d(TAG, "Build.SERIAL = " + Build.SERIAL);
            Log.d(TAG, "Build.TIME = " + Build.TIME);
        } catch (Throwable t) {
            Log.e(TAG, "printBuild", t);
        }
    }

    /**
     * 判断当前是否为鸿蒙系统
     *
     * @return 是否是鸿蒙系统，是：true，不是：false
     */
    public static boolean isHarmonyOs() {
        try {
            Class<?> buildExClass = Class.forName("com.huawei.system.BuildEx");
            Object osBrand = buildExClass.getMethod("getOsBrand").invoke(buildExClass);
            if (osBrand == null) {
                return false;
            }
            return "harmony".equalsIgnoreCase(osBrand.toString());
        } catch (Throwable e) {
            return false;
        }
    }

    /**
     * 获取鸿蒙系统 Version
     *
     * @return HarmonyOS Version
     */
    public static String getHarmonyOSVersion() {
        String version = null;
        if (isHarmonyOs()) {
            version = getProp("hw_sc.build.platform.version", "");
        }
        return version;
    }

    public static void filterMaps(String keyWord) {
        int pid = android.os.Process.myPid();
        try {
            FileReader fr = new FileReader(new File("proc/" + pid + "/maps"));
            BufferedReader br = new BufferedReader(fr);
            String line = null;
            Log.d(TAG, "Start.");
            while((line = br.readLine()) != null) {
                if (line.contains(keyWord)) {
                    Log.d(TAG, line);
                }
            }
            Log.d(TAG, "End.");
        }catch (Throwable t) {
            Log.d(TAG, "filterMaps", t);
        }
    }
}

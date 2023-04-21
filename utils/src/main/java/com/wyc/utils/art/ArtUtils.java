package com.wyc.utils.art;

import android.util.Log;

import com.wyc.utils.base.BaseUtils;

import dalvik.system.DelegateLastClassLoader;

public class ArtUtils {
    public static final String TAG = "AndCore.ArtUtils";

    /**
     *  自定义DelegateLastClassLoader
     * @param apkPath : apk路径
     * @param descriptor : 要寻找的class
     * @param keyWord : maps中过滤展示的关键字
     * */
    public static void testDelegateLastClassLoader(String apkPath, String descriptor, String keyWord) {
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O_MR1) {
            DelegateLastClassLoader classLoader =  new DelegateLastClassLoader(apkPath, "",
                    ClassLoader.getSystemClassLoader());
            try {
                Log.d(TAG, "Call Class.forName...");
                Class cls = Class.forName(descriptor, true, classLoader);
                Log.d(TAG, "Find cls [ " + cls + " ]");
                Log.d(TAG, "Filter /proc/self/maps");
                BaseUtils.filterMaps(keyWord);
            } catch (ClassNotFoundException e) {
                Log.d(TAG, "", e);
            }
        }
    }
}

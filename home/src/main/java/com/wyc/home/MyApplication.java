package com.wyc.home;

import android.app.Application;
import android.content.Context;

import com.wyc.utils.AndCoreRuntime;

import me.weishu.reflection.Reflection;

public class MyApplication extends Application {
    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(base);
        AndCoreRuntime.instance().init(MyApplication.this);
        Reflection.unseal(base);
    }
}

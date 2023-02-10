package com.wyc.utils.ipc;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;

public class LocalService extends Service {
    public static final String TAG = "AndCore.Service";
    private final IBinder binder = new InnerBinder();

    public LocalService() {
    }

    public class InnerBinder extends Binder {
        public LocalService getService() {
            return LocalService.this;
        }
    }

    @Override
    public IBinder onBind(Intent intent) {
        Log.d(TAG, "LocalService onBind");
        return binder;
    }

    public int getPid() {
        Log.d(TAG, "LocalService action");
        return android.os.Process.myPid();
    }
}

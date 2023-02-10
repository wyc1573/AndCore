package com.wyc.utils.ipc;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

public class LocalService extends Service {
    public static final String TAG = "AndCore.Service";
    public LocalService() {
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        throw new UnsupportedOperationException("Not yet implemented");
    }

    public void action() {
        Log.d(TAG, "LocalService action");
    }
}
package com.wyc.utils.ipc;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;

public class RemoteService extends Service {
    private final static String TAG = "AndCore.Service";
    private final IBinder binder = new InnerBinder();

    public RemoteService() {
    }

    public class InnerBinder extends Binder {
        RemoteService getService() {
            return RemoteService.this;
        }
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        throw new UnsupportedOperationException("Not yet implemented");
    }

    public void action() {
        Log.d(TAG, "RemoteService action");

    }

}
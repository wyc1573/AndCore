package com.wyc.utils.ipc;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;

public class RemoteService extends Service {
    private final static String TAG = "AndCore.Service";
    private final IBinder binder = new InnerBinder();

    public RemoteService() {
    }

    public class InnerBinder extends IRemoteService.Stub {
        @Override
        public void basicTypes(int anInt, long aLong, boolean aBoolean, float aFloat, double aDouble, String aString) throws RemoteException {
        }

        @Override
        public int getPid() throws RemoteException {
            Log.d(TAG, "remote getPid = " + android.os.Process.myPid());
            return android.os.Process.myPid();
        }
    }

    @Override
    public IBinder onBind(Intent intent) {
        Log.d(TAG, "RemoteService onBind");
        return binder;
    }
}

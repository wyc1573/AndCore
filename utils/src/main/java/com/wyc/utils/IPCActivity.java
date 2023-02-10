package com.wyc.utils;

import androidx.appcompat.app.AppCompatActivity;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import com.wyc.utils.ipc.IRemoteService;
import com.wyc.utils.ipc.LocalService;
import com.wyc.utils.ipc.RemoteService;

public class IPCActivity extends AppCompatActivity {
    private static final String TAG = "AndCore.Service";
    Button startRemote;
    Button remoteAction;
    Button startLocal;
    Button localAction;
    IRemoteService remoteService;
    LocalService localService;

    private ServiceConnection remoteConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName componentName, IBinder iBinder) {
            Log.d(TAG, "remote onServiceConnected");
            remoteService = IRemoteService.Stub.asInterface(iBinder);
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            Log.d(TAG, "remote onServiceDisconnected");
        }
    };

    private ServiceConnection localConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName componentName, IBinder iBinder) {
            Log.d(TAG, "local onServiceConnected");
            localService = ((LocalService.InnerBinder) iBinder).getService();
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            Log.d(TAG, "local onServiceDisconnected");
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.ipc_activity);
        startRemote = findViewById(R.id.utils_start_remote);
        remoteAction = findViewById(R.id.utils_remote_action);
        startLocal = findViewById(R.id.utils_start_local);
        localAction = findViewById(R.id.utils_local_action);
        startRemote.setOnClickListener(new StartRemoteClickListener());
        startLocal.setOnClickListener(new StartLocalClickListener());
        remoteAction.setOnClickListener(new RemoteActionClickListener());
        localAction.setOnClickListener(new LocalActionClickListener());
    }

    class StartRemoteClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            Log.d(TAG, "StartRemoteClickListener");
            Intent intent = new Intent(IPCActivity.this, RemoteService.class);
            bindService(intent, remoteConnection, Context.BIND_AUTO_CREATE);
        }
    }

    class RemoteActionClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            Log.d(TAG, "RemoteActionClickListener");
            if (remoteService == null) {
                Log.d(TAG, "remoteService is null, return!");
                return;
            }
            try {
                Log.d(TAG, "remote pid = " + remoteService.getPid());
            } catch (RemoteException e) {
                Log.d(TAG, "", e);
            }
        }
    }

    class StartLocalClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            Log.d(TAG, "StartLocalClickListener");
            Intent intent = new Intent(IPCActivity.this, LocalService.class);
            bindService(intent, localConnection, Context.BIND_AUTO_CREATE);
        }
    }

    class LocalActionClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            Log.d(TAG, "LocalActionClickListener");
            if (localService == null) {
                Log.d(TAG, "localService is null, return!");
                return;
            }
            Log.d(TAG, "local pid = " + localService.getPid());
        }
    }
}

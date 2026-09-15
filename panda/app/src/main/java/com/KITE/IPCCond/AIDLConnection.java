package com.KITE.IPCCond;

import android.content.ComponentName;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.util.Log;

import com.KITE.IPCCond.IMutual;

public class AIDLConnection implements ServiceConnection {
    public static IMutual iTestService;
    private final boolean isDaemon;

    public AIDLConnection(boolean b) {
         isDaemon = b;
    }

    @Override
    public void onServiceConnected(ComponentName name, IBinder service) {
        iTestService = IMutual.Stub.asInterface(service);
	    IPCService.InItIPC(iTestService);
        Log.d("Shocker", "Connected");
    }

    @Override
    public void onServiceDisconnected(ComponentName componentName) {
        Log.d("Shocker", "Disconnected");
    }  
	
}

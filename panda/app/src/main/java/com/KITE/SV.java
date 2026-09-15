package com.KITE;

import android.content.Context;
import android.graphics.PixelFormat;
import android.os.RemoteException;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.KITE.IPCCond.IPCService;

public class SV extends SurfaceView implements SurfaceHolder.Callback {
	
    public SV(Context context) {
        super(context);
        setZOrderOnTop(true);
        getHolder().setFormat(PixelFormat.TRANSPARENT);
        getHolder().addCallback(this);
    }

    @Override
    public void surfaceCreated(final SurfaceHolder holder) {
        Log.e("DrawView","surfaceCreated");
		holder.setType(SurfaceHolder.SURFACE_TYPE_GPU);
        try {
            IPCService.GetIPC().setSurface(holder.getSurface());
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
		try {
			IPCService.GetIPC().start(width,height);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }
}


package com.KITE;

import android.content.Context;
import android.graphics.PixelFormat;
import android.os.RemoteException;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.graphics.SurfaceTexture;
import android.view.TextureView;
import android.annotation.NonNull;

import com.KITE.IPCCond.IPCService;


public class TV extends TextureView implements TextureView.SurfaceTextureListener {
	
    public TV(Context context) {
        super(context);
        this.setOpaque(true);
        setSurfaceTextureListener(this);
    }
   
    @Override
    public void onSurfaceTextureAvailable(@NonNull SurfaceTexture surface, int width, int height) {
        try {
            if (MainActivity.app_Operation_mode_root) {
     			IPCService.GetIPC().start(width, height);
                IPCService.GetIPC().setSurface(new Surface(surface));
            } else {
     			SuperJNI.start(width, height);
                SuperJNI.setSurface(new Surface(surface));            
            }
       } catch (RemoteException e) {
            e.printStackTrace();
        }
        Log.e("TV", "surfaceCreated_");
    }

    @Override
    public void onSurfaceTextureSizeChanged(@NonNull SurfaceTexture surface, int width, int height) {
    }

    @Override
    public boolean onSurfaceTextureDestroyed(@NonNull SurfaceTexture surface) {
        Log.e("NDK-java", "onSurfaceTextureDestroyed"); //结束
        return false;
    }

    @Override
    public void onSurfaceTextureUpdated(@NonNull SurfaceTexture surface) {
        //Log.e("NDK-java", "onSurfaceTextureUpdated");
    }
    
    
}




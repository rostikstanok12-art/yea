package com.KITE;
import android.view.MotionEvent;
import android.view.Surface;
import java.util.UUID;

public class SuperJNI {
    public static native void setSurface(Surface surface);
    public static native String start(int ScreenX, int ScreenY);
    public static native void MotionEventClick(int Event_getAction, float PosX, float PosY);
    public static native float[] GetImGuiwinsize();
    
    public static native void setPid(int pid);
	public static native void setKey(String key);
	public static native void setUUid(String UUID);
}

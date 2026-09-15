package com.KITE;

public class SuperJNI {
    static { System.loadLibrary("main"); }
    public static native String start(int width, int height);
    public static native void setSurface(android.view.Surface surface);
    public static native void setPid(int pid);
    public static native void setUUid(String uuid);
    public static native void setKey(String key);
    public static native void MotionEventClick(int action, float x, float y);
    public static native float[] GetImGuiwinsize();
}

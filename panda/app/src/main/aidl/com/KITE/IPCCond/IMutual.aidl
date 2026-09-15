package com.KITE.IPCCond;


interface IMutual {
    void setSurface(in Surface surface);
    String start(int ScreenX, int ScreenY);
    void MotionEventClick(int Event_getAction, float PosX, float PosY);
    float[] GetImGuiwinsize();
    void setPid(int pid);
    void setKey(String key);
	void setUUid(String UUID);
}

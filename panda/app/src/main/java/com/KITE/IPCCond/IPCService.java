package com.KITE.IPCCond;

import com.KITE.IPCCond.IMutual;

public class IPCService {
    private static IMutual ipc;

    public static boolean isConnect() {
        return ipc != null;
    }

    public static void InItIPC(IMutual ipc) {
        IPCService.ipc = ipc;
    }

    public static IMutual GetIPC() {
        return ipc;
    }
}

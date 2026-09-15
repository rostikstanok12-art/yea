#pragma once
#include "memory.h"
#include "oxide_offsets_bluestacks.h"
#include <android/log.h>
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "BlueStacks", __VA_ARGS__)

// BlueStacks specific helpers for Oxide
class BlueStacksHelper {
public:
    Memory* mem;
    uintptr_t il2cppBase = 0;

    BlueStacksHelper(Memory* m) : mem(m) {
        il2cppBase = m->get_il2cpp_base();
    }

    // For BlueStacks, get PlayerManager class static fields
    // Il2CppClass structure (simplified for il2cpp version used in Oxide)
    // class->static_fields is at offset 0xB8 or 0xC0 depending on version
    // We try to brute force find it via scanning
    uintptr_t getStaticFieldsForClass(const char* className) {
        // TODO: implement proper il2cpp class lookup
        // For now, we will try to find class via il2cpp API if we're internal
        // For external BlueStacks, we need to parse GlobalMetadata.dat
        // Placeholder: return 0
        return 0;
    }

    // BlueStacks: get player list via static field 0x10
    // clientPlayerList is List<PlayerManager>
    struct PlayerList {
        uintptr_t listAddr;
        int size;
        uintptr_t itemsArray;
    };

    PlayerList getClientPlayerList() {
        PlayerList pl = {0,0,0};
        // This will be filled after we implement static fields parsing
        // For testing on BlueStacks, we can hardcode address found via CE
        // Example: search for list object in memory
        return pl;
    }

    // BlueStacks: check if running on BlueStacks emulator
    static bool isBlueStacks() {
        // Check build properties
        char brand[128] = {0};
        FILE* fp = fopen("/system/build.prop", "r");
        if (!fp) return false;
        char line[512];
        bool isBS = false;
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, "bluestacks") || strstr(line, "BlueStacks") || strstr(line, "bst")) {
                isBS = true;
                break;
            }
        }
        fclose(fp);
        return isBS;
    }

    // BlueStacks: ensure selinux permissive
    static bool ensurePermissive() {
        // Try to setenforce 0 via su
        int ret = system("su -c setenforce\\ 0");
        // Check getenforce
        FILE* fp = popen("getenforce", "r");
        if (!fp) return false;
        char buf[64] = {0};
        fgets(buf, sizeof(buf), fp);
        pclose(fp);
        LOGD("getenforce: %s", buf);
        return strstr(buf, "Permissive") != nullptr;
    }

    // BlueStacks: get package PID via pidof (works with root)
    static int getPidViaPidof(const char* pkg) {
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "su -c pidof\\ %s", pkg);
        FILE* fp = popen(cmd, "r");
        if (!fp) return -1;
        char buf[64] = {0};
        fgets(buf, sizeof(buf), fp);
        pclose(fp);
        return atoi(buf);
    }
};

// For BlueStacks OpenGL, we need to hook eglSwapBuffers to ensure ImGui draws
// This is handled in main.cpp already via EGL

#pragma once
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <android/native_window_jni.h>
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_android.h"
#include "imgui_Input_android.h"

extern ImGuiWindow* Window;
extern ANativeWindow* window_sv;
extern EGLDisplay g_EglDisplay;
extern EGLSurface g_EglSurface;
extern EGLContext g_EglContext;
extern int _ScreenX, _ScreenY;
extern int abs_ScreenX, abs_ScreenY;
extern bool g_Initialized;

void initGui();
void initializeEGLContext();

// Oxide specific config - adapted from panda
struct OxideSwitch {
    // ESP
    bool esp = true;
    bool box = true;
    bool line = true;
    bool distance = true;
    bool name = true;
    bool health = true;
    bool skeleton = false;
    bool radar = true;
    bool sleeping = false;
    bool showTeam = false;
    // Aim
    bool aimbot = false;
    bool aimCircle = true;
    bool aimClosest = true; // true = crosshair closest, false = distance closest
    bool aimOnScope = false; // only when scoped
    // Weapons
    bool noRecoil = false;
    bool noSpread = false;
    bool fastReload = false;
    bool infiniteAmmo = false;
    // Movement
    bool speedHack = false;
    bool noFall = false;
    bool highJump = false;
    // World
    bool espVehicles = false;
    bool espContainers = false;
    bool espAirdrop = false;
};

struct OxideConfig {
    float aimFov = 290.0f;
    float aimDistance = 300.0f;
    float espMaxDistance = 400.0f;
    float radarX = 300.0f;
    float radarY = 500.0f;
    float aimOffsetY = 0.2f;
    float aimSmooth = 5.0f;
    float boxThickness = 2.0f;
    float textSize = 18.0f;
};

extern OxideSwitch g_Switch;
extern OxideConfig g_Config;

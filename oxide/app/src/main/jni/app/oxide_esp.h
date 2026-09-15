#pragma once
#include "memory.h"
#include "oxide_sdk.h"
#include "config.h"
#include <vector>

class OxideESP {
public:
    Memory mem;
    uintptr_t il2cppBase = 0;
    uintptr_t unityBase = 0;
    uintptr_t localPlayer = 0;
    std::vector<OxidePlayer> players;
    OxideCamera camera;
    bool initialized = false;

    void Init(int pid);
    void Update();
    void Render();

    // External reading helpers
    Vector3 getPlayerPos(uintptr_t playerAddr);
    float getPlayerHealth(uintptr_t playerAddr);
    std::string getPlayerName(uintptr_t playerAddr);
    bool getViewMatrix(float* outMatrix);
    Vector3 getCameraPos();

    // For emulator OpenGL - we read camera from PlayerManager worldCameraRoot
    // worldCameraRoot -> Transform -> position
    // viewMatrix from Camera.main

    // List parsing
    std::vector<uintptr_t> getPlayerList(bool activeOnly = true);

    // Aimbot
    OxidePlayer* getClosestTarget(float& outDist, float& outCrossDist);
    void doAimbot(OxidePlayer* target);

    // Weapon mods
    void applyNoRecoil(uintptr_t weaponConfig);
    void applyNoSpread(uintptr_t weaponConfig);
};

extern OxideESP g_OxideESP;

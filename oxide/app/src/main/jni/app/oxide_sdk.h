#pragma once
#include <cstdint>
#include <string>
#include "Vector3.hpp"
#include "oxide_offsets.h"

// Minimal Unity & IL2CPP structs for Oxide
// For external reading via process_vm_readv

struct UnityVec3 {
    float x, y, z;
    operator Vector3() const { return Vector3(x, y, z); }
};

struct UnityVec2 {
    float x, y;
};

// Player list handling
struct OxidePlayer {
    uintptr_t address; // PlayerManager instance
    Vector3 position;
    Vector3 lastTickPos;
    float health;
    float maxHealth;
    bool isSleeping;
    bool isLocal;
    std::string name;
    float distance;
    // screen pos for ESP
    float screenX, screenY;
    float screenW, screenH;
    bool isVisible;
};

// Camera
struct OxideCamera {
    Vector3 position;
    float viewMatrix[16];
    float fov;
};

// Weapon config read
struct OxideWeaponConfig {
    float damage;
    float maxDistance;
    float bulletSpeed;
    float fireRate;
    float baseDispersion;
    // recoil
    Vector3 minRecoil;
    Vector3 maxRecoil;
    float recoilAimMult;
};

// For il2cpp internal hooking
// These are function pointers we can resolve from libil2cpp.so
namespace Il2CppAPI {
    // Domain & assembly
    using il2cpp_domain_get_t = void* (*)();
    using il2cpp_thread_attach_t = void* (*)(void* domain);
    using il2cpp_domain_assembly_open_t = void* (*)(void* domain, const char* name);
    using il2cpp_assembly_get_image_t = void* (*)(void* assembly);
    using il2cpp_class_from_name_t = void* (*)(void* image, const char* namespaze, const char* name);
    using il2cpp_class_get_field_from_name_t = void* (*)(void* klass, const char* name);
    using il2cpp_field_static_get_value_t = void (*)(void* field, void* value);
    using il2cpp_field_get_value_t = void (*)(void* obj, void* field, void* value);
    using il2cpp_class_get_method_from_name_t = void* (*)(void* klass, const char* name, int paramCount);
    using il2cpp_runtime_invoke_t = void* (*)(void* method, void* obj, void** params, void** exc);
}

// Helper to get libil2cpp base and resolve exports
// Used for emulator (x86_64) and arm64
struct Il2CppResolver {
    uintptr_t il2cppBase = 0;
    uintptr_t unityBase = 0;
    void* handle = nullptr;

    bool init(const char* libName = "libil2cpp.so");
    uintptr_t getExport(const char* name);
    uintptr_t getRVA(uintptr_t rva) { return il2cppBase + rva; }
};

// For OpenGL emulator: W2S using view matrix from camera
// Standard Unity W2S
inline bool WorldToScreen(const Vector3& worldPos, Vector3& screenPos, float* viewMatrix, int screenWidth, int screenHeight) {
    // viewMatrix is 4x4
    float clipX = viewMatrix[0] * worldPos.X + viewMatrix[1] * worldPos.Y + viewMatrix[2] * worldPos.Z + viewMatrix[3];
    float clipY = viewMatrix[4] * worldPos.X + viewMatrix[5] * worldPos.Y + viewMatrix[6] * worldPos.Z + viewMatrix[7];
    float clipZ = viewMatrix[8] * worldPos.X + viewMatrix[9] * worldPos.Y + viewMatrix[10] * worldPos.Z + viewMatrix[11];
    float clipW = viewMatrix[12] * worldPos.X + viewMatrix[13] * worldPos.Y + viewMatrix[14] * worldPos.Z + viewMatrix[15];

    if (clipW < 0.1f) return false;

    float ndcX = clipX / clipW;
    float ndcY = clipY / clipW;
    float ndcZ = clipZ / clipW;

    screenPos.X = (screenWidth / 2.0f) * (1.0f + ndcX);
    screenPos.Y = (screenHeight / 2.0f) * (1.0f - ndcY);
    screenPos.Z = ndcZ;
    return true;
}

// Transform get position external (Unity Transform hierarchy)
// For Oxide we can read via Transform internal: 0x10 -> TransformAccess etc
// Simplified: PlayerManager -> characterModel (GameObject) -> Transform -> position
// We'll implement in oxide_esp.cpp

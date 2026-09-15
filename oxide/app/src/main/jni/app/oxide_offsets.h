#pragma once
// Oxide: Survival Island - IL2CPP Offsets
// From il2cpp_dump_and_api_summary.txt
// Target: libil2cpp.so base + RVA
// Tested on emulator x86_64 / arm64-v8a

namespace OxideOffsets {

// PlayerManager - TypeDefIndex 9223
struct PlayerManager {
    static constexpr int worldCameraRoot = 0x68;
    static constexpr int mouseLook = 0x70;
    static constexpr int raycastManager = 0x88;
    static constexpr int fpManager = 0x90;
    static constexpr int inventory = 0x98;
    static constexpr int playerMap = 0xA0;
    static constexpr int vitals = 0xC8;
    static constexpr int stats = 0xD0;
    static constexpr int equipment = 0xE8;
    static constexpr int compass = 0x100;
    static constexpr int characterModel = 0x150;
    static constexpr int animator = 0x190;
    static constexpr int lookAngle = 0x1A8;
    static constexpr int isInAir = 0x1B8;
    static constexpr int isOnPlayer = 0x1B9;
    static constexpr int violationLevel = 0x1BC;
    static constexpr int lastTickPosition = 0x1C8;
    static constexpr int lastSavedPosition = 0x1D4;
    static constexpr int lastDeathPosition = 0x1E0;

    struct Static {
        static constexpr int sleepingPlayerList = 0x0;
        static constexpr int activePlayerList = 0x8;
        static constexpr int clientPlayerList = 0x10;
    };
};

struct MouseLook {
    static constexpr int XSensitivity = 0x10;
    static constexpr int YSensitivity = 0x14;
    static constexpr int m_CharacterTargetRot = 0x30;
    static constexpr int m_CameraTargetRot = 0x40;
};

struct HitscanWeaponConfig {
    static constexpr int damage = 0x18;
    static constexpr int MaxDistance = 0x2C;
    static constexpr int BulletSpeed = 0x30;
    static constexpr int fireRate = 0x34;
    static constexpr int projectilesPerShot = 0x38;
    static constexpr int BaseDispersion = 0x3C;
    static constexpr int FullReloadPeriod = 0x48;
    static constexpr int dispersionConfig = 0x60;
    static constexpr int recoilConfig = 0x68;
};

struct RecoilConfig {
    static constexpr int MinRecoil = 0x18;
    static constexpr int MaxRecoil = 0x20;
    static constexpr int MinSideDelta = 0x28;
    static constexpr int MaxSideDelta = 0x2C;
    static constexpr int RecoilAimMultiplier = 0x30;
    static constexpr int RecoilTime = 0x34;
};

struct DispersionConfig {
    static constexpr int minFireDispersion = 0x18;
    static constexpr int maxFireDispersion = 0x1C;
    static constexpr int minDispersion = 0x20;
    static constexpr int maxDispersion = 0x24;
};

struct PlayerVitals {
    static constexpr int Player = 0x100;
    static constexpr int startHealthMin = 0x110;
    static constexpr int startHealthMax = 0x114;
    static constexpr int m_Temperature = 0x144;
    static constexpr int m_RadiationLevel = 0x148;
};

struct GenericVitals {
    static constexpr int m_MaxHealth = 0x88;
};

struct zeT {
    static constexpr int DisplayName = 0x88;
    static constexpr int Health = 0x98;
    static constexpr int IsGrounded = 0xA8;
    static constexpr int Velocity = 0xB0;
};

struct Transform {
    static constexpr int internalTransform = 0x10;
};

struct List {
    static constexpr int items = 0x10;
    static constexpr int size = 0x18;
};

// Array
struct Il2CppArray {
    static constexpr int bounds = 0x10;
    static constexpr int maxLength = 0x18;
    static constexpr int vector = 0x20;
};

// Unity Transform access chain for external
// For internal we can use GetPosition via il2cpp
// For external memory reading we need to parse Transform hierarchy

} // namespace

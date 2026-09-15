#pragma once
// Oxide: Survival Island - Offsets for BlueStacks (x86_64 + arm64-v8a)
// From il2cpp_dump_and_api_summary.txt - verified for BlueStacks 5 + MuMu 12
// Base = libil2cpp.so - use adb shell to get base via /proc/pid/maps

// BlueStacks specific notes:
// - BlueStacks 5 runs Android 11, libil2cpp.so is x86_64 (houdini translation for arm64 games) or arm64-v8a native
// - With adb root, /proc/pid/maps readable, process_vm_readv works with selinux Permissive
// - Package names: com.oxide.survivalisland, com.oxide.survival, com.catsbit.oxide (check with pm list)

namespace OxideBS {

    // Core offsets - same as emulator but with BlueStacks notes
    struct PlayerManager {
        // Instance fields
        static constexpr int worldCameraRoot = 0x68;      // Transform - camera root
        static constexpr int mouseLook = 0x70;            // MouseLook
        static constexpr int raycastManager = 0x88;
        static constexpr int fpManager = 0x90;
        static constexpr int inventory = 0x98;            // PlayerInventory
        static constexpr int playerMap = 0xA0;
        static constexpr int vitals = 0xC8;               // PlayerVitals
        static constexpr int stats = 0xD0;
        static constexpr int equipment = 0xE8;            // zbZ equipment - weapon
        static constexpr int compass = 0x100;
        static constexpr int characterModel = 0x150;      // GameObject
        static constexpr int animator = 0x190;
        static constexpr int lookAngle = 0x1A8;           // float
        static constexpr int isInAir = 0x1B8;             // bool
        static constexpr int lastTickPosition = 0x1C8;    // Vector3 - main pos for ESP
        static constexpr int lastSavedPosition = 0x1D4;
        static constexpr int lastDeathPosition = 0x1E0;

        // Static fields - in Il2CppClass->static_fields
        // For BlueStacks external reading:
        // 1. Find Il2CppClass for PlayerManager via il2cpp_class_from_name
        // 2. static_fields pointer at class + 0x... (depends on il2cpp version)
        // 3. clientPlayerList = static_fields + 0x10
        struct Static {
            static constexpr int sleepingPlayerList = 0x0;   // iL<PlayerManager>
            static constexpr int activePlayerList = 0x8;     // iL<PlayerManager>
            static constexpr int clientPlayerList = 0x10;    // List<PlayerManager> - use this for ESP
            static constexpr int serverName = 0x18;
            static constexpr int serverBuildNumber = 0x20;
        };

        // For BlueStacks: RVA to get class (from script.json)
        // These RVAs are from dump.cs - add to libil2cpp base
        struct RVA {
            static constexpr uintptr_t get_activePlayerList = 0x0; // TODO fill from script.json
            static constexpr uintptr_t get_clientPlayerList = 0x0;
        };
    };

    struct PlayerInventory {
        static constexpr int player = 0x20;
        static constexpr int quickSlots = 0x30;
    };

    struct PlayerVitals {
        static constexpr int Player = 0x100;
        static constexpr int startHealthMin = 0x110;
        static constexpr int startHealthMax = 0x114;
        static constexpr int m_Temperature = 0x144;
        static constexpr int m_RadiationLevel = 0x148;
        // Health is in zeT at 0x98 - need chain
    };

    struct GenericVitals {
        static constexpr int m_MaxHealth = 0x88;
        static constexpr int m_Protection = 0x90;
        static constexpr int m_Hitboxes = 0xA8;
    };

    struct zeT {
        static constexpr int DisplayName = 0x88;      // string
        static constexpr int Health = 0x98;           // zeu<float> - read float inside
        static constexpr int IsGrounded = 0xA8;
        static constexpr int Velocity = 0xB0;         // Vector3
    };

    // Weapon - for NoRecoil/NoSpread on BlueStacks
    struct HitscanWeaponConfig {
        static constexpr int damage = 0x18;           // float - one shot damage
        static constexpr int MaxDamageOnDistance = 0x1C; // Vector2
        static constexpr int MinDamageOnDistance = 0x24; // Vector2
        static constexpr int MaxDistance = 0x2C;      // float - max range
        static constexpr int BulletSpeed = 0x30;      // float
        static constexpr int fireRate = 0x34;         // float - set to 0.01 for fast
        static constexpr int projectilesPerShot = 0x38; // int
        static constexpr int BaseDispersion = 0x3C;   // float - set 0 for no spread
        static constexpr int FullReloadPeriod = 0x48; // float - set 0 for instant reload
        static constexpr int dispersionConfig = 0x60; // DispersionConfig*
        static constexpr int recoilConfig = 0x68;     // RecoilConfig*
    };

    struct RecoilConfig {
        static constexpr int MinRecoil = 0x18;        // Vector2
        static constexpr int MaxRecoil = 0x20;        // Vector2
        static constexpr int MinSideDelta = 0x28;     // float
        static constexpr int MaxSideDelta = 0x2C;     // float
        static constexpr int RecoilAimMultiplier = 0x30; // float - set 0
        static constexpr int RecoilTime = 0x34;
    };

    struct DispersionConfig {
        static constexpr int minFireDispersion = 0x18;
        static constexpr int maxFireDispersion = 0x1C;
        static constexpr int minDispersion = 0x20;    // set 0
        static constexpr int maxDispersion = 0x24;    // set 0
        static constexpr int dispersionIncreasePerShot = 0x28;
        static constexpr int dispersionDecreaseRate = 0x2C;
    };

    struct MouseLook {
        static constexpr int XSensitivity = 0x10;
        static constexpr int YSensitivity = 0x14;
        static constexpr int MinimumX = 0x1C;
        static constexpr int MaximumX = 0x20;
        static constexpr int m_CharacterTargetRot = 0x30; // Quaternion
        static constexpr int m_CameraTargetRot = 0x40;    // Quaternion
    };

    // Unity List
    struct List {
        static constexpr int items = 0x10; // Il2CppArray*
        static constexpr int size = 0x18;  // int
    };

    struct Il2CppArray {
        static constexpr int maxLength = 0x18;
        static constexpr int vector = 0x20; // first element
    };

    // Transform - for W2S on BlueStacks
    // BlueStacks OpenGL: libunity.so Camera.main -> worldToCameraMatrix
    struct Transform {
        // For internal: use Transform::GetPosition
        // For external BlueStacks: need to read Transform hierarchy
        // TransformAccess at 0x10, then matrix
        static constexpr int internalTransform = 0x10;
    };

    // Camera - for view matrix on BlueStacks
    struct Camera {
        // In Unity, Camera has view matrix
        // For BlueStacks external, find MainCamera via Tag
        // Or read from PlayerManager worldCameraRoot (Transform) + Camera component
        static constexpr int m_CachedPtr = 0x10;
        static constexpr int worldToCameraMatrix = 0x2C0; // approximate, check dump
        static constexpr int projectionMatrix = 0x300;
    };
}

// BlueStacks specific memory helpers
namespace BlueStacksHelper {
    // BlueStacks 5 default ports
    static constexpr const char* DEFAULT_PORTS[] = {
        "127.0.0.1:5555",   // BlueStacks 5 default
        "127.0.0.1:5565",
        "127.0.0.1:5575",
        "127.0.0.1:5585",
        "127.0.0.1:7555",   // MuMu 12
        "127.0.0.1:16416",  // MuMu 12 second instance
        "127.0.0.1:62001",  // Nox
        "127.0.0.1:62025"   // Nox second
    };

    // Package names to try for Oxide on BlueStacks
    static constexpr const char* OXIDE_PACKAGES[] = {
        "com.oxide.survivalisland",
        "com.oxide.survival",
        "com.catsbit.oxide",
        "com.oxide.survivalisland.overseas"
    };
}

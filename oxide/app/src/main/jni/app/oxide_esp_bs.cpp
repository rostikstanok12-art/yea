#include "oxide_esp.h"
#include "oxide_offsets_bluestacks.h"
#include "bluestacks_helper.h"
#include <android/log.h>

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "OxideBS", __VA_ARGS__)

// BlueStacks specific implementation with offsets from dump

void OxideESP_BlueStacks_Init(Memory& mem) {
    // For BlueStacks, check selinux and root
    BlueStacksHelper::ensurePermissive();
    bool isBS = BlueStacksHelper::isBlueStacks();
    LOGD("Running on BlueStacks: %d", isBS);

    uintptr_t il2cpp = mem.get_il2cpp_base();
    uintptr_t unity = mem.get_unity_base();
    LOGD("BlueStacks il2cpp base: %lx unity base: %lx", il2cpp, unity);

    // Try to find Oxide PID via all package names
    for (auto pkg : BlueStacksHelper::OXIDE_PACKAGES) {
        int pid = BlueStacksHelper::getPidViaPidof(pkg);
        if (pid > 0) {
            LOGD("Found Oxide pkg %s pid %d", pkg, pid);
        }
    }
}

// Example of using BlueStacks offsets for weapon mods
void ApplyNoRecoil_BlueStacks(Memory& mem, uintptr_t weaponConfig) {
    if (!weaponConfig) return;
    uintptr_t recoil = mem.getPtr(weaponConfig + OxideBS::HitscanWeaponConfig::recoilConfig);
    if (!recoil) return;
    
    // Zero recoil for BlueStacks
    mem.writeFloat(recoil + OxideBS::RecoilConfig::MinRecoil, 0.0f);
    mem.writeFloat(recoil + OxideBS::RecoilConfig::MinRecoil + 4, 0.0f);
    mem.writeFloat(recoil + OxideBS::RecoilConfig::MaxRecoil, 0.0f);
    mem.writeFloat(recoil + OxideBS::RecoilConfig::MaxRecoil + 4, 0.0f);
    mem.writeFloat(recoil + OxideBS::RecoilConfig::RecoilAimMultiplier, 0.0f);
    mem.writeFloat(recoil + OxideBS::RecoilConfig::MinSideDelta, 0.0f);
    mem.writeFloat(recoil + OxideBS::RecoilConfig::MaxSideDelta, 0.0f);
    
    LOGD("Applied NoRecoil to %lx", weaponConfig);
}

void ApplyNoSpread_BlueStacks(Memory& mem, uintptr_t weaponConfig) {
    if (!weaponConfig) return;
    uintptr_t disp = mem.getPtr(weaponConfig + OxideBS::HitscanWeaponConfig::dispersionConfig);
    if (!disp) return;
    
    mem.writeFloat(disp + OxideBS::DispersionConfig::minDispersion, 0.0f);
    mem.writeFloat(disp + OxideBS::DispersionConfig::maxDispersion, 0.0f);
    mem.writeFloat(disp + OxideBS::DispersionConfig::minFireDispersion, 0.0f);
    mem.writeFloat(disp + OxideBS::DispersionConfig::maxFireDispersion, 0.0f);
    mem.writeFloat(weaponConfig + OxideBS::HitscanWeaponConfig::BaseDispersion, 0.0f);
    
    LOGD("Applied NoSpread to %lx", weaponConfig);
}

// BlueStacks W2S - using Camera.main view matrix
// For BlueStacks OpenGL, we need to read view matrix from Unity Camera
bool GetViewMatrix_BlueStacks(Memory& mem, float* outMatrix) {
    // Attempt 1: Find MainCamera via Unity tag
    // Attempt 2: Read from PlayerManager worldCameraRoot Transform
    // For now, placeholder - in real implementation, parse CameraManager
    
    // For BlueStacks emulator, view matrix is often at:
    // Camera.main + 0x2C0 (worldToCamera) and 0x300 (projection)
    // We try to find Camera.main object via il2cpp static
    
    // TODO: implement proper Camera.main lookup
    return false;
}

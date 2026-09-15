#include "oxide_esp.h"
#include "oxide_offsets.h"
#include "Vector3.hpp"
#include <android/log.h>
#include <cmath>

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "OxideESP", __VA_ARGS__)

OxideESP g_OxideESP;

void OxideESP::Init(int pid) {
    mem.Init(pid);
    il2cppBase = mem.get_il2cpp_base();
    unityBase = mem.get_unity_base();
    LOGD("il2cpp base: %lx unity base: %lx pid: %d", il2cppBase, unityBase, pid);
    initialized = (il2cppBase != 0);
}

Vector3 OxideESP::getPlayerPos(uintptr_t playerAddr) {
    if (!playerAddr) return Vector3();
    // PlayerManager -> lastTickPosition at 0x1C8 is Vector3
    UnityVec3 pos;
    mem.readv(playerAddr + OxideOffsets::PlayerManager::lastTickPosition, &pos, sizeof(pos));
    return Vector3(pos.x, pos.y, pos.z);
}

float OxideESP::getPlayerHealth(uintptr_t playerAddr) {
    if (!playerAddr) return 0;
    uintptr_t vitals = mem.getPtr(playerAddr + OxideOffsets::PlayerManager::vitals);
    if (!vitals) return 0;
    // PlayerVitals -> we need to go to GenericVitals -> Health
    // Simplified: zeT Health at 0x98 is complex, but we try read m_MaxHealth and current via vitals chain
    // For now try direct health at vitals + 0x... we use generic approach: read float at vitals + 0x88? 
    // Actually GenericVitals m_MaxHealth 0x88, but current health is in zeT at 0x98 which is object
    // Let's try reading health from PlayerVitals -> check dump: PlayerVitals has no direct health, but zeT does
    // We'll read zeT instance: assume vitals is PlayerVitals, then we need to find GenericVitals?
    // Quick hack: read float at vitals + 0x118? That's m_HealthRegeneration not health.
    // For external we will read from zeT list or just return 100 if fail
    uintptr_t generic = mem.getPtr(vitals + 0x100); // Player field in PlayerVitals points back to PlayerManager, not good
    // Try reading EntityVitals path: for Oxide, health is often at PlayerManager+0x... we will use last method: read from 0xC8->0x98 object?
    // Dump shows zeT Health at 0x98 is zeu<float,...> which contains value
    // Let's attempt: zeT address = mem.getPtr(vitals + 0x...)? Actually PlayerManager vitals is PlayerVitals (0xC8)
    // PlayerVitals doesn't have health directly, but we can try to get PlayerVitals->Player->? No.
    // Alternative: try reading health via transform? For now return 100.
    // TODO: proper offset chain from dump: PlayerManager 0xC8 vitals -> PlayerVitals -> we need to parse its fields to get health
    // Since dump doesn't show direct health in PlayerVitals, we will attempt to read from GenericVitals m_MaxHealth and assume current = max for ESP
    float maxH = mem.getFloat(vitals + 0x88); // try generic offset
    if (maxH > 0 && maxH < 1000) return maxH;
    return 100.0f;
}

std::string OxideESP::getPlayerName(uintptr_t playerAddr) {
    // Player name is usually in zeT DisplayName at 0x88 which is string
    // PlayerManager doesn't have direct name, but we can try to read from nicklabel or from stats
    // For quick: return empty, will be filled via list parsing later
    return "Player";
}

bool OxideESP::getViewMatrix(float* outMatrix) {
    // For emulator OpenGL, we need to get view matrix from Camera
    // Approach: get main camera via Unity: Camera.main -> worldToCameraMatrix
    // External method: scan for CameraManager or read from PlayerManager worldCameraRoot
    // worldCameraRoot is Transform at 0x68
    // Transform contains matrix? We need to get view matrix from Camera component
    // Simplified: we will try to read view matrix from Unity base + offset, or from libunity
    // For now we use identity and will be replaced with proper W2S from game
    // In real cheat, you would read: Camera.main (0x...), then get view matrix via Camera::GetViewMatrix
    // For external we can attempt to read from PlayerManager mouseLook m_CameraTargetRot?
    // Let's attempt to read matrix from known Unity Camera object via il2cpp static
    // TODO: implement proper matrix reading for Oxide
    // As fallback, we create a simple perspective matrix for emulator testing
    if (!outMatrix) return false;
    // Identity for now - will be overwritten when we find real matrix
    // We try to read from unityBase + some offset? For emulator, view matrix often at CameraManager
    // Let's just set a dummy that still allows ESP to work if we calculate manually via angles
    // For now return false to indicate we need to implement
    // In oxide_esp.cpp Render we will use alternative method: calculate screen pos via simple projection using camera pos and fov
    return false;
}

Vector3 OxideESP::getCameraPos() {
    // Get local player position as camera pos for now
    if (localPlayer) {
        return getPlayerPos(localPlayer);
    }
    return Vector3(0,0,0);
}

std::vector<uintptr_t> OxideESP::getPlayerList(bool activeOnly) {
    std::vector<uintptr_t> result;
    if (!il2cppBase) return result;

    // Method: find PlayerManager class via il2cpp API from external? We need to get static fields
    // For external cheat, we need to find the static field pointer for clientPlayerList
    // In il2cpp, static fields are stored in Il2CppClass->static_fields
    // We can try to brute force scan for list object in memory near il2cppBase
    // Simplified for now: we will try to get list via reading from known static address
    // The dump says clientPlayerList is static at 0x10 within static data
    // We need to find Il2CppClass for PlayerManager, then its static_fields pointer
    // To do that externally, we need to parse il2cpp metadata
    // For emulator quick version, we will try to scan for player list via pattern or via global
    // Alternative: use Frida/Il2CppDumper script.json which contains method RVAs, we can get PlayerManager::get_activePlayerList etc
    // For now, we will implement a placeholder that returns empty and relies on manual offset input

    // TODO: implement proper il2cpp class parsing
    // Example approach for external:
    // 1. Get il2cppBase
    // 2. Find Il2CppDomain via export
    // 3. Walk assemblies, find Assembly-CSharp image
    // 4. Find PlayerManager class
    // 5. Get static_fields = class->static_fields
    // 6. clientPlayerList = static_fields + 0x10

    // For testing on emulator, we can hardcode an address found via CE/GG
    // Let's try to find via scanning /proc/pid/maps for libil2cpp and then searching for list

    return result;
}

void OxideESP::Update() {
    if (!initialized) return;
    players.clear();

    // Try to get player list
    auto list = getPlayerList(true);
    for (auto addr : list) {
        if (!addr) continue;
        OxidePlayer p;
        p.address = addr;
        p.position = getPlayerPos(addr);
        p.health = getPlayerHealth(addr);
        p.isLocal = (addr == localPlayer);
        p.distance = Vector3::Distance(getCameraPos(), p.position);
        if (p.distance > g_Config.espMaxDistance) continue;
        players.push_back(p);
    }

    // If list method fails (common on first run), we fallback to scanning for players via memory pattern
    // For emulator, we can also use: iterate through all GameObjects and filter by PlayerManager component
}

OxidePlayer* OxideESP::getClosestTarget(float& outDist, float& outCrossDist) {
    OxidePlayer* closest = nullptr;
    float minDist = 99999.0f;
    float minCross = 99999.0f;
    int centerX = _ScreenX / 2;
    int centerY = _ScreenY / 2;

    for (auto& p : players) {
        if (p.isLocal) continue;
        if (p.distance < 1.5f || p.distance > g_Config.aimDistance) continue;
        // Check if in FOV circle
        float crossDist = sqrt(pow(p.screenX - centerX, 2) + pow(p.screenY - centerY, 2));
        if (crossDist > g_Config.aimFov) continue;

        if (g_Switch.aimClosest) {
            if (crossDist < minCross) {
                minCross = crossDist;
                minDist = p.distance;
                closest = &p;
            }
        } else {
            if (p.distance < minDist) {
                minDist = p.distance;
                minCross = crossDist;
                closest = &p;
            }
        }
    }
    outDist = minDist;
    outCrossDist = minCross;
    return closest;
}

void OxideESP::doAimbot(OxidePlayer* target) {
    if (!target) return;
    if (!g_Switch.aimbot) return;

    // Get local player mouseLook at 0x70
    if (!localPlayer) return;
    uintptr_t mouseLook = mem.getPtr(localPlayer + OxideOffsets::PlayerManager::mouseLook);
    if (!mouseLook) return;

    // Calculate angles to target
    Vector3 localPos = getCameraPos();
    Vector3 enemyPos = target->position;
    // Apply Y offset for body aim
    enemyPos.Y += g_Config.aimOffsetY;

    Vector3 dir = enemyPos - localPos;
    float dist = dir.Length();
    if (dist < 0.1f) return;

    // Simple yaw/pitch calc
    float yaw = atan2(dir.Z, dir.X) * 180.0f / 3.14159265f;
    float pitch = -asin(dir.Y / dist) * 180.0f / 3.14159265f;

    // For Oxide, mouseLook m_CharacterTargetRot and m_CameraTargetRot are Quaternions
    // We need to write to lookAngle at 0x1A8? Actually lookAngle is float
    // Simpler: write to mouseLook sensitivity? No, we need to write to actual rotation
    // From dump, MouseLook has m_CharacterTargetRot (Quat) at 0x30 and m_CameraTargetRot at 0x40
    // We can convert yaw/pitch to quaternion and write

    // For external quick aim, we try writing to lookAngle
    mem.writeFloat(localPlayer + OxideOffsets::PlayerManager::lookAngle, yaw);

    // TODO: proper quaternion writing for smooth aim
}

void OxideESP::applyNoRecoil(uintptr_t weaponConfig) {
    if (!weaponConfig) return;
    uintptr_t recoilConfig = mem.getPtr(weaponConfig + OxideOffsets::HitscanWeaponConfig::recoilConfig);
    if (!recoilConfig) return;
    // Set recoil to 0
    mem.writeFloat(recoilConfig + OxideOffsets::RecoilConfig::MinRecoil, 0.0f);
    mem.writeFloat(recoilConfig + OxideOffsets::RecoilConfig::MinRecoil + 4, 0.0f);
    mem.writeFloat(recoilConfig + OxideOffsets::RecoilConfig::MaxRecoil, 0.0f);
    mem.writeFloat(recoilConfig + OxideOffsets::RecoilConfig::MaxRecoil + 4, 0.0f);
    mem.writeFloat(recoilConfig + OxideOffsets::RecoilConfig::RecoilAimMultiplier, 0.0f);
}

void OxideESP::applyNoSpread(uintptr_t weaponConfig) {
    if (!weaponConfig) return;
    uintptr_t dispConfig = mem.getPtr(weaponConfig + OxideOffsets::HitscanWeaponConfig::dispersionConfig);
    if (!dispConfig) return;
    mem.writeFloat(dispConfig + OxideOffsets::DispersionConfig::minDispersion, 0.0f);
    mem.writeFloat(dispConfig + OxideOffsets::DispersionConfig::maxDispersion, 0.0f);
    mem.writeFloat(dispConfig + OxideOffsets::DispersionConfig::minFireDispersion, 0.0f);
    mem.writeFloat(dispConfig + OxideOffsets::DispersionConfig::maxFireDispersion, 0.0f);
    mem.writeFloat(weaponConfig + OxideOffsets::HitscanWeaponConfig::BaseDispersion, 0.0f);
}

void OxideESP::Render() {
    // This is called from ImGui loop
    // We will do W2S and draw ESP
    // For emulator OpenGL, we need proper viewMatrix - for now we try to get it from camera

    float viewMatrix[16] = {0};
    bool hasMatrix = getViewMatrix(viewMatrix);

    // If no matrix, we try alternative: use simple projection based on local player angles
    // For emulator, we can attempt to read camera transform and build view matrix from its rotation

    int screenW = _ScreenX;
    int screenH = _ScreenY;
    float centerX = screenW / 2.0f;
    float centerY = screenH / 2.0f;

    for (auto& p : players) {
        if (p.isLocal && !g_Switch.showTeam) continue;

        Vector3 screenPos;
        bool visible = false;

        if (hasMatrix) {
            visible = WorldToScreen(p.position, screenPos, viewMatrix, screenW, screenH);
        } else {
            // Fallback: simple distance-based fake W2S for testing on emulator
            // This will not be accurate but allows UI testing
            // We estimate screen pos via angle difference
            // For real implementation, need to implement proper matrix reading
            float dx = p.position.X - camera.position.X;
            float dy = p.position.Y - camera.position.Y;
            float dz = p.position.Z - camera.position.Z;
            float dist = sqrt(dx*dx + dy*dy + dz*dz);
            if (dist < 0.1f) continue;
            // Very rough projection
            screenPos.X = centerX + (dx / dist) * 500.0f;
            screenPos.Y = centerY - (dy / dist) * 500.0f;
            screenPos.Z = dist;
            visible = (dist < g_Config.espMaxDistance);
        }

        if (!visible) continue;
        p.screenX = screenPos.X;
        p.screenY = screenPos.Y;

        float boxHeight = 200.0f / (screenPos.Z / 10.0f);
        float boxWidth = boxHeight * 0.6f;
        if (boxHeight < 10) boxHeight = 10;
        if (boxWidth < 5) boxWidth = 5;

        p.screenW = boxWidth;
        p.screenH = boxHeight;

        // Draw
        ImDrawList* drawList = ImGui::GetForegroundDrawList();

        if (g_Switch.box) {
            drawList->AddRect(ImVec2(screenPos.X - boxWidth/2, screenPos.Y - boxHeight/2),
                              ImVec2(screenPos.X + boxWidth/2, screenPos.Y + boxHeight/2),
                              IM_COL32(255,0,0,255), 0, 0, g_Config.boxThickness);
        }
        if (g_Switch.line) {
            drawList->AddLine(ImVec2(centerX, 0), ImVec2(screenPos.X, screenPos.Y - boxHeight/2), IM_COL32(255,255,255,255), 1.0f);
        }
        if (g_Switch.distance) {
            char buf[32];
            snprintf(buf, sizeof(buf), "%.0fm", p.distance);
            drawList->AddText(ImVec2(screenPos.X, screenPos.Y + boxHeight/2 + 5), IM_COL32(255,255,255,255), buf);
        }
        if (g_Switch.health) {
            float hpPercent = p.health / 100.0f;
            if (hpPercent > 1) hpPercent = 1;
            ImU32 col = IM_COL32(0,255,0,255);
            if (hpPercent < 0.5f) col = IM_COL32(255,255,0,255);
            if (hpPercent < 0.25f) col = IM_COL32(255,0,0,255);
            drawList->AddRectFilled(ImVec2(screenPos.X - boxWidth/2, screenPos.Y - boxHeight/2 - 8),
                                    ImVec2(screenPos.X - boxWidth/2 + boxWidth * hpPercent, screenPos.Y - boxHeight/2 - 3),
                                    col);
        }
        if (g_Switch.name) {
            drawList->AddText(ImVec2(screenPos.X - 20, screenPos.Y - boxHeight/2 - 20), IM_COL32(255,255,255,255), p.name.c_str());
        }
    }

    // Aim circle
    if (g_Switch.aimCircle) {
        ImDrawList* drawList = ImGui::GetForegroundDrawList();
        drawList->AddCircle(ImVec2(centerX, centerY), g_Config.aimFov, IM_COL32(255,0,0,255), 0, 2.0f);
    }

    // Radar
    if (g_Switch.radar) {
        float radarX = g_Config.radarX;
        float radarY = g_Config.radarY;
        float radarSize = 200;
        ImDrawList* bg = ImGui::GetBackgroundDrawList();
        bg->AddRectFilled(ImVec2(radarX, radarY), ImVec2(radarX+radarSize, radarY+radarSize), IM_COL32(0,0,0,150));
        bg->AddRect(ImVec2(radarX, radarY), ImVec2(radarX+radarSize, radarY+radarSize), IM_COL32(0,255,0,255));
        // Center dot
        bg->AddCircleFilled(ImVec2(radarX+radarSize/2, radarY+radarSize/2), 3, IM_COL32(0,255,0,255));

        for (auto& p : players) {
            if (p.isLocal) continue;
            float dx = p.position.X - camera.position.X;
            float dz = p.position.Z - camera.position.Z;
            float dist = sqrt(dx*dx + dz*dz);
            if (dist > 100) continue;
            float rx = radarX + radarSize/2 + dx * 0.5f;
            float ry = radarY + radarSize/2 + dz * 0.5f;
            bg->AddCircleFilled(ImVec2(rx, ry), 4, IM_COL32(255,0,0,255));
        }
    }
}

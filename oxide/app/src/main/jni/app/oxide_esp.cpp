#include "oxide_esp.h"
#include "oxide_offsets.h"
#include "oxide_offsets_bluestacks.h"
#include "bluestacks_helper.h"
#include "il2cpp_parser.h"
#include "Vector3.hpp"
#include <android/log.h>
#include <cmath>
#include <cstdio>

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "OxideESP", __VA_ARGS__)

OxideESP g_OxideESP;

void OxideESP::Init(int pid) {
    mem.Init(pid);
    il2cppBase = mem.get_il2cpp_base();
    unityBase = mem.get_unity_base();
    LOGD("BlueStacks il2cpp base: %lx unity base: %lx pid: %d", il2cppBase, unityBase, pid);
    initialized = (il2cppBase != 0);

    // BlueStacks extra: ensure permissive
    BlueStacksHelper::ensurePermissive();
    
    // Try to read dump file from helper (no Frida method)
    FILE* f = fopen("/data/local/tmp/oxide_dump.txt", "r");
    if (f) {
        char line[256];
        while (fgets(line, sizeof(line), f)) {
            LOGD("dump: %s", line);
            // Parse clientPlayerList
            if (strstr(line, "clientPlayerList=")) {
                uintptr_t addr = 0;
                sscanf(line, "clientPlayerList=%lx", &addr);
                LOGD("Found clientPlayerList from helper file: %lx", addr);
            }
        }
        fclose(f);
    }
}

Vector3 OxideESP::getPlayerPos(uintptr_t playerAddr) {
    if (!playerAddr) return Vector3();
    UnityVec3 pos;
    mem.readv(playerAddr + OxideOffsets::PlayerManager::lastTickPosition, &pos, sizeof(pos));
    return Vector3(pos.x, pos.y, pos.z);
}

float OxideESP::getPlayerHealth(uintptr_t playerAddr) {
    if (!playerAddr) return 100.0f;
    uintptr_t vitals = mem.getPtr(playerAddr + OxideOffsets::PlayerManager::vitals);
    if (!vitals) return 100.0f;
    float maxH = mem.getFloat(vitals + OxideOffsets::GenericVitals::m_MaxHealth);
    if (maxH > 0 && maxH < 1000) return maxH;
    return 100.0f;
}

std::string OxideESP::getPlayerName(uintptr_t playerAddr) {
    return "Player";
}

bool OxideESP::getViewMatrix(float* outMatrix) {
    // BlueStacks: try to get view matrix from helper or from Camera
    // For now, try to read from /data/local/tmp/oxide_matrix.bin if helper wrote it
    FILE* f = fopen("/data/local/tmp/oxide_matrix.bin", "rb");
    if (f) {
        fread(outMatrix, sizeof(float), 16, f);
        fclose(f);
        return true;
    }
    // Fallback: try to find Camera.main view matrix via scanning
    // TODO: implement proper matrix reading for BlueStacks OpenGL
    return false;
}

Vector3 OxideESP::getCameraPos() {
    if (localPlayer) {
        return getPlayerPos(localPlayer);
    }
    return Vector3(0,0,0);
}

std::vector<uintptr_t> OxideESP::getPlayerList(bool activeOnly) {
    std::vector<uintptr_t> result;
    if (!il2cppBase) return result;

    // Method 1: No Frida - read from helper dump file (injected SO)
    // This is the primary method for BlueStacks without Frida
    FILE* f = fopen("/data/local/tmp/oxide_addrs.bin", "rb");
    if (f) {
        uintptr_t clientList = 0, activeList = 0, sleepingList = 0;
        fread(&clientList, sizeof(uintptr_t), 1, f);
        fread(&activeList, sizeof(uintptr_t), 1, f);
        fread(&sleepingList, sizeof(uintptr_t), 1, f);
        fclose(f);

        uintptr_t listToUse = activeOnly ? activeList : clientList;
        if (listToUse == 0) listToUse = clientList;
        if (listToUse == 0) listToUse = activeList;

        if (listToUse) {
            Il2CppParser parser(&mem);
            auto players = parser.parsePlayerList(listToUse);
            LOGD("Got %zu players from helper file list %lx", players.size(), listToUse);
            if (!players.empty()) return players;
        }
    }

    // Method 2: Pure external parser - no helper, no Frida
    // Uses Il2CppParser to brute force find PlayerManager class
    {
        Il2CppParser parser(&mem);
        auto players = parser.getPlayersNoFrida();
        if (!players.empty()) {
            LOGD("Got %zu players from pure external parser", players.size());
            return players;
        }
    }

    // Method 3: Scan heap for PlayerManager-like objects (fallback for BlueStacks)
    // This works without any il2cpp parsing, just heuristic
    // Scan /proc/pid/maps for rw regions and look for valid PlayerManager objects
    // For BlueStacks with root, we can read /proc/pid/mem
    // Implementation: iterate maps, read each rw region, check if at offset 0x1C8 there's a valid Vector3
    // and at 0xC8 there's a valid pointer

    // For now, return empty if all methods fail - will be filled when helper is injected
    LOGD("No players found, need helper injection");
    return result;
}

void OxideESP::Update() {
    if (!initialized) return;
    players.clear();

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
        if (p.distance < 1.0f) {
            localPlayer = addr;
            p.isLocal = true;
            if (!g_Switch.showTeam) continue;
        }
        players.push_back(p);
    }

    // Update camera pos
    camera.position = getCameraPos();
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
    if (!localPlayer) return;
    uintptr_t mouseLook = mem.getPtr(localPlayer + OxideOffsets::PlayerManager::mouseLook);
    if (!mouseLook) return;

    Vector3 localPos = getCameraPos();
    Vector3 enemyPos = target->position;
    enemyPos.Y += g_Config.aimOffsetY;

    Vector3 dir = enemyPos - localPos;
    float dist = dir.Length();
    if (dist < 0.1f) return;

    float yaw = atan2(dir.Z, dir.X) * 180.0f / 3.14159265f;
    mem.writeFloat(localPlayer + OxideOffsets::PlayerManager::lookAngle, yaw);
}

void OxideESP::applyNoRecoil(uintptr_t weaponConfig) {
    if (!weaponConfig) return;
    uintptr_t recoilConfig = mem.getPtr(weaponConfig + OxideOffsets::HitscanWeaponConfig::recoilConfig);
    if (!recoilConfig) return;
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
    float viewMatrix[16] = {0};
    bool hasMatrix = getViewMatrix(viewMatrix);

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
            float dx = p.position.X - camera.position.X;
            float dy = p.position.Y - camera.position.Y;
            float dz = p.position.Z - camera.position.Z;
            float dist = sqrt(dx*dx + dy*dy + dz*dz);
            if (dist < 0.1f) continue;
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

    if (g_Switch.aimCircle) {
        ImDrawList* drawList = ImGui::GetForegroundDrawList();
        drawList->AddCircle(ImVec2(centerX, centerY), g_Config.aimFov, IM_COL32(255,0,0,255), 0, 2.0f);
    }

    if (g_Switch.radar) {
        float radarX = g_Config.radarX;
        float radarY = g_Config.radarY;
        float radarSize = 200;
        ImDrawList* bg = ImGui::GetBackgroundDrawList();
        bg->AddRectFilled(ImVec2(radarX, radarY), ImVec2(radarX+radarSize, radarY+radarSize), IM_COL32(0,0,0,150));
        bg->AddRect(ImVec2(radarX, radarY), ImVec2(radarX+radarSize, radarY+radarSize), IM_COL32(0,255,0,255));
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

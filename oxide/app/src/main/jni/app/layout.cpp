#include "config.h"
#include "oxide_esp.h"
#include <cmath>

void DrawOxideMenu() {
    ImGuiStyle* style = &ImGui::GetStyle();
    ImGui::SetNextWindowBgAlpha(0.85f);
    style->WindowRounding = 8.0f;
    style->FrameRounding = 4.5f;
    style->WindowBorderSize = 0.0f;
    style->FramePadding = ImVec2(7,7);

    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImVec4* colors = style->Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.95f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    static bool show_esp = true;
    static bool show_aim = false;
    static bool show_weapon = false;
    static bool show_world = false;

    ImGui::Begin("Oxide: Survival Island - Panda Emu OpenGL");
    ImGui::SetWindowSize({800, 600}, ImGuiCond_Once);
    Window = ImGui::GetCurrentWindow();

    if (ImGui::Button("ESP", ImVec2(180, 60))) {
        show_esp = true; show_aim = false; show_weapon = false; show_world = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("AIM", ImVec2(180, 60))) {
        show_esp = false; show_aim = true; show_weapon = false; show_world = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("WEAPON", ImVec2(180, 60))) {
        show_esp = false; show_aim = false; show_weapon = true; show_world = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("WORLD", ImVec2(180, 60))) {
        show_esp = false; show_aim = false; show_weapon = false; show_world = true;
    }

    ImGui::Separator();

    if (show_esp) {
        ImGui::Text("ESP Settings - Oxide");
        ImGui::Checkbox("ESP Enable", &g_Switch.esp);
        ImGui::Checkbox("Box", &g_Switch.box);
        ImGui::Checkbox("Line", &g_Switch.line);
        ImGui::Checkbox("Distance", &g_Switch.distance);
        ImGui::Checkbox("Name", &g_Switch.name);
        ImGui::Checkbox("Health Bar", &g_Switch.health);
        ImGui::Checkbox("Radar", &g_Switch.radar);
        ImGui::Checkbox("Show Sleeping", &g_Switch.sleeping);

        ImGui::SliderFloat("ESP Max Distance", &g_Config.espMaxDistance, 50.0f, 1000.0f, "%.0f m");
        ImGui::SliderFloat("Box Thickness", &g_Config.boxThickness, 1.0f, 5.0f, "%.1f");
        ImGui::SliderFloat("Radar X", &g_Config.radarX, 0, 1000, "%.0f");
        ImGui::SliderFloat("Radar Y", &g_Config.radarY, 0, 1000, "%.0f");
    }

    if (show_aim) {
        ImGui::Text("Aimbot - Oxide");
        ImGui::Checkbox("Aimbot", &g_Switch.aimbot);
        ImGui::Checkbox("Aim Circle", &g_Switch.aimCircle);
        ImGui::Checkbox("Closest to Crosshair", &g_Switch.aimClosest);
        ImGui::Checkbox("Only on Scope", &g_Switch.aimOnScope);

        ImGui::SliderFloat("Aim FOV", &g_Config.aimFov, 50.0f, 600.0f, "%.0f");
        ImGui::SliderFloat("Aim Distance", &g_Config.aimDistance, 10.0f, 500.0f, "%.0f m");
        ImGui::SliderFloat("Aim Offset Y", &g_Config.aimOffsetY, -1.0f, 2.0f, "%.2f");
        ImGui::SliderFloat("Aim Smooth", &g_Config.aimSmooth, 1.0f, 20.0f, "%.1f");

        if (g_Switch.aimbot) {
            float dist, cross;
            auto* target = g_OxideESP.getClosestTarget(dist, cross);
            if (target) {
                ImGui::Text("Target: %.0f m | cross %.0f", dist, cross);
                if (ImGui::Button("Aim Now")) {
                    g_OxideESP.doAimbot(target);
                }
            } else {
                ImGui::Text("No target in FOV");
            }
        }
    }

    if (show_weapon) {
        ImGui::Text("Weapon Mods - Oxide (No Recoil / No Spread)");
        ImGui::Checkbox("No Recoil", &g_Switch.noRecoil);
        ImGui::Checkbox("No Spread", &g_Switch.noSpread);
        ImGui::Checkbox("Fast Reload", &g_Switch.fastReload);
        ImGui::Checkbox("Infinite Ammo (visual)", &g_Switch.infiniteAmmo);

        ImGui::Text("From dump: HitscanWeaponConfig offsets:");
        ImGui::BulletText("damage 0x18, BulletSpeed 0x30, fireRate 0x34");
        ImGui::BulletText("recoilConfig 0x68 -> MinRecoil 0x18");
        ImGui::BulletText("dispersionConfig 0x60 -> minDispersion 0x20");

        if (ImGui::Button("Apply Weapon Mods")) {
            // In real cheat, you would get current weapon config from PlayerManager->equipment
            // For emulator testing, this is placeholder
        }
    }

    if (show_world) {
        ImGui::Text("World ESP - Oxide");
        ImGui::Checkbox("Vehicles", &g_Switch.espVehicles);
        ImGui::Checkbox("Containers", &g_Switch.espContainers);
        ImGui::Checkbox("Airdrop", &g_Switch.espAirdrop);
        ImGui::Checkbox("High Jump", &g_Switch.highJump);
        ImGui::Checkbox("No Fall", &g_Switch.noFall);
        ImGui::Checkbox("Speed Hack", &g_Switch.speedHack);

        ImGui::Separator();
        ImGui::Text("Debug Info:");
        ImGui::Text("il2cpp base: 0x%lx", g_OxideESP.il2cppBase);
        ImGui::Text("unity base: 0x%lx", g_OxideESP.unityBase);
        ImGui::Text("players found: %zu", g_OxideESP.players.size());
        ImGui::Text("screen: %d x %d", _ScreenX, _ScreenY);
        ImGui::Text("pid: %d", g_OxideESP.mem.pid);

        if (ImGui::Button("Rescan PID com.oxide.survival")) {
            int pid = g_OxideESP.mem.getPID("com.oxide.survival");
            if (pid > 0) {
                g_OxideESP.Init(pid);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Rescan PID com.oxide.survivalisland")) {
            int pid = g_OxideESP.mem.getPID("com.oxide.survivalisland");
            if (pid > 0) {
                g_OxideESP.Init(pid);
            }
        }
    }

    ImGui::Separator();
    ImGui::Text("Oxide Panda - Emulator OpenGL Build");
    ImGui::Text("Dump: PlayerManager clientPlayerList static 0x10");
    ImGui::Text("Build: %s", __DATE__);

    ImGui::End();
}

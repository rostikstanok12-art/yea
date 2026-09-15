#pragma once
#include "imgui.h"
#ifndef IM_DOWN
#define IM_DOWN 0
#endif
#ifndef IM_MOVE
#define IM_MOVE 2
#endif
#ifndef IM_UP
#define IM_UP 1
#endif
struct ImGuInput_LinuxEvent {
    int Action_type;
    int fingerIndex;
    ImVec2 pos;
};

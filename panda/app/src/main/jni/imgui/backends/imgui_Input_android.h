#pragma once

#ifndef IM_DOWN
    #define IM_DOWN 0 //按下
#endif
#ifndef IM_MOVE
    #define IM_MOVE 2 //移动
#endif 
#ifndef IM_UP
    #define IM_UP 1    //放开
#endif

typedef struct {
    int Action;
    float x;
    float y;
    int VSCOLL;
    int HSCROLL;
    int ToolType;
    int ButtonState;
    float AXIS_VSCROLL;
    float AXIS_HSCROLL;
} MyInputEvent;

struct ImGuInput_LinuxEvent {
    int Action_type;
    int fingerIndex;
    ImVec2 pos;
};

IMGUI_IMPL_API int32_t  ImGui_ImplAndroid_HandleInputEvent(MyInputEvent input_event);
IMGUI_IMPL_API int32_t  ImGui_ImplLinux_HandleInputEvent(ImGuInput_LinuxEvent event);

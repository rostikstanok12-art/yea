#include "imgui.h"
#include "imgui_Input_android.h"
#include <time.h>
#include <android/native_window.h>
#include <android/input.h>
#include <android/keycodes.h>


int32_t ImGui_ImplAndroid_HandleInputEvent(MyInputEvent input_event)
{
        ImGuiIO& io = ImGui::GetIO();
        int32_t event_action = input_event.Action;
        event_action &= AMOTION_EVENT_ACTION_MASK;
        //LOGI("%d %d",AMOTION_EVENT_AXIS_VSCROLL,AMOTION_EVENT_AXIS_HSCROLL);
        switch (event_action)
        {
        case AMOTION_EVENT_ACTION_DOWN:
        case AMOTION_EVENT_ACTION_UP:
            // Physical mouse buttons (and probably other physical devices) also invoke the actions AMOTION_EVENT_ACTION_DOWN/_UP,
            // but we have to process them separately to identify the actual button pressed. This is done below via
            // AMOTION_EVENT_ACTION_BUTTON_PRESS/_RELEASE. Here, we only process "FINGER" input (and "UNKNOWN", as a fallback).
            if((input_event.ToolType == AMOTION_EVENT_TOOL_TYPE_FINGER)
            || (input_event.ToolType == AMOTION_EVENT_TOOL_TYPE_UNKNOWN))
            {
                io.MouseDown[0] = (event_action == AMOTION_EVENT_ACTION_DOWN) ? true : false;
                io.MousePos = ImVec2(input_event.x, input_event.y);
            }
            break;
        case AMOTION_EVENT_ACTION_BUTTON_PRESS:
        case AMOTION_EVENT_ACTION_BUTTON_RELEASE:
            {
                int32_t button_state = input_event.ButtonState;
                io.MouseDown[0] = (button_state & AMOTION_EVENT_BUTTON_PRIMARY) ? true : false;
                io.MouseDown[1] = (button_state & AMOTION_EVENT_BUTTON_SECONDARY) ? true : false;
                io.MouseDown[2] = (button_state & AMOTION_EVENT_BUTTON_TERTIARY) ? true : false;
            }
            break;
        case AMOTION_EVENT_ACTION_HOVER_MOVE: // Hovering: Tool moves while NOT pressed (such as a physical mouse)
        case AMOTION_EVENT_ACTION_MOVE:       // Touch pointer moves while DOWN
            io.MousePos = ImVec2(input_event.x, input_event.y);
            break;
        case AMOTION_EVENT_ACTION_SCROLL:
            io.MouseWheel = input_event.AXIS_VSCROLL; //AMotionEvent_getAxisValue(input_event, AMOTION_EVENT_AXIS_VSCROLL, event_pointer_index);
            io.MouseWheelH = input_event.AXIS_HSCROLL;// AMotionEvent_getAxisValue(input_event, AMOTION_EVENT_AXIS_HSCROLL, event_pointer_index);
            break;
        default:
            break;
        }
        return 1;

   // return 0;
}

int32_t ImGui_ImplLinux_HandleInputEvent(ImGuInput_LinuxEvent event) {
    ImGuiIO &io = ImGui::GetIO();
    if (event.Action_type == IM_DOWN || event.Action_type == IM_UP) {
        io.MouseDown[0] = (event.Action_type == IM_DOWN);
        io.MousePos = event.pos;
    } else {
        io.MousePos = event.pos;
    }
    return 0;
}

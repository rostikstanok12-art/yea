#ifndef IMGUI_CUSTOMIZE_H
#define IMGUI_CUSTOMIZE_H

#include "imgui.h"
#include "imgui_internal.h"


namespace ImGui {
    //自定义::(控件)小组件
	IMGUI_API bool          M_SwitchCircle(const char* str_id, bool* v, const ImVec2& size = ImVec2(110.0f, 45.0f));
    IMGUI_API bool          ButtonTextColored(const ImVec4& col, const char* fmt, ...);
    IMGUI_API bool          M_shut(const char* text);
    IMGUI_API void          M_shut(const char* text, bool* close);
    IMGUI_API void          M_shut_G(const char* text, bool* close, const ImVec4& col);


}

#endif //IMGUI_CUSTOMIZE_H

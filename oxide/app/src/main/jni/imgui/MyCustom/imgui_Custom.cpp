#include "imgui_Custom.h"


bool ImGui::M_SwitchCircle(const char* str_id, bool* v, const ImVec2& size) {
    if (size.y > size.x)
        return false;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(str_id);
    ImVec2 pos = window->DC.CursorPos;
	
    const ImVec2 label_size = CalcTextSize(str_id, NULL, true);
    const ImRect total_bb(pos, ImVec2(pos.x + size.x + label_size.x + style.ItemInnerSpacing.x, pos.y + size.y));
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id))
        return false;


    bool hovered, held;
    bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
    if (pressed) {
        *v = !(*v);
        MarkItemEdited(id);
    }
    
	window->DrawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), ImColor(255, 255, 255), size.y*0.5f);
	int crevice = 4;
	if (*v) {
		window->DrawList->AddRectFilled(ImVec2(pos.x + (size.x-size.y) + crevice, pos.y + crevice), ImVec2(pos.x + size.x - crevice, pos.y + size.y-crevice), ImColor(159, 191, 255), size.y*0.5f);    
    } else {
		window->DrawList->AddRectFilled(ImVec2(pos.x + crevice, pos.y + crevice), ImVec2(pos.x + size.y - crevice, pos.y + size.y - crevice), ImGui::GetColorU32(ImVec4(0.78f, 0.78f, 0.78f, 1.0f)), size.y*0.5f);
    }
    //const ImRect total_bb_2(ImVec2(total_bb.Max.x+1, pos.y), ImVec2(total_bb.Max.x + label_size.x, total_bb.Max.y));
    //ItemSize(total_bb_2, style.FramePadding.y);
	if (size.y >= label_size.y) {
    	float fg = (size.y - label_size.y) / 2;
    	RenderText(ImVec2(pos.x + size.x + style.ItemInnerSpacing.x, pos.y + fg), str_id);
    }
    //held 是按住状态
	return pressed;
}


bool ImGui::ButtonTextColored(const ImVec4& col, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    TextColoredV(col, fmt, args);
    va_end(args);
    return IsItemClicked();
}

bool ImGui::M_shut(const char* text) {
    const float w = ImGui::GetWindowWidth();
    const ImVec2 text_size = CalcTextSize(text, NULL, true);
    ImGui::SetCursorPosX(w - (text_size.x + ImGui::GetFrameHeight()/4));
    if (ImGui::SmallButton(text))
        return true;
    else 
        return false;
}

void ImGui::M_shut(const char* text, bool* close) {
    const float w = ImGui::GetWindowWidth();
    const ImVec2 text_size = CalcTextSize(text, NULL, true);
    ImGui::SetCursorPosX(w - (text_size.x + ImGui::GetFrameHeight()/4));
    if (ImGui::SmallButton(text))
       *close = true;
}
void ImGui::M_shut_G(const char* text, bool* close, const ImVec4& col) {
    const float w = ImGui::GetWindowWidth();
    const ImVec2 text_size = CalcTextSize(text, NULL, true);
    ImGui::SetCursorPosX(w - (text_size.x + ImGui::GetFrameHeight()/4));
    if (ImGui::ButtonTextColored(col, text)) {
       *close = !*close;
    }
}

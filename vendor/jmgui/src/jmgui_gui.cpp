#include "jmgui.h"
#include "imgui.h"
#include <stdlib.h>

bool jmgui_begin(const char *name, bool *p_open)
{
	return ImGui::Begin(name, p_open);
}

void jmgui_end()
{
	ImGui::End();
}

void jmgui_push_id_u64(uint64_t id)
{
	ImGui::PushID((void *)id);
}

void jmgui_pop_id()
{
	ImGui::PopID();
}

void jmgui_checkbox(const char *name, bool *v)
{
	ImGui::Checkbox(name, v);
}

void jmgui_slider_float(const char *name, float *v, float v_min, float v_max)
{
	ImGui::SliderFloat(name, v, v_min, v_max);
}

bool jmgui_button(const char *label)
{
	return ImGui::Button(label);
}

bool jmgui_button_small(const char *label)
{
	bool r = ImGui::SmallButton(label);
	return r;
}

bool jmgui_collapsing_header(const char *name)
{
	return ImGui::CollapsingHeader(name);
}

bool jmgui_input_text(const char *label, char *buf, size_t buf_size)
{
	return ImGui::InputText(label, buf, buf_size, 0, NULL, NULL);
}

void jmgui_debug_locate(unsigned int id)
{
	// ImGui::DebugTextUnformattedWithLocateItem(line_begin, line_end);
	ImGui::DebugLocateItemOnHover(id);
}

unsigned int jmgui_get_id_by_string(const char *str_id)
{
	return ImGui::GetID(str_id);
}

unsigned int jmgui_get_last_id()
{
	ImGuiContext *g = ImGui::GetCurrentContext();
	return g->LastItemData.ID;
}

void jmgui_dummy(float x, float y)
{
	ImGui::Dummy(ImVec2(x, y));
}

bool jmgui_last_hover()
{
	ImGuiContext *g = ImGui::GetCurrentContext();
	ImRect r = g->LastItemData.Rect;
	if (ImGui::IsMouseHoveringRect(r.Min, r.Max, true)) {
		return true;
	}
	return false;
	/*
	TextUnformatted(line_begin, line_end);
	if (!IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
	return;
	ImGuiContext& g = *GImGui;
	ImRect text_rect = g.LastItemData.Rect;
	for (const char* p = line_begin; p <= line_end - 10; p++)
	{
	ImGuiID id = 0;
	if (p[0] != '0' || (p[1] != 'x' && p[1] != 'X') || sscanf(p + 2, "%X", &id) != 1 || ImCharIsXdigitA(p[10]))
	continue;
	ImVec2 p0 = CalcTextSize(line_begin, p);
	ImVec2 p1 = CalcTextSize(p, p + 10);
	g.LastItemData.Rect = ImRect(text_rect.Min + ImVec2(p0.x, 0.0f), text_rect.Min + ImVec2(p0.x + p1.x, p1.y));
	if (IsMouseHoveringRect(g.LastItemData.Rect.Min, g.LastItemData.Rect.Max, true))
	DebugLocateItemOnHover(id);
	p += 10;
	*/
}

void jmgui_separator()
{
	ImGui::Separator();
}

void jmgui_set_next_item_storage_id(unsigned int id)
{
	ImGui::SetNextItemStorageID((ImGuiID)id);
}



void jmgui_sameline()
{
	ImGui::SameLine();
}
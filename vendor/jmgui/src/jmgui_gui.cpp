#include "jmgui.h"
#include "imgui.h"

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

void jmgui_text(const char *text)
{
	ImGui::Text("%s", text);
}

void jmgui_text_colored_va(float r, float g, float b, const char *fmt, va_list args)
{
	const ImVec4 col = ImVec4(r, g, b, 1.0f);
	ImGui::TextColoredV(col, fmt, args);
}

void jmgui_text_colored(float r, float g, float b, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	jmgui_text_colored_va(r, g, b, fmt, args);
	va_end(args);
}

bool jmgui_collapsing_header(const char *name)
{
	return ImGui::CollapsingHeader(name);
}

bool jmgui_tree(const char *name)
{
	return ImGui::TreeNode(name);
}

void jmgui_tree_pop()
{
	ImGui::TreePop();
}

void jmgui_sameline()
{
	ImGui::SameLine();
}

bool jmgui_tree_node(const char *name, int flags, float r, float g, float b)
{
	// const ImVec4 col = ImVec4(r, g, b, 1.0f);
	// ImGui::PushStyleColor(ImGuiCol_Text, col);
	bool result = ImGui::TreeNodeEx(name, flags);
	// ImGui::PopStyleColor(1);
	return result;
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

bool jmgui_text_link(const char *label)
{
	return ImGui::TextLink(label);
}

void jmgui_separator()
{
	ImGui::Separator();
}

void jmgui_color_edit3(float c[3])
{
	static ImGuiColorEditFlags base_flags = ImGuiColorEditFlags_None;
	ImGui::ColorEdit3("MyColor##1", c, base_flags);
}

void jmgui_get_current_tree_node(unsigned int *id, int *treeflags, int *itemflags)
{
	ImGuiContext &g = *GImGui;
	if (g.TreeNodeStack.size() == 0) {
		return;
	}
	const ImGuiTreeNodeStackData *data = &g.TreeNodeStack.back();
	if (id) {
		(*id) = data->ID;
	}
	if (treeflags) {
		(*treeflags) = data->TreeFlags;
	}
	if (itemflags) {
		(*itemflags) = data->ItemFlags;
	}
}

unsigned int jmgui_tree_node_get_open(unsigned int storage_id)
{
	return ImGui::TreeNodeGetOpen(storage_id);
}

void jmgui_set_next_item_storage_id(unsigned int id)
{
	ImGui::SetNextItemStorageID((ImGuiID)id);
}

bool jmgui_input_i32(const char *label, int32_t *v, int step, int step_fast, int flags)
{
	return ImGui::InputInt(label, v, step, step_fast, flags);
}

bool jmgui_input_scalar_n(const char *label, int data_type, void *p_data, int components, const void *p_step, const void *p_step_fast, const char *format, int flags)
{
	return ImGui::InputScalarN(label, data_type, p_data, components, p_step, p_step_fast, format, flags);
}

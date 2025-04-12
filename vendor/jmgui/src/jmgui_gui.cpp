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

void jmgui_text_colored_va(float r, float g, float b, const char* fmt, va_list args)
{
	const ImVec4 col = ImVec4(r, g, b, 1.0f);
	ImGui::TextColoredV(col, fmt, args);
}

void jmgui_text_colored(float r, float g, float b, const char* fmt, ...)
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
	const ImVec4 col = ImVec4(r, g, b, 1.0f);
	ImGui::PushStyleColor(ImGuiCol_Text, col);
	bool result = ImGui::TreeNodeEx(name, flags);
	ImGui::PopStyleColor(1);
	return result;
}

/*
        if (ImGui::BeginTable("3ways", 3, table_flags))
        {
            // The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
            ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);
            ImGui::TableHeadersRow();
*/

bool jmgui_table_begin(const char *name, int columns, int flags)
{
	ImGuiTableFlags table_flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;
	return ImGui::BeginTable(name, columns, table_flags);
}

void jmgui_table_end()
{
	ImGui::EndTable();
}

void jmgui_table_setup_column(const char *name, int flags, float width)
{
	float w = ImGui::CalcTextSize("A").x;
	ImGui::TableSetupColumn(name, flags, w * width);
}

void jmgui_table_header_row()
{
	ImGui::TableHeadersRow();
}

void jmgui_table_next_row(int flags)
{
	ImGui::TableNextRow(flags);
}
void jmgui_table_next_column()
{
	ImGui::TableNextColumn();
}

bool jmgui_tab_begin(const char *name, int flags)
{
	return ImGui::BeginTabBar(name, flags);
}

void jmgui_tab_end()
{
	ImGui::EndTabBar();
}

bool jmgui_tab_item_begin(const char *name, int flags)
{
	return ImGui::BeginTabItem(name, nullptr, flags);
}
void jmgui_tab_item_end()
{
	ImGui::EndTabItem();
}


bool jmgui_input_text(const char *label, char *buf, size_t buf_size)
{
	return ImGui::InputText(label, buf, buf_size, 0, NULL, NULL);
}

void jmgui_debug_locate(unsigned int id)
{
	//ImGui::DebugTextUnformattedWithLocateItem(line_begin, line_end);
	ImGui::DebugLocateItemOnHover(id);
}

unsigned int jmgui_get_id_by_string(const char* str_id)
{
	return ImGui::GetID(str_id);
}

unsigned int jmgui_get_last_id()
{
	ImGuiContext* g = ImGui::GetCurrentContext();
	return g->LastItemData.ID;
}


void jmgui_dummy(float x, float y)
{
	ImGui::Dummy(ImVec2(x, y));
}

void jmgui_same_line(float offset_from_start_x, float spacing)
{
	ImGui::SameLine();
}

bool jmgui_last_hover()
{
	ImGuiContext* g = ImGui::GetCurrentContext();
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

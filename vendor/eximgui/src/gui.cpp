#include "eximgui.h"
#include "imgui.h"

bool gui_begin(const char *name, bool *p_open)
{
	return ImGui::Begin(name, p_open);
}

void gui_end()
{
	ImGui::End();
}

void gui_push_id_u64(uint64_t id)
{
	ImGui::PushID((void *)id);
}

void gui_pop_id()
{
	ImGui::PopID();
}

void gui_checkbox(const char *name, bool *v)
{
	ImGui::Checkbox(name, v);
}

void gui_slider_float(const char *name, float *v, float v_min, float v_max)
{
	ImGui::SliderFloat(name, v, v_min, v_max);
}

bool gui_button(const char *label)
{
	return ImGui::Button(label);
}

bool gui_button_small(const char *label)
{
	bool r = ImGui::SmallButton(label);
	return r;
}

void gui_text(const char *text)
{
	ImGui::Text("%s", text);
}

bool gui_collapsing_header(const char *name)
{
	return ImGui::CollapsingHeader(name);
}

bool gui_tree(const char *name)
{
	return ImGui::TreeNode(name);
}

void gui_tree_pop()
{
	ImGui::TreePop();
}

void gui_sameline()
{
	ImGui::SameLine();
}

bool gui_tree_node(const char *name, int flags)
{
	return ImGui::TreeNodeEx(name, flags);
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

bool gui_table_begin(const char *name, int columns, int flags)
{
	ImGuiTableFlags table_flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;
	return ImGui::BeginTable(name, columns, table_flags);
}

void gui_table_end()
{
	ImGui::EndTable();
}

void gui_table_setup_column(const char *name, int flags, float width)
{
	float w = ImGui::CalcTextSize("A").x;
	ImGui::TableSetupColumn(name, flags, w * width);
}

void gui_table_header_row()
{
	ImGui::TableHeadersRow();
}

void gui_table_next_row(int flags)
{
	ImGui::TableNextRow(flags);
}
void gui_table_next_column()
{
	ImGui::TableNextColumn();
}

bool gui_tab_begin(const char *name, int flags)
{
	return ImGui::BeginTabBar(name, flags);
}

void gui_tab_end()
{
	ImGui::EndTabBar();
}

bool gui_tab_item_begin(const char *name, int flags)
{
	return ImGui::BeginTabItem(name, nullptr, flags);
}
void gui_tab_item_end()
{
	ImGui::EndTabItem();
}


bool gui_input_text(const char *label, char *buf, size_t buf_size)
{
	return ImGui::InputText(label, buf, buf_size, 0, NULL, NULL);
}

void gui_debug_locate(const char* line_begin, const char* line_end)
{
	ImGui::DebugTextUnformattedWithLocateItem(line_begin, line_end);
}

unsigned int gui_get_id_by_string(const char* str_id)
{
	return ImGui::GetID(str_id);
}

void gui_dummy(float x, float y)
{
	ImGui::Dummy(ImVec2(x, y));
}
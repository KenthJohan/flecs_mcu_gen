#include "jmgui/jmgui_table.h"
#include "imgui.h"

bool jmgui_table_begin(const char *name, int columns, int flags)
{
	ImGuiTableFlags table_flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody;
	// table_flags |= ImGuiTableFlags_RowBg;
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

void jmgui_table_merge_begin()
{
	ImGuiTable *table = ImGui::GetCurrentTable();
	float min_x = table->Columns[0].MinX;
	float max_x = table->Columns[table->ColumnsCount - 1].MaxX;
	float y = ImGui::GetCursorScreenPos().y;
	float height = ImGui::GetTextLineHeightWithSpacing();
	ImVec2 min(min_x, y);
	ImVec2 max(max_x, y + height);
	// Draw background for merged row
	// ImGui::GetWindowDrawList()->AddRectFilled(min, max, IM_COL32(100, 100, 255, 50));
	// Temporarily disable clipping for this merged cell
	ImGui::PushClipRect(min, max, false);
	// ImGui::SetCursorScreenPos(min);
	// ImGui::TextUnformatted("Merged header Merged headerMerged headerMerged headerMerged headerMerged headerMerged headerMerged headerMerged headerMerged headerMerged headerMerged header");
}

void jmgui_table_merge_end()
{
	ImGui::PopClipRect();
}

void jmgui_table_set_row_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(r, g, b, a));
}

void jmgui_table_set_column_index(int i)
{
	ImGui::TableSetColumnIndex(i);
}
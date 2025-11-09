#include "jmgui/jmgui_tabs.h"
#include "imgui.h"

bool jmgui_tabs_begin(const char *name, int flags)
{
	return ImGui::BeginTabBar(name, flags);
}

void jmgui_tabs_end()
{
	ImGui::EndTabBar();
}

bool jmgui_tabs_item_begin(const char *name, int flags)
{
	return ImGui::BeginTabItem(name, nullptr, flags);
}
void jmgui_tabs_item_end()
{
	ImGui::EndTabItem();
}



void jmgui_get_cursor_pos(float *x, float *y)
{
	ImVec2 a = ImGui::GetCursorPos();
	*x = a.x;
	*y = a.y;
}

void jmgui_set_cursor_pos(float x, float y)
{
	ImVec2 a = {x, y};
	ImGui::SetCursorPos(a);
}

bool jmgui_is_item_active()
{
	return ImGui::IsItemActive();
}


void jmgui_get_mouse_delta(float *x, float *y)
{
	ImVec2 a = ImGui::GetIO().MouseDelta;
	*x = a.x;
	*y = a.y;
}
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

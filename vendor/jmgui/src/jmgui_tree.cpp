
#include "jmgui/jmgui_tree.h"
#include "imgui.h"

bool jmgui_tree(const char *name)
{
	return ImGui::TreeNode(name);
}

void jmgui_tree_pop()
{
	ImGui::TreePop();
}

bool jmgui_tree_node(const char *name, int flags, float r, float g, float b)
{
	// const ImVec4 col = ImVec4(r, g, b, 1.0f);
	// ImGui::PushStyleColor(ImGuiCol_Text, col);
	bool result = ImGui::TreeNodeEx(name, flags);
	// ImGui::PopStyleColor(1);
	return result;
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
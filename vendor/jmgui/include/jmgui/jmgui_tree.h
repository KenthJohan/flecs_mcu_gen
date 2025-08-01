#pragma once

#ifdef __cplusplus
extern "C" {
#endif

bool jmgui_tree_node(const char *name, int flags, float r, float g, float b);
bool jmgui_tree(const char *name);
void jmgui_tree_pop();


unsigned int jmgui_tree_node_get_open(unsigned int storage_id);
void jmgui_get_current_tree_node(unsigned int *id, int *treeflags, int *itemflags);

#ifdef __cplusplus
}
#endif
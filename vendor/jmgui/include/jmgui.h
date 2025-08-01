#pragma once

#include "jmgui/jmgui_table.h"
#include "jmgui/jmgui_tabs.h"
#include "jmgui/jmgui_frame.h"
#include "jmgui/jmgui_context.h"

#ifdef __cplusplus
extern "C" {
#endif

bool jmgui_begin(const char *name, bool *p_open);
void jmgui_end();
void jmgui_text(const char *text);
void jmgui_textf(const char *fmt, ...);
bool jmgui_collapsing_header(const char *name);
bool jmgui_tree_node(const char *name, int flags, float r, float g, float b);
bool jmgui_tree(const char *name);
void jmgui_tree_pop();
void jmgui_push_id_u64(uint64_t id);
void jmgui_pop_id();
bool jmgui_button(const char *label);
bool jmgui_button_small(const char *label);
void jmgui_sameline();
void jmgui_debug_locate(unsigned int id);
unsigned int jmgui_get_id_by_string(const char *str_id);
unsigned int jmgui_get_last_id();
void jmgui_dummy(float x, float y);
bool jmgui_last_hover();
void jmgui_text_colored(float r, float g, float b, const char *fmt, ...);
bool jmgui_text_link(const char *label);
void jmgui_separator();
void jmgui_color_edit3(float c[3]);
unsigned int jmgui_tree_node_get_open(unsigned int storage_id);
void jmgui_get_current_tree_node(unsigned int *id, int *treeflags, int *itemflags);
bool jmgui_input_text(const char *label, char *buf, size_t buf_size);
bool jmgui_input_i32(const char *label, int32_t *v, int step, int step_fast, int flags);
void jmgui_slider_float(const char *name, float *v, float v_min, float v_max);
bool jmgui_input_scalar_n(const char *label, int data_type, void *p_data, int components, const void *p_step, const void *p_step_fast, const char *format, int flags);

// SetNextItemStorageID
void jmgui_set_next_item_storage_id(unsigned int id);

void jmgui_color_picker4_str(char * str);
void jmgui_color_picker3_str(char * str);

#ifdef __cplusplus
}
#endif
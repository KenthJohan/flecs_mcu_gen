#pragma once

#include "jmgui/jmgui_table.h"
#include "jmgui/jmgui_tabs.h"
#include "jmgui/jmgui_frame.h"
#include "jmgui/jmgui_context.h"
#include "jmgui/jmgui_text.h"
#include "jmgui/jmgui_color.h"
#include "jmgui/jmgui_input.h"
#include "jmgui/jmgui_tree.h"
#include "jmgui/jmgui_input.h"

#ifdef __cplusplus
extern "C" {
#endif

bool jmgui_begin(const char *name, bool *p_open);
void jmgui_end();

bool jmgui_collapsing_header(const char *name);
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
void jmgui_separator();
void jmgui_slider_float(const char *name, float *v, float v_min, float v_max);

// SetNextItemStorageID
void jmgui_set_next_item_storage_id(unsigned int id);

#ifdef __cplusplus
}
#endif
#pragma once

#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif







typedef struct {
	bool done;
	bool show_demo_window;
	bool show_another_window;
	float clear_color[4];
	SDL_Window *window;
	SDL_GPUDevice *gpu_device;
} jmgui_context_t;


int jmgui_context_init(jmgui_context_t *eximgui);
int jmgui_context_fini(jmgui_context_t *eximgui);
//void eximgui_progress(jmgui_context_t *eximgui);
void jmgui_frame_begin(jmgui_context_t *eximgui);
void jmgui_frame_frame(jmgui_context_t *eximgui);




bool jmgui_begin(const char *name, bool *p_open);
void jmgui_end();
void jmgui_text(const char *text);
bool jmgui_collapsing_header(const char *name);
bool jmgui_tree_node(const char *name, int flags, float r, float g, float b);
bool jmgui_tree(const char *name);
void jmgui_tree_pop();
void jmgui_push_id_u64(uint64_t id);
void jmgui_pop_id();
bool jmgui_button(const char *label);
bool jmgui_button_small(const char *label);
void jmgui_sameline();
bool jmgui_table_begin(const char *name, int columns, int flags);
void jmgui_table_end();
void jmgui_table_setup_column(const char *name, int flags, float width);
void jmgui_table_header_row();
void jmgui_table_next_row(int flags);
void jmgui_table_next_column();
bool jmgui_tab_begin(const char *name, int flags);
void jmgui_tab_end();
bool jmgui_tab_item_begin(const char *name, int flags);
void jmgui_tab_item_end();
void jmgui_debug_locate(unsigned int id);
unsigned int jmgui_get_id_by_string(const char* str_id);
unsigned int jmgui_get_last_id();
void jmgui_dummy(float x, float y);
bool jmgui_last_hover();
void jmgui_same_line(float offset_from_start_x, float spacing);
void jmgui_text_colored(float r, float g, float b, const char* fmt, ...);
bool jmgui_text_link(const char* label);
void jmgui_separator();
void jmgui_color_edit3(float c[3]);

unsigned int jmgui_tree_node_get_open(unsigned int storage_id);
void jmgui_get_current_tree_node(unsigned int * id, int * treeflags, int * itemflags);



bool jmgui_input_text(const char *label, char *buf, size_t buf_size);
bool jmgui_input_i32(const char *label, int32_t *v, int step, int step_fast, int flags);
void jmgui_slider_float(const char *name, float *v, float v_min, float v_max);

bool jmgui_input_scalar_n(const char* label, int data_type, void* p_data, int components, const void* p_step, const void* p_step_fast, const char* format, int flags);



//SetNextItemStorageID
void jmgui_set_next_item_storage_id(unsigned int id);

#ifdef __cplusplus
}
#endif
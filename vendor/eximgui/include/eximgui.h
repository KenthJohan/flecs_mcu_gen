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
	void * query1;
	void * query2;
	void * query3;
} eximgui_t;


int eximgui_init(eximgui_t *eximgui);
int eximgui_fini(eximgui_t *eximgui);
//void eximgui_progress(eximgui_t *eximgui);
void eximgui_begin_frame(eximgui_t *eximgui);
void eximgui_end_frame(eximgui_t *eximgui);




bool gui_begin(const char *name, bool *p_open);
void gui_end();
void gui_text(const char *text);
bool gui_collapsing_header(const char *name);
bool gui_tree_node(const char *name, int flags);
bool gui_tree(const char *name);
void gui_tree_pop();
void gui_push_id_u64(uint64_t id);
void gui_pop_id();
bool gui_button(const char *label);
bool gui_button_small(const char *label);
void gui_sameline();
bool gui_table_begin(const char *name, int columns, int flags);
void gui_table_end();
void gui_table_setup_column(const char *name, int flags, float width);
void gui_table_header_row();
void gui_table_next_row(int flags);
void gui_table_next_column();
bool gui_tab_begin(const char *name, int flags);
void gui_tab_end();
bool gui_tab_item_begin(const char *name, int flags);
void gui_tab_item_end();

#ifdef __cplusplus
}
#endif
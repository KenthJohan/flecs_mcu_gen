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
} eximgui_t;


int eximgui_init(eximgui_t *eximgui);
int eximgui_fini(eximgui_t *eximgui);
//void eximgui_progress(eximgui_t *eximgui);
void eximgui_begin_frame(eximgui_t *eximgui);
void eximgui_end_frame(eximgui_t *eximgui);

void gui_begin(const char *name, bool *p_open);
void gui_end();
void gui_text(const char *text);
bool gui_collapsing_header(const char *name);
bool gui_tree(const char *name);
void gui_tree_pop();

#ifdef __cplusplus
}
#endif
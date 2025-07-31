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

int jmgui_context_init(jmgui_context_t *eximgui, char const * title, int w, int h);
int jmgui_context_fini(jmgui_context_t *eximgui);

#ifdef __cplusplus
}
#endif
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
} eximgui_t;


int eximgui_init(eximgui_t *eximgui);
int eximgui_fini(eximgui_t *eximgui);
void eximgui_progress(eximgui_t *eximgui);

int test2();


int test1();


#ifdef __cplusplus
}
#endif
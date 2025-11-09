#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool jmgui_tabs_begin(const char *name, int flags);
void jmgui_tabs_end();
bool jmgui_tabs_item_begin(const char *name, int flags);
void jmgui_tabs_item_end();
void jmgui_get_cursor_pos(float *x, float *y);
void jmgui_set_cursor_pos(float x, float y);
bool jmgui_is_item_active();
void jmgui_get_mouse_delta(float *x, float *y);


#ifdef __cplusplus
}
#endif
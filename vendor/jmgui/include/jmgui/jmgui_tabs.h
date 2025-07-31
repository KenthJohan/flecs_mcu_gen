#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

bool jmgui_tabs_begin(const char *name, int flags);
void jmgui_tabs_end();
bool jmgui_tabs_item_begin(const char *name, int flags);
void jmgui_tabs_item_end();

#ifdef __cplusplus
}
#endif
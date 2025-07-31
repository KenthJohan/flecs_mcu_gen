#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

bool jmgui_table_begin(const char *name, int columns, int flags);
void jmgui_table_end();
void jmgui_table_setup_column(const char *name, int flags, float width);
void jmgui_table_header_row();
void jmgui_table_next_row(int flags);
void jmgui_table_next_column();
void jmgui_table_merge_begin();
void jmgui_table_merge_end();
void jmgui_table_set_row_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);


#ifdef __cplusplus
}
#endif
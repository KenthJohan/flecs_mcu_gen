#pragma once
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

bool jmgui_input_scalar_n(const char *label, int data_type, void *p_data, int components, const void *p_step, const void *p_step_fast, const char *format, int flags);
bool jmgui_input_text(const char *label, char *buf, size_t buf_size);
bool jmgui_input_i32(const char *label, int32_t *v, int step, int step_fast, int flags);

#ifdef __cplusplus
}
#endif
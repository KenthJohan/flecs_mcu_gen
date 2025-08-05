#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void jmgui_text(const char *text);
void jmgui_textf(const char *fmt, ...);
void jmgui_text_colored(float r, float g, float b, const char *fmt, ...);
bool jmgui_text_link(const char *label);
void jmgui_textf_wrap(const char *fmt, ...);

#ifdef __cplusplus
}
#endif
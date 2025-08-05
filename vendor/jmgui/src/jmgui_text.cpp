#include "jmgui/jmgui_text.h"
#include "imgui.h"

void jmgui_text(const char *text)
{
	ImGui::Text("%s", text);
}

void jmgui_textf(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	ImGui::TextV(fmt, args);
	va_end(args);
}

void jmgui_text_colored_va(float r, float g, float b, const char *fmt, va_list args)
{
	const ImVec4 col = ImVec4(r, g, b, 1.0f);
	ImGui::TextColoredV(col, fmt, args);
}

void jmgui_text_colored(float r, float g, float b, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	jmgui_text_colored_va(r, g, b, fmt, args);
	va_end(args);
}

bool jmgui_text_link(const char *label)
{
	return ImGui::TextLink(label);
}


void jmgui_textf_wrap(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	ImGui::TextWrappedV(fmt, args);
	va_end(args);
}


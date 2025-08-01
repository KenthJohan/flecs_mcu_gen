#include "jmgui/jmgui_color.h"
#include "imgui.h"


void jmgui_color_edit3(float c[3])
{
	static ImGuiColorEditFlags base_flags = ImGuiColorEditFlags_None;
	ImGui::ColorEdit3("MyColor##1", c, base_flags);
}

void jmgui_color_picker4_str(char *str)
{
	if (str == NULL) {
		return;
	}
	if (str[0] != '#') {
		return;
	}
	size_t l = strlen(str);
	if (l != 9) {
		return;
	}
	char *e = NULL;
	long v = strtol(str + 1, &e, 16);
	long r = (v >> 24) & 0xFF;
	long g = (v >> 16) & 0xFF;
	long b = (v >> 8) & 0xFF;
	long a = (v >> 0) & 0xFF;
	ImVec4 c = ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
	ImGui::ColorPicker4("##picker", (float *)&c, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
	snprintf(str + 1, 9, "%02X%02X%02X%02X", (unsigned int)(c.x * 255.0f), (unsigned int)(c.y * 255.0f), (unsigned int)(c.z * 255.0f), (unsigned int)(c.w * 255.0f));
}

void jmgui_color_picker3_str(char *str)
{
	if (str == NULL) {
		return;
	}
	if (str[0] != '#') {
		return;
	}
	size_t l = strlen(str);
	if (l != 7) {
		return;
	}
	char *e = NULL;
	long v = strtol(str + 1, &e, 16);
	long r = (v >> 16) & 0xFF;
	long g = (v >> 8) & 0xFF;
	long b = (v >> 0) & 0xFF;
	float c[3] = {r / 255.0f, g / 255.0f, b / 255.0f};
	// ImGui::ColorPicker3("##picker", (float *)&c, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
	ImGui::ColorEdit3("MyColor##3", (float *)&c, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
	snprintf(str + 1, 7, "%02X%02X%02X", (unsigned int)(c[0] * 255.0f), (unsigned int)(c[1] * 255.0f), (unsigned int)(c[2] * 255.0f));
}
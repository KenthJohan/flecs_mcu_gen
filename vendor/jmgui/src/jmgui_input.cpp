#include "jmgui/jmgui_input.h"
#include "imgui.h"

bool jmgui_input_i32(const char *label, int32_t *v, int step, int step_fast, int flags)
{
	return ImGui::InputInt(label, v, step, step_fast, flags);
}

bool jmgui_input_scalar_n(const char *label, int data_type, void *p_data, int components, const void *p_step, const void *p_step_fast, const char *format, int flags)
{
	return ImGui::InputScalarN(label, data_type, p_data, components, p_step, p_step_fast, format, flags);
}

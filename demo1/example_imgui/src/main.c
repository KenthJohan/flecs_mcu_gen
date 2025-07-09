#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <jmgui.h>

int main()
{
	jmgui_context_t jmgui = {.clear_color = {0.45f, 0.55f, 0.60f, 1.00f}};
	jmgui_context_init(&jmgui);
	while (!jmgui.done) {
		jmgui_frame_begin(&jmgui);
		jmgui_frame_frame(&jmgui);
	}
	jmgui_context_fini(&jmgui);
	return 0;
}

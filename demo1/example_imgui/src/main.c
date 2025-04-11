#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <eximgui.h>

int main()
{
	jmgui_context_t eximgui = {.clear_color = {0.45f, 0.55f, 0.60f, 1.00f}};
	jmgui_context_init(&eximgui);
	while (!eximgui.done) {
		eximgui_progress(&eximgui);
	}
	jmgui_context_fini(&eximgui);
	return 0;
}

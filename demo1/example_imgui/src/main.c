#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <eximgui.h>

int main()
{
	eximgui_t eximgui = {.clear_color = {0.45f, 0.55f, 0.60f, 1.00f}};
	eximgui_init(&eximgui);
	while (!eximgui.done) {
		eximgui_progress(&eximgui);
	}
	eximgui_fini(&eximgui);
	return 0;
}

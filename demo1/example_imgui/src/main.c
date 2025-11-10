#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <jmgui.h>

void tab1()
{
	if (jmgui_tabs_item_begin("tab1", 0)) {
		jmgui_text("tab1!");
		jmgui_tabs_item_end();
	}
}

void tab2()
{
	if (jmgui_tabs_item_begin("tab2", 0)) {
		jmgui_text("tab2!");
		jmgui_button("btn1");
		jmgui_tabs_item_end();
	}
}

static void draw()
{
	if (jmgui_begin("Hello", NULL)) {
		jmgui_text("World!");
		if (jmgui_tabs_begin("t1", 0)) {
			tab1();
			jmgui_tabs_end();
		}
		if (jmgui_tabs_begin("t1", 0)) {
			tab2();
			jmgui_tabs_end();
		}
	}
	jmgui_end();
}

int main()
{
	jmgui_context_t jmgui = {.clear_color = {0.45f, 0.55f, 0.60f, 1.00f}};
	jmgui_context_init(&jmgui, "jmgui Tabs Example", 1280, 720);
	while (!jmgui.done) {
		jmgui_frame_begin(&jmgui);
		draw();
		jmgui_frame_frame(&jmgui);
	}
	jmgui_context_fini(&jmgui);
	return 0;
}

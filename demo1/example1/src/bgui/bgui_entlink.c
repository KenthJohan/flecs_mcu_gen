#include "bgui_entlink.h"

#include "../Gui.h"

#include <jmgui.h>


void bgui_entlink_draw(ecs_world_t * world, ecs_entity_t gui, ecs_entity_t subject)
{
	char const * name = ecs_get_name(world, subject);
	bool clicked = jmgui_text_link(name);
	if (clicked) {
		ecs_add_pair(world, gui, GuiClicked, subject);

		ecs_emit(world, &(ecs_event_desc_t) {
			.event = ecs_id(GuiEventClick),
			.entity = gui,
			.param = &(GuiEventClick){gui, subject} // pass payload
		});

	}
}
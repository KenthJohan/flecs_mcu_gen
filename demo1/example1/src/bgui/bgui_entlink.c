#include "bgui_entlink.h"
#include "../Gui.h"
#include "../IconsForkAwesome.h"
#include <jmgui.h>
#include <stdio.h>

// https://forkaweso.me/Fork-Awesome/icons/
void bgui_entlink_draw(ecs_world_t * world, ecs_entity_t gui, ecs_entity_t subject)
{
	ecs_log_set_level(1);
	char const * name = ecs_get_name(world, subject);
	char buf[128];
	char const * icon = "";
	if (ecs_has_id(world, subject, ecs_id(EcsComponent))) {
		icon = ICON_FK_TH;
	} else if (ecs_has_id(world, subject, EcsTraversable)) {
		icon = ICON_FK_RETWEET;
	} else if (ecs_has_id(world, subject, EcsTrait)) {
		icon = ICON_FK_BOOKMARK;
	} else {
		icon = ICON_FK_CIRCLE;
	}

	jmgui_text_colored(0.1,0.5,0.5, "%s", icon);
	jmgui_sameline();

	snprintf(buf, sizeof(buf), "%s", name);
	bool clicked = jmgui_text_link(buf);
	if (clicked) {
		ecs_trace("Emit!");
		ecs_emit(world, &(ecs_event_desc_t) {
			.event = ecs_id(GuiEventClick),
			.entity = gui,
			.ids = &(ecs_type_t){ (ecs_id_t[]){ ecs_id(GuiElement), ecs_id(GuiQueryColumn) }, 2 },
			.param = &(GuiEventClick){gui, subject} // pass payload
		});
	}
	ecs_log_set_level(0);
}
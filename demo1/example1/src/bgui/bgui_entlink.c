#include "bgui_entlink.h"
#include "../Gui.h"
#include "../IconsForkAwesome.h"
#include <jmgui.h>
#include <stdio.h>

// https://forkaweso.me/Fork-Awesome/icons/
void bgui_entlink_draw(ecs_world_t * world, ecs_entity_t gui, ecs_entity_t subject)
{
	ecs_log_set_level(1);
	char const * name = "?";
	char const * icon = "";
	if (subject) {
		name = ecs_get_name(world, subject);
		if (ecs_has_id(world, subject, ecs_id(EcsComponent))) {
			icon = ICON_FK_CUBE;
		} else if (ecs_has_id(world, subject, ecs_id(EcsMember))) {
			icon = ICON_FK_NEUTER;
		} else if (subject == EcsOnInstantiate) {
			icon = ICON_FK_MARS_STROKE_V;
		} else if (subject == EcsOnDeleteTarget) {
			icon = ICON_FK_MARS_STROKE_V;
		} else if (subject == EcsTraversable) {
			icon = ICON_FK_ARROWS_V;
		} else if (ecs_has_id(world, subject, EcsTrait)) {
			icon = ICON_FK_BOOKMARK;
		}else {
			icon = ICON_FK_CIRCLE_O;
		}
	} else {
		icon = ICON_FK_BAN;
	}

	jmgui_text_colored(0.4,0.6,0.2, "%s", icon);

	if (subject) {
		jmgui_sameline();
		char buf[128];
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
	}

	ecs_log_set_level(0);
}
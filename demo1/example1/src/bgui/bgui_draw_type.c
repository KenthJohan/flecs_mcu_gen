#include "bgui_draw_type.h"
#include "Ec.h"
#include <stdio.h>
#include "jmgui.h"


bool jmgui_draw_type(ecs_world_t *world, ecs_entity_t type, void const *ptr)
{
	bool clicked = false;
	if (type == ecs_id(EcInteger)) {
		EcInteger const *q = (EcInteger *)ptr;
		EcsUnit const *u = ecs_get(world, q->unit, EcsUnit);
		char buf[64];
		if (q->unit == EcsBytes) {
			snprintf(buf, sizeof(buf), "0x%jx", q->value);
		} else {
			snprintf(buf, sizeof(buf), "%ju", q->value);
		}
		jmgui_text(buf);
		jmgui_sameline();
		jmgui_text_colored(0.3, 0.3, 0.7, "%s", u->symbol);
	} else if (type == ecs_id(EcsIdentifier)) {
		EcsIdentifier const *q = (EcsIdentifier *)ptr;
		// jmgui_text_colored(1.0, 1.0, 0.7, "%s", q->value);
		clicked = jmgui_text_link(q->value);
	} else {
		char *json = ecs_ptr_to_json(world, type, ptr);
		if (json) {
			jmgui_text(json);
			ecs_os_free(json);
		}
	}
	return clicked;
}
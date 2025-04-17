#include "GuiDraws.h"
#include "Gui.h"
#include "bgui.h"
#include <jmgui.h>
#include "ecs0.h"
#include "bgui_qtable.h"

static void SystemGuiWindow2(ecs_world_t *world, ecs_entity_t e);

static void SystemGuiWindow1(ecs_world_t *world, ecs_entity_t parent)
{
	ecs_iter_t it = ecs_children(world, parent);
	while (ecs_children_next(&it)) {
		for (int i = 0; i < it.count; i++) {
			ecs_entity_t e = it.entities[i];
			SystemGuiWindow2(world, e);
		}
	}
}



static void SystemGuiWindow2(ecs_world_t *world, ecs_entity_t e)
{
	char const *name = ecs_get_name(world, e);
	if (!name) {
		return;
	}
	GuiElement *el = ecs_get_mut(world, e, GuiElement);
	el->id = jmgui_get_id_by_string(name);

	switch (el->type) {
	case GuiTypeTabs:
		if (jmgui_tab_begin(name, 0)) {
			SystemGuiWindow1(world, e);
			jmgui_tab_end();
		}
		break;
	case GuiTypeTab:
		if (jmgui_tab_item_begin(name, 0)) {
			SystemGuiWindow1(world, e);
			jmgui_tab_item_end();
		}
		break;
	case GuiTypeInputText:
		if (1) {
			char buf[128] = {0};
			if (el == NULL) {
				return;
			}
			const EcsDocDescription *ptr = ecs_get(world, el->storage, EcsDocDescription);
			if (ptr == NULL) {
				return;
			}
			if (el->storage == 0) {
				return;
			}
			ecs_os_strncpy(buf, ptr->value, sizeof(buf));
			if (jmgui_input_text(name, buf, sizeof(buf))) {
				/* Safe, value gets copied by copy hook */
				ecs_set(world, el->storage, EcsDocDescription, {.value = ECS_CONST_CAST(char *, buf)});
			}
			el->id = jmgui_get_id_by_string(name);
		}
		break;
	case GuiTypeNodeTreeReflection:
		if (1) {
			bgui_qtable_draw(world, e, el->storage);
		}
		break;

	default:
		break;
	}
}

static void SystemGuiWindow(ecs_iter_t *it)
{
	//GuiWindow *win = ecs_field(it, GuiWindow, 0);
	for (int i = 0; i < it->count; ++i) {
		//ecs_defer_begin(it->world);
		ecs_entity_t e = it->entities[i];
		char const *name = ecs_get_name(it->world, e);
		if (!name) {
			continue;
		}
		if (jmgui_begin(name, NULL)) {
			SystemGuiWindow1(it->world, e);
		}
		jmgui_end();
		//ecs_defer_end(it->world);
	}
}

void GuiDrawsImport(ecs_world_t *world)
{
	ECS_MODULE(world, GuiDraws);
	ecs_set_name_prefix(world, "GuiDraws");

	ecs_system(world,
		{.entity = ecs_entity(world, {.name = "SystemGuiWindow", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
		.callback = SystemGuiWindow,
		//.immediate = true,
		.query.terms = {
		{.id = ecs_id(GuiWindow), .src.id = EcsSelf},
		}});

}

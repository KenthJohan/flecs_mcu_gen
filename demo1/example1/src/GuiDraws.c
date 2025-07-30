#include "GuiDraws.h"
#include "Gui.h"
#include "bgui/bgui.h"
#include <jmgui.h>
#include <stdio.h>
#include <ecsx.h>
#include "bgui/bgui_qtable.h"
#include "bgui/bgui_entinfo.h"
#include <Ec.h>
#include "bgui/bgui_entlink.h"
#include "IconsForkAwesome.h"

static void SystemGuiTraverse2(ecs_world_t *world, ecs_entity_t e);

static void SystemGuiTraverse1(ecs_world_t *world, ecs_entity_t parent)
{
	ecs_iter_t it = ecs_children(world, parent);
	while (ecs_children_next(&it)) {
		for (int i = 0; i < it.count; i++) {
			ecs_entity_t e = it.entities[i];
			if (ecs_has_id(world, e, EcsPrefab)) {
				continue;
			}

			jmgui_push_id_u64(e);
			SystemGuiTraverse2(world, e);
			jmgui_pop_id();
		}
	}
}


/*
	char *id_str = ecs_id_str(world, id);
	jmgui_text_link(id_str);
	ecs_os_free(id_str);
	char *type_str = ecs_type_str(world, type);
	ecs_os_free(type_str);
*/
void iterate_components(ecs_world_t *world, ecs_entity_t egui, ecs_entity_t subject)
{
	const ecs_type_t *type = ecs_get_type(world, subject);
	for (int i = 0; i < type->count; i++) {
		ecs_id_t id = type->array[i];
		void * ptr = NULL;
		ecs_entity_t comp = 0;
		ecs_entity_t tgt = 0;
		if (ECS_HAS_ID_FLAG(id, PAIR)) {
			comp = ecs_pair_first(world, id);
			tgt = ecs_pair_second(world, id);
		} else {
			comp = id & ECS_COMPONENT_MASK;
		}
		// flecs.meta.quantity EcsComponent has zero size. 
		// ecs_get_mut_id() fails if EcsComponent.size is zero.
		EcsComponent const * cc = ecs_get(world, comp, EcsComponent);
		if (cc && (cc->size > 0)) {
			ptr = ecs_get_mut_id(world, subject, id);
		}
		if (comp && tgt) {
			bgui_entlink_draw(world, egui, comp);
			jmgui_sameline();
			jmgui_text(ICON_FK_LONG_ARROW_RIGHT);
			jmgui_sameline();
			bgui_entlink_draw(world, egui, tgt);
		} else if (comp) {
			bgui_entlink_draw(world, egui, comp);
		}
		if (ptr) {
			bgui_entinfo_draw(world, comp, ptr, egui);
		}
		jmgui_separator();
	}
}

static void SystemGuiTraverse2(ecs_world_t *world, ecs_entity_t e)
{
	GuiElement *el = ecs_get_mut(world, e, GuiElement);
	if (el == NULL) {
		// ecs_err("GuiElement not found for entity %s", name);
		return;
	}
	char name[128] = {0};
	{
		char const * name1 = NULL;
		if (name1 == NULL) {
			name1 = ecs_doc_get_name(world, e);
			snprintf(name, sizeof(name), "%s", name1);
		}
		if (name1 == NULL) {
			name1 = ecs_get_name(world, e);
			snprintf(name, sizeof(name), "%s", name1);
		}
		if (name1 == NULL) {
			snprintf(name, sizeof(name), "%jx", e);
		}
		el->id = jmgui_get_id_by_string(name);
	}

	switch (el->type) {
	case GuiTypeWindow: {
		bool open = true;
		if (jmgui_begin(name, &open)) {
			SystemGuiTraverse1(world, e);
		}
		jmgui_end();
		if (open == false) {
			ecs_delete(world, e);
		}
	} break;
	case GuiTypeTabs:
		if (jmgui_tab_begin(name, 0)) {
			SystemGuiTraverse1(world, e);
			jmgui_tab_end();
		}
		break;
	case GuiTypeTab:
		if (jmgui_tab_item_begin(name, 0)) {
			SystemGuiTraverse1(world, e);
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
		if (el->storage) {
			bgui_qtable_draw(world, e, el->storage);
		}
		break;
	case GuiTypeEntityInfo:
		if (el->storage) {
			iterate_components(world, e, el->storage);
		}
		break;

	default:
		break;
	}
}

static void SystemDraw(ecs_iter_t *it)
{
	// GuiRoot *r = ecs_field(it, GuiRoot, 0);
	for (int i = 0; i < it->count; ++i) {
		// ecs_defer_begin(it->world);
		ecs_entity_t e = it->entities[i];
		SystemGuiTraverse1(it->world, e);
		// ecs_defer_end(it->world);
	}
}

void GuiDrawsImport(ecs_world_t *world)
{
	ECS_MODULE(world, GuiDraws);
	ecs_set_name_prefix(world, "GuiDraws");

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "SystemDraw", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = SystemDraw,
	.immediate = true,
	.query.terms = {
	{.id = ecs_id(GuiRoot)},
	}});
}

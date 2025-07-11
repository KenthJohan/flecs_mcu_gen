#include "GuiDraws.h"
#include "Gui.h"
#include "bgui/bgui.h"
#include <jmgui.h>
#include <stdio.h>
#include <ecsx.h>
#include "bgui/bgui_qtable.h"

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

void iterate_components(ecs_world_t *ecs, ecs_entity_t e)
{
	// First get the entity's type, which is a vector of (component) ids.
	const ecs_type_t *type = ecs_get_type(ecs, e);

	// 1. The easiest way to print the components is to use ecs_type_str
	char *type_str = ecs_type_str(ecs, type);
	printf("ecs_type_str: %s\n\n", type_str);
	ecs_os_free(type_str);

	// 2. To print individual ids, iterate the type array with ecs_id_str
	const ecs_id_t *type_ids = type->array;
	int32_t i, count = type->count;

	for (i = 0; i < count; i++) {
		ecs_id_t id = type_ids[i];
		char *id_str = ecs_id_str(ecs, id);
		printf("%d: %s\n", i, id_str);
		ecs_os_free(id_str);
	}

	printf("\n");

	// 3. we can also inspect and print the ids in our own way. This is a
	// bit more complicated as we need to handle the edge cases of what can be
	// encoded in an id, but provides the most flexibility.
	for (i = 0; i < count; i++) {
		ecs_id_t id = type_ids[i];

		printf("%d: ", i);

		if (ECS_HAS_ID_FLAG(id, PAIR)) { // See relationships
			ecs_entity_t rel = ecs_pair_first(ecs, id);
			ecs_entity_t tgt = ecs_pair_second(ecs, id);
			printf("rel: %s, tgt: %s",
			ecs_get_name(ecs, rel), ecs_get_name(ecs, tgt));
		} else {
			ecs_entity_t comp = id & ECS_COMPONENT_MASK;
			printf("entity: %s", ecs_get_name(ecs, comp));
		}

		printf("\n");
	}

	printf("\n\n");
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
		if (1) {
			bgui_qtable_draw(world, e, el->storage);
		}
		break;
	case GuiTypeEntityInfo:
		if (1) {
			iterate_components(world, e);
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
	//.immediate = true,
	.query.terms = {
	{.id = ecs_id(GuiRoot)},
	}});
}

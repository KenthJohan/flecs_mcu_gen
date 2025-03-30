#include "gui.h"
#include <eximgui.h>


bool ecs_has_children(ecs_world_t *world, ecs_entity_t entity)
{
	ecs_iter_t it = ecs_children(world, entity);
	bool r = ecs_iter_is_true(&it);
	ecs_iter_fini(&it);
	return r;
}

void draw_table_columns_from_members(ecs_world_t *world, ecs_entity_t parent)
{
	ecs_iter_t it = ecs_children(world, parent);
	while (ecs_children_next(&it)) {
		for (int i = 0; i < it.count; i++) {
			ecs_entity_t e = it.entities[i];
			EcsMember const *member = ecs_get(world, e, EcsMember);
			if (member == NULL) {
				ecs_warn("Not a member");
				continue;
			}
			if (member->count <= 0) {
				ecs_warn("Too little elements");
				continue;
			}
			char const *name = ecs_get_name(world, e);
			if (name == NULL) {
				ecs_warn("No name");
			}
			gui_table_setup_column(name ? name : "???", 16, 12.0f);
		}
	}
}

void draw_from_members(ecs_world_t *world, ecs_entity_t component, ecs_entity_t target)
{
	char const *value = ecs_get_id(world, target, component);
	ecs_iter_t it = ecs_children(world, component);
	while (ecs_children_next(&it)) {
		for (int i = 0; i < it.count; i++) {
			ecs_entity_t e = it.entities[i];
			EcsMember const *member = ecs_get(world, e, EcsMember);
			if (member == NULL) {
				ecs_warn("Not a member");
				continue;
			}
			if (member->count <= 0) {
				ecs_warn("Too little elements");
				continue;
			}
			char buf[32] = {0};
			if (value == NULL) {
				// Do nothing
			} else if (member->type == ecs_id(ecs_u32_t)) {
				snprintf(buf, sizeof(buf), "u32: %u", *(uint32_t *)(value + member->offset));
			} else if (member->type == ecs_id(ecs_i32_t)) {
				snprintf(buf, sizeof(buf), "i32: %i", *(int32_t *)(value + member->offset));
			}
			gui_table_next_column();
			gui_text(buf);
		}
	}
}

void draw_tree(ecs_world_t *world, ecs_entity_t parent, ecs_entity_t components[])
{
	ecs_iter_t it = ecs_children(world, parent);
	while (ecs_children_next(&it)) {
		for (int i = 0; i < it.count; i++) {
			char const *name = ecs_get_name(world, it.entities[i]);
			if (!name) {
				continue;
			}
			gui_table_next_row(0);
			gui_table_next_column();
			int a = ecs_has_children(world, it.entities[i]);
			if (a) {
				a += gui_tree_node(name, 0);
			} else {
				gui_tree_node(name, 8 | 256 | 512);
			}
			for (int j = 0; components[j]; j++) {
				draw_from_members(world, components[j], it.entities[i]);
			}
			if (a == 2) {
				draw_tree(world, it.entities[i], components);
				gui_tree_pop();
			}
		}
	}
}


void gui_ntt_reflection(ecs_world_t *world, ecs_entity_t parent, ecs_entity_t components[])
{
	int columns = 1;
	for (int j = 0; components[j]; j++) {
		ecs_iter_t it = ecs_children(world, components[j]);
		columns += ecs_iter_count(&it);
	}
	char buf[1288] = {0};
	char * path = ecs_get_path(world, parent);
	snprintf(buf, sizeof(buf), "path: %s, cols: %i", path, columns);
	ecs_os_free(path);
	gui_text(buf);
	gui_table_begin(buf, columns, 0);
	gui_table_setup_column("Name", 128, 0);
	for (int j = 0; components[j]; j++) {
		draw_table_columns_from_members(world, components[j]);
	}
	gui_table_header_row();
	draw_tree(world, parent, components);
	gui_table_end();
}
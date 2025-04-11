#include "bgui.h"
#include <jmgui.h>
#include "Gui.h"


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
		if (value == NULL) {
			continue;
		}
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
			gui_table_next_column();

			if (member->unit == GuiDebugIdUnit) {
				uint32_t id = *(uint32_t *)(value + member->offset);
				char buf[128] = {0};
				snprintf(buf, sizeof(buf), "0x%08x", id);
				gui_text(buf);
				if(gui_last_hover()) {
					gui_debug_locate(id);
				}
			} else if (member->type == ecs_id(ecs_u32_t)) {
				char buf[128] = {0};
				snprintf(buf, sizeof(buf), "%u", *(ecs_u32_t *)(value + member->offset));
				gui_text(buf);
			} else if (member->type == ecs_id(ecs_i32_t)) {
				char buf[128] = {0};
				snprintf(buf, sizeof(buf), "%u", *(ecs_u32_t *)(value + member->offset));
				gui_text(buf);
			} else {
				ecs_strbuf_t buf = ECS_STRBUF_INIT;
				ecs_ptr_to_str_buf(world, member->type, value + member->offset, &buf);
				char const *msg = ecs_strbuf_get(&buf);
				if (msg) {
					gui_text(msg);
					ecs_os_free((char*)msg);
				}
			}
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


int bgui_children_sum(ecs_world_t *world, ecs_entity_t components[], int count)
{
	int total = 0;
	for (int j = 0; j < count; j++) {
		if (components[j] == 0) {
			break;
		}
		ecs_iter_t it = ecs_children(world, components[j]);
		int32_t n = ecs_iter_count(&it);
		if (n > 0) {
			total += n;
		}
	}
	return total;
}


void gui_ntt_reflection(ecs_world_t *world, ecs_entity_t parent, ecs_entity_t components[])
{
	if (parent == 0) {
		return;
	}
	for (int j = 0; components[j]; j++) {
		draw_table_columns_from_members(world, components[j]);
	}
	gui_table_header_row();
	draw_tree(world, parent, components);
}



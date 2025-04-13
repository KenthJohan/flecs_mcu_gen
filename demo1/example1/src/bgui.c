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
			jmgui_table_setup_column(name ? name : "???", 16, 12.0f);
		}
	}
}

typedef struct {
	void const *value;
	ecs_entity_t unit;
	ecs_entity_t type;
} bgui_generic_value_t;

void bgui_generic_value_get(ecs_world_t *world, ecs_entity_t storage, ecs_entity_t member, bgui_generic_value_t *v)
{
	ecs_entity_t component = ecs_get_parent(world, member);
	if (component == 0) {
		return;
	}
	char const *value = ecs_get_id(world, storage, component);
	if (value == NULL) {
		return;
	}
	EcsMember const *m = ecs_get(world, member, EcsMember);
	v->unit = m->unit;
	v->type = m->type;
	if (m == NULL) {
		ecs_warn("Not a member");
		return;
	}
	if (m->count <= 0) {
		ecs_warn("Too little elements");
		return;
	}
	value += m->offset;
	v->value = value;
}

EcsUnit const * bgui_get_row_unit(ecs_world_t *world, ecs_entity_t storage, ecs_entity_t member)
{
	if (storage == 0) {
		return NULL;
	}
	if (member == 0) {
		return NULL;
	}
	bgui_generic_value_t v = {0};
	bgui_generic_value_get(world, storage, member, &v);
	if (v.value == 0) {
		return NULL;
	}
	ecs_entity_t unit = *(ecs_entity_t *)(v.value);
	EcsUnit const *u = ecs_get(world, unit, EcsUnit);
	return u;
}

void draw_generic_member(ecs_world_t *world, ecs_entity_t column, ecs_entity_t row)
{
	GuiColumnComponent const *col = ecs_get(world, column, GuiColumnComponent);
	if (col == NULL) {
		ecs_warn("No column");
		return;
	}
	bgui_generic_value_t v = {0};
	bgui_generic_value_get(world, row, col->member, &v);
	if (v.value == NULL) {
		//char *str = ecs_type_str(world, ecs_get_type(world, target));
		//ecs_warn("No value: %s [%s]", ecs_get_name(world, col->member), str);
		//ecs_os_free(str);
		return;
	}

	EcsUnit const *rowunit = bgui_get_row_unit(world, row, col->unit);

	if (v.type == ecs_id(ecs_u32_t)) {
		if (v.unit == GuiDebugIdUnit) {
			uint32_t id = *(uint32_t *)(v.value);
			char buf[128] = {0};
			snprintf(buf, sizeof(buf), "0x%08x", id);
			jmgui_text(buf);
			if (jmgui_last_hover()) {
				jmgui_debug_locate(id);
			}
		} else {
			char buf[128] = {0};
			snprintf(buf, sizeof(buf), "%u", *(uint32_t *)(v.value));
			jmgui_text(buf);
			if (rowunit) {
				snprintf(buf, sizeof(buf), "%s", rowunit->symbol);
				jmgui_sameline(0, -1);
				jmgui_text_colored(0.5, 0.5, 0.5, "%s", buf);
			}
		}
	} else if (v.type == ecs_id(ecs_i32_t)) {
		char buf[128] = {0};
		snprintf(buf, sizeof(buf), "%i", *(ecs_i32_t *)(v.value));
		jmgui_text(buf);
		if (rowunit) {
			snprintf(buf, sizeof(buf), "%s", rowunit->symbol);
			jmgui_sameline(0, -1);
			jmgui_text_colored(0.5, 0.5, 0.5, "%s", buf);
		}
	} else {
		ecs_strbuf_t buf = ECS_STRBUF_INIT;
		ecs_ptr_to_str_buf(world, v.type, v.value, &buf);
		char const *msg = ecs_strbuf_get(&buf);
		if (msg) {
			jmgui_text(msg);
			ecs_os_free((char *)msg);
		}
	}
}


GuiColor3 const * get_color(ecs_world_t *world, ecs_entity_t table, ecs_entity_t row)
{
	ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
		.terms = {
			{ .id = ecs_pair(ecs_id(GuiColor3), EcsWildcard), .inout = EcsInOutFilter }
		}
	});
	ecs_iter_t it = ecs_query_iter(world, q);
	while (ecs_query_next(&it)) {
		ecs_id_t id = ecs_field_id(&it, 0);
		ecs_entity_t second = ecs_pair_second(world, id);
		if (ecs_has_id(world, row, second)) {
			GuiColor3 const *color = ecs_get_pair(world, table, GuiColor3, second);
			if (color) {
				return color;
			}
		}
	}
	ecs_query_fini(q);
	return NULL;
}


void draw_tree(ecs_world_t *world, ecs_entity_t table, ecs_entity_t storage, ecs_entity_t columns[])
{
	ecs_iter_t it = ecs_children(world, storage);
	while (ecs_children_next(&it)) {
		for (int i = 0; i < it.count; i++) {
			char const *name = ecs_get_name(world, it.entities[i]);
			if (!name) {
				continue;
			}
			jmgui_table_next_row(0);
			jmgui_table_next_column();
			int a = ecs_has_children(world, it.entities[i]);
			GuiColor3 const *color = get_color(world, table, it.entities[i]);
			float c[3] = {1.0f, 1.0f, 1.0f};
			if (color) {
				c[0] = color->r;
				c[1] = color->g;
				c[2] = color->b;
			}
			if (a) {
				a += jmgui_tree_node(name, 0, c[0], c[1], c[2]);
			} else {
				jmgui_tree_node(name, 8 | 256 | 512, c[0], c[1], c[2]);
			}
			for (int j = 0; columns[j]; j++) {
				jmgui_table_next_column();
				draw_generic_member(world, columns[j], it.entities[i]);
			}
			if (a == 2) {
				draw_tree(world, table, it.entities[i], columns);
				jmgui_tree_pop();
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

void bgui_ntt_reflection(ecs_world_t *world, ecs_entity_t table, ecs_entity_t storage, ecs_entity_t columns[])
{
	if (storage == 0) {
		return;
	}
	for (int j = 0; columns[j]; j++) {
		char const *name = ecs_get_name(world, columns[j]);
		if (name == NULL) {
			ecs_warn("No name");
		}
		jmgui_table_setup_column(name ? name : "???", 16, 12.0f);
	}
	jmgui_table_header_row();
	draw_tree(world, table, storage, columns);
}



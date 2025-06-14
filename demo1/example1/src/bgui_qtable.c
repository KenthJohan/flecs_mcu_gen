#include "bgui_qtable.h"
#include "ecs0.h"
#include "ecs0_ser.h"
#include "jmgui.h"
#include "Gui.h"
#include "Ec.h"
#include <stdio.h>

/*
ecs_type_t const *type = ecs_get_type(world, e);
char const *str = ecs_type_str(world, type);
if (str) {
    printf("type: %s: %s\n", ecs_get_name(world, e), str);
    ecs_os_free((char *)str);
}
*/

/*
// Position, Velocity || Speed, Mass
ecs_query_t *q = ecs_query(world, {
  .terms = {
    { ecs_id(Position) },
    { ecs_id(Velocity), .oper = EcsOr },
    { ecs_id(Speed) },
    { ecs_id(Mass) }
  }
});

ecs_iter_t it = ecs_query_iter(world, q);
while (ecs_query_next(&it)) {
  Position *p = ecs_field(&it, Position, 0);
  Mass *m = ecs_field(&it, Mass, 2); // not 3, because of the Or expression

  ecs_id_t vs_id = ecs_field_id(&it, 1);
  if (vs_id == ecs_id(Velocity)) {
    // We can only use ecs_field if the field type is the same for all results,
    // but we can get the table column directly.
    Velocity *v = ecs_table_get_id(world, it.table, ecs_id(Velocity), it.offset);
    // iterate as usual
  } else if (vs_id == ecs_id(Speed)) {
    Speed *s = ecs_table_get_id(world, it.table, ecs_id(Speed), it.offset);
    // iterate as usual
  }
}

for (int i = 0; i < it->field_count; i++) {
    ecs_id_t id = ecs_field_id(it, i);
    char *id_str = ecs_id_str(it->world, id);
    printf("%d: %s, %i\n", i, id_str, ecs_field_is_set(it, i));
    ecs_os_free(id_str);
}


*/

static void ser_component(ecs_world_t *world, ecs_entity_t type, void *data, ecs_strbuf_t *str)
{
	ecs_type_kind_t k = EcsPrimitiveType;
	ecs_primitive_kind_t p = 0;
	ecs_entity_t t = 0;
	EcsMember const *m = ecs_get(world, type, EcsMember);
	if (m) {
		EcsPrimitive const *prim = ecs_get(world, m->type, EcsPrimitive);
		if (prim) {
			k = EcsPrimitiveType;
			p = prim->kind;
		} else {
			t = m->type;
		}
	}

	if (t) {
		EcsType const *info = ecs_get(world, t, EcsType);
		if (info) {
			k = info->kind;
		}
	}

	switch (k) {
	case EcsPrimitiveType:
		ecs0_flecs_expr_ser_primitive(world, p, data, str, false);
		break;
	case EcsBitmaskType:
		ecs0_expr_ser_bitmask(world, t, data, str);
		break;
	case EcsEnumType:
		break;
	case EcsStructType:
		break;
	default:
		break;
	}
}

static void jmgui_qtable_draw_row(ecs_iter_t *it, ecs_entity_t table, GuiTable const *guitable, int archrow, int8_t c2f[16])
{
	/*
	printf("%s\n", ecs_get_name(it->world, it->entities[archrow]));
	for (int i = 0; i < it->field_count; i++) {
	    ecs_id_t id = ecs_field_id(it, i);
	    char *id_str = ecs_id_str(it->world, id);
	    printf("%d: %s, %i\n", i, id_str, ecs_field_is_set(it, i));
	    ecs_os_free(id_str);
	}
	*/

	// Start from column 1, because column 0 is reserved for the name
	ecs_entity_t cols = ecs_lookup_child(it->world, table, "cols");
	ecs_iter_t it1 = ecs_children(it->world, cols);
	int j = 0;
	while (ecs_children_next(&it1)) {
		for (int i = 0; i < it1.count; i++) {
			j++;
			int8_t f = c2f[j];
			ecs_id_t id = ecs_field_id(it, f);
			int size = ecs_field_size(it, f);
			char *data = ecs_field_w_size(it, size, f);
			if (ECS_HAS_ID_FLAG(id, PAIR)) {
				data = NULL;
				ecs_entity_t rel = ecs_pair_first(it->world, id);
				ecs_entity_t tgt = ecs_pair_second(it->world, id);
				// printf("rel: %s, tgt: %s\n\n", ecs_get_name(it->world, rel), ecs_get_name(it->world, tgt));
			}
			char const *msg = NULL;
			if (data) {
				data += ecs_field_is_self(it, f) * archrow * size;
				GuiColumn const *col = ecs_get_pair(it->world, it1.entities[i], GuiColumn, id);
				if (col) {
					ecs_entity_t entlast = 0;
					data += ecs0_sum_offset(it->world, col->members, &entlast);
					ecs_strbuf_t buf = ECS_STRBUF_INIT;
					ser_component(it->world, entlast, data, &buf);
					msg = ecs_strbuf_get(&buf);
				} else {
					ecs_strbuf_t buf = ECS_STRBUF_INIT;
					ecs_ptr_to_str_buf(it->world, id, data, &buf);
					msg = ecs_strbuf_get(&buf);
				}
			}
			jmgui_table_next_column();
			if (msg) {
				// printf("msg: %s\n", msg);
				jmgui_text(msg);
				ecs_os_free((char *)msg);
			} else {
				jmgui_text("NULL");
			}
		}
	}
}

static void get_fields(ecs_world_t *world, ecs_entity_t table, ecs_iter_t *iter, int8_t c2f[16])
{
	// for (int i = 1; i < guitable->columns_count; i++)
	ecs_entity_t cols = ecs_lookup_child(world, table, "cols");
	ecs_iter_t it = ecs_children(world, cols);
	while (ecs_children_next(&it)) {
		for (int i = 0; i < it.count; i++) {
			GuiFields const *fields = ecs_get(world, it.entities[i], GuiFields);
			if (fields == NULL) {
				continue;
			}
			for (int j = 0; j < 16; j++) {
				int8_t f = fields->indices[j];
				if (f == -1) {
					break;
				}
				if (ecs_field_is_set(iter, f)) {
					c2f[i] = f;
					break;
				}
			}
		}
	}
}

void jmgui_qtable_recursive(ecs_world_t *world, ecs_entity_t table, ecs_query_t *q, ecs_entity_t storage, GuiTable const *guitable)
{
	ecs_iter_t it = ecs_query_iter(world, q);
	ecs_iter_set_group(&it, storage);
	while (ecs_query_next(&it)) {

		// Get column to field mapping
		int8_t c2f[16] = {0};
		get_fields(world, table, &it, c2f);

		EcsIdentifier const *id = ecs_get(world, it.entities[0], EcsIdentifier);

		for (int i = 0; i < it.count; i++) {
			ecs_entity_t e = it.entities[i];
			char const *name = ecs_get_name(world, e);
			if (!name) {
				continue;
			}
			// First row is reserved for the tree node
			// Draw the tree node
			jmgui_table_next_row(0);
			jmgui_table_next_column();
			int a = ecs0_has_children(world, e);
			if (a) {
				a += jmgui_tree_node(name, 0, 1, 1, 1);
			} else {
				jmgui_tree_node(name, 8 | 256 | 512, 1, 1, 1);
			}
			// Draw rest of the row
			//jmgui_qtable_draw_row(&it, table, guitable, i, c2f);
			// Check if the entity has children and gui node is open
			if (a == 2) {
				jmgui_qtable_recursive(world, table, q, e, guitable);
				jmgui_tree_pop();
			}
		}
	}
}

void bgui_qtable_draw(ecs_world_t *world, ecs_entity_t table, ecs_entity_t storage)
{
	GuiQuery const *q = ecs_get(world, table, GuiQuery);
	if (q == NULL) {
		return;
	}
	if (q->query == NULL) {
		return;
	}
	char const *name = ecs_get_name(world, table);
	GuiTable const *guitable = ecs_get(world, table, GuiTable);
	if (guitable == NULL) {
		return;
	}

	// Setup the table header with the name and number of columns:
	ecs_entity_t cols = ecs_lookup_child(world, table, "cols");
	int32_t columns_count = ecs0_children_count(world, cols);
	if (columns_count == 0) {
		ecs_warn("Table '%s' has no columns", name);
		return;
	}
	jmgui_table_begin(name, columns_count, 0);

	// Draw each column:
	ecs_iter_t it = ecs_children(world, cols);
	while (ecs_children_next(&it)) {
		for (int i = 0; i < it.count; i++) {
			ecs_entity_t e = it.entities[i];
			char const *colname = ecs_get_name(world, e);
			if (colname) {
				jmgui_table_setup_column(colname, 128, 0);
			} else {
				jmgui_table_setup_column("?", 128, 0);
			}
		}
	}

	jmgui_table_header_row();
	jmgui_qtable_recursive(world, table, q->query, storage, guitable);
	jmgui_table_end();
}

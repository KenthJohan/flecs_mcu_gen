#include "bgui_qtable.h"
#include "ecs0.h"
#include "jmgui.h"
#include "Gui.h"
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



static void print_values(ecs_iter_t *it, GuiTable const *guitable)
{
	// Get field to column mapping
	ecs_id_t c2f[16] = {0};
	for (int i = 0; i < it->field_count; i++) {
		if (ecs_field_is_set(it, i)) {
			int8_t colidx = guitable->f2c[i];
			ecs_id_t id = ecs_field_id(it, i);
			c2f[colidx] = id;
		}
	}
	// Start from column 1, because column 0 is reserved for the name
	for (int i = 1; i < guitable->columns_count; i++) {
		ecs_id_t id = c2f[i];
		void *data = NULL;
		if (id == 0) {
			data = NULL;
		} else if (ECS_HAS_ID_FLAG(id, PAIR)) {
			data = NULL;
			ecs_entity_t rel = ecs_pair_first(it->world, id);
			ecs_entity_t tgt = ecs_pair_second(it->world, id);
			printf("rel: %s, tgt: %s", ecs_get_name(it->world, rel), ecs_get_name(it->world, tgt));
		} else {
			data = ecs_table_get_id(it->world, it->table, id, it->offset);
		}
		char const *msg = NULL;
		if (data) {
			ecs_strbuf_t buf = ECS_STRBUF_INIT;
			ecs_ptr_to_str_buf(it->world, id, data, &buf);
			msg = ecs_strbuf_get(&buf);
		}
		if (msg) {
			// printf("msg: %s\n", msg);
			jmgui_table_next_column();
			jmgui_text(msg);
			ecs_os_free((char *)msg);
		} else {
			jmgui_table_next_column();
			jmgui_text("NULL");
		}
	}
}

void jmgui_qtable_recursive(ecs_world_t *world, ecs_query_t *q, ecs_entity_t storage, GuiTable const *guitable)
{
	ecs_iter_t it = ecs_query_iter(world, q);
	ecs_iter_set_group(&it, storage);
	while (ecs_query_next(&it)) {
		for (int i = 0; i < it.count; i++) {
			ecs_entity_t e = it.entities[i];
			char const *name = ecs_get_name(world, e);
			if (!name) {
				continue;
			}
			jmgui_table_next_row(0);
			jmgui_table_next_column();
			int a = ecs0_has_children(world, e);
			if (a) {
				a += jmgui_tree_node(name, 0, 1, 1, 1);
			} else {
				jmgui_tree_node(name, 8 | 256 | 512, 1, 1, 1);
			}
			print_values(&it, guitable);
			if (a == 2) {
				jmgui_qtable_recursive(world, q, e, guitable);
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
	if (guitable->columns_count > 16) {
		return;
	}
	jmgui_table_begin(name, guitable->columns_count, 0);
	for (int i = 0; guitable->columns[i]; i++) {
		ecs_entity_t e = guitable->columns[i];
		char const *colname = ecs_get_name(world, e);
		if (colname) {
			jmgui_table_setup_column(colname, 128, 0);
		} else {
			jmgui_table_setup_column("?", 128, 0);
		}
	}
	jmgui_table_header_row();
	jmgui_qtable_recursive(world, q->query, storage, guitable);
	jmgui_table_end();
}

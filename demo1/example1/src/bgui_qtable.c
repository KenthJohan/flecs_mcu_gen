#include "bgui_qtable.h"
#include "ecs0.h"
#include "jmgui.h"
#include "Gui.h"

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
*/

void bgui_qtable_terms(ecs_iter_t *it)
{
	for (int i = 0; i < it->field_count; i++) {
		ecs_id_t id = ecs_field_id(it, i);
		void *value = ecs_table_get_id(it->world, it->table, id, it->offset);
	}
}

/*
ecs_type_t const *type = ecs_get_type(world, e);
char const *str = ecs_type_str(world, type);
if (str) {
    printf("type: %s: %s\n", ecs_get_name(world, e), str);
    ecs_os_free((char *)str);
}
*/
void jmgui_qtable_recursive(ecs_world_t *world, ecs_query_t *q, ecs_entity_t parent)
{
	//ecs_query_count_t n = ecs_query_count(q);
	//ecs_log(-1, "query: %p", q);
	ecs_iter_t it = ecs_query_iter(world, q);
	ecs_iter_set_group(&it, parent);
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
			if (a == 2) {
			    jmgui_qtable_recursive(world, q, e);
			    jmgui_tree_pop();
			}
		}
	}
	// ecs_iter_fini(&it);
}

/*
            jmgui_table_begin(name, k+1, 0);
            el->id = jmgui_get_id_by_string(name);
            jmgui_table_setup_column("Name", 128, 0);
            bgui_ntt_reflection(world, e, el->storage, columns);
            jmgui_table_end();
*/

void bgui_qtable_draw(ecs_world_t *world, ecs_entity_t table, ecs_entity_t storage)
{
	/*
	EcsDocDescription const *ptr = ecs_get_pair(world, table, EcsDocDescription, ecs_id(GuiQuery));
	if (ptr == NULL) {
	    return;
	}
	ecs_query_t *q = ecs_query_init(world,
	&(ecs_query_desc_t){
	.expr = ptr->value,
	.group_by = EcsChildOf,
	});
	*/
	GuiQuery const *q = ecs_get(world, table, GuiQuery);
	if (q == NULL) {
		return;
	}
	if (q->query == NULL) {
		return;
	}
	char const *name = ecs_get_name(world, table);
	int columns = 1;
	jmgui_table_begin(name, columns, 0);
	jmgui_table_setup_column("Name", 128, 0);
	jmgui_table_header_row();
	jmgui_qtable_recursive(world, q->query, storage);
	jmgui_table_end();
	//ecs_query_fini(q->query);
}

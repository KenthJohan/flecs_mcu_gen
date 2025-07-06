#include "bgui_qtable.h"
#include "ecs0.h"
#include "ecs0_ser.h"
#include "jmgui.h"
#include "Gui.h"
#include "Ec.h"
#include <stdio.h>


void ecs0_id_info(ecs_world_t *ecs, ecs_id_t id)
{
	if (ECS_HAS_ID_FLAG(id, PAIR)) { // See relationships
		ecs_entity_t rel = ecs_pair_first(ecs, id);
		ecs_entity_t tgt = ecs_pair_second(ecs, id);
		printf("rel: %s, tgt: %s",
		ecs_get_name(ecs, rel), ecs_get_name(ecs, tgt));
	} else {
		ecs_entity_t comp = id & ECS_COMPONENT_MASK;
		printf("entity: %s", ecs_get_name(ecs, comp));
	}
}





int jmgui_qtable_recursive(ecs_entity_t table, ecs_query_t *q, ecs_entity_t estorage, GuiTable * gtable)
{
	ecs_iter_t it = ecs_query_iter(q->world, q);
	ecs_iter_set_group(&it, estorage);
	while (ecs_query_next(&it)) {
		
		//EcsIdentifier const *id = ecs_get(q->world, it.entities[0], EcsIdentifier);
		for (int i = 0; i < it.count; i++) {
			ecs_entity_t e = it.entities[i];
			char const *name = ecs_get_name(q->world, e);
			//printf("name: %s\n", name);
			if (!name) {
				continue;
			}
			bool has_children = false;
			// First row is reserved for the tree node
			jmgui_table_next_row(0);
			jmgui_table_next_column();
			if (ecs0_has_children(q->world, e)) {
				// The entity has children, draw a tree node
				if(jmgui_tree_node(name, 0, 1, 1, 1)) {
					has_children = true;
				}
			} else {
				// The entity has no children, draw a regular text
				jmgui_tree_node(name, 8 | 256 | 512, 1, 1, 1);
			}

			// https://github.com/SanderMertens/flecs/blob/master/src/addons/json/serialize_iter_result_query.c#L198
			int32_t count = ecs_vec_count(&gtable->columns);
			for (int32_t c = 1; c < count; c ++) {
				GuiQueryColumn const *column = ecs_vec_get_t(&gtable->columns, GuiQueryColumn, c);
				jmgui_table_next_column();
				if (ecs_field_is_set(&it, column->field) == false) {
					continue;
				}
				ecs_size_t size = it.sizes[column->field];
				void * ptr = ecs_field_w_size(&it, size, column->field);
				
				if (ptr == NULL) {
					continue;
				}
				if (it.sources[column->field] == 0) {
					ptr = ECS_ELEM(ptr, size, i);
				}
				char * json = ecs_ptr_to_json(q->world, column->type, ptr);
				if (json == NULL) {
					continue;
				}
				jmgui_text(json);
				ecs_os_free(json);
			}

			if (has_children) {
				// The entity has children and the node is open, draw the row
				jmgui_qtable_recursive(table, q, e, gtable);
				jmgui_tree_pop();
			}

		}
	}
	return 0;
}

void bgui_qtable_draw(ecs_world_t *world, ecs_entity_t etable, ecs_entity_t estorage)
{
	GuiQuery const *q = ecs_get(world, etable, GuiQuery);
	if (q == NULL) {
		return;
	}
	if (q->query == NULL) {
		return;
	}
	char const *name = ecs_get_name(world, etable);
	GuiTable *guitable = ecs_get_mut(world, etable, GuiTable);
	if (guitable == NULL) {
		return;
	}

	// Setup the table header with the name and number of columns:
	ecs_entity_t cols = ecs_lookup_child(world, etable, "cols");
	ecs_entities_t entities = ecs_get_ordered_children(world, cols);
	if (entities.count == 0) {
		ecs_warn("Table '%s' has no columns", name);
		return;
	}

	ecs_vec_set_min_count_zeromem(NULL, &guitable->columns, sizeof(GuiQueryColumn), entities.count);
	//ecs_vec_get_t(&guitable->columns, GuiColumn, 0)->members[0] = 1;

	jmgui_table_begin(name, entities.count, 0);


	for (int i = 0; i < entities.count; i++) {
		ecs_entity_t e = entities.ids[i];
		GuiQueryColumn const * c = ecs_get(world, e, GuiQueryColumn);
		(*ecs_vec_get_t(&guitable->columns, GuiQueryColumn, i)) = (*c);
		char const *colname = ecs_get_name(world, e);
		if (colname) {
			jmgui_table_setup_column(colname, 128, 0);
		} else {
			jmgui_table_setup_column("?", 128, 0);
		}
		}


	jmgui_table_header_row();
	jmgui_qtable_recursive(etable, q->query, estorage, guitable);
	jmgui_table_end();
}

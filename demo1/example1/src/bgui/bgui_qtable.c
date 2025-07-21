#include "bgui_qtable.h"
#include <ecsx.h>
#include "jmgui.h"
#include "../Gui.h"
#include "Ec.h"
#include "bgui_draw_type.h"
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





void jmgui_qtable_draw_row(ecs_world_t * world, ecs_iter_t *it, int row, ecs_entities_t columns)
{
	for (int i = 0; i < columns.count; i++) {
		ecs_entity_t e = columns.ids[i];
		GuiQueryColumn const *c = ecs_get(world, e, GuiQueryColumn);
		if (ecs_field_is_set(it, c->field) == false) {
			continue;
		}
		ecs_size_t size = it->sizes[c->field];
		void * ptr = ecs_field_w_size(it, size, c->field);
		if (ptr == NULL) {
			continue;
		}
		if (it->sources[c->field] == 0) {
			ptr = ECS_ELEM(ptr, size, row);
		}
		bool clicked = jmgui_draw_type(world, c->type, ptr);
		if (clicked) {
			//ecs_doc_set_name(world, e, "Hej");
			ecs_entity_t e2[] = {e, it->entities[row]};
			void * data[] = {e2};
			ecs_bulk_init(world, &(ecs_bulk_desc_t) {
				.count = 1,
				.ids = {c->on_click},
				.data = data
			});
			//ecs_add_pair(world, e, GuiClicked, it->entities[row]);
		}
		if ((i+1) >= columns.count) {
			break;
		}
		jmgui_table_next_column();
	}
}


int jmgui_qtable_recursive(ecs_entity_t table, ecs_query_t *q, ecs_entity_t estorage, ecs_entities_t cols_entities)
{
	uint32_t tree_node_flags_leaf = (8 | 256 | 512);
	uint32_t tree_node_flags = ((1 << 19));
	ecs_iter_t it = ecs_query_iter(q->world, q);
	ecs_iter_set_group(&it, estorage);
	while (ecs_query_next(&it)) {
		for (int i = 0; i < it.count; i++) {
			ecs_entity_t e = it.entities[i];
			char const *name = ecs_get_name(q->world, e);
			if (!name) {
				continue;
			}
			bool has_open = false;
			jmgui_table_next_row(0);
			jmgui_table_next_column();
			jmgui_push_id_u64(e); // Allows using same node name of hierarchical data.
			if (ecsx_has_children(q->world, e)) {
				// The entity has children, draw a tree node
				has_open = jmgui_tree_node("", tree_node_flags, 1, 1, 1);
			} else {
				// The entity has no children
				jmgui_tree_node("", tree_node_flags | tree_node_flags_leaf, 1, 1, 1);
			}
			jmgui_sameline();
			jmgui_qtable_draw_row(q->world, &it, i, cols_entities);
			if (has_open) {
				// The entity has children and the tree node is open.
				// Draw all entities again under this node:
				jmgui_qtable_recursive(table, q, e, cols_entities);
				jmgui_tree_pop();
			}
			jmgui_pop_id();

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

	jmgui_table_begin(name, entities.count, 0);


	//jmgui_table_setup_column("?", (1 << 4), 40)
	for (int i = 0; i < entities.count; i++) {
		ecs_entity_t e = entities.ids[i];
		char const *colname = ecs_get_name(world, e);
		if (colname) {
			jmgui_table_setup_column(colname, 128, 0);
		} else {
			jmgui_table_setup_column("?", 128, 0);
		}
	}


	jmgui_table_header_row();
	jmgui_qtable_recursive(etable, q->query, estorage, entities);
	jmgui_table_end();
}

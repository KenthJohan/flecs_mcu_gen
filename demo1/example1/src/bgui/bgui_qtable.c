#include "bgui_qtable.h"
#include <ecsx.h>
#include "jmgui.h"
#include "../Gui.h"
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

bool jmgui_draw_type(ecs_world_t * world, ecs_entity_t type, void const * ptr)
{
	bool clicked = false;
	if (type == ecs_id(EcInteger)) {
		EcInteger const * q = (EcInteger*)ptr;
		EcsUnit const * u = ecs_get(world, q->unit, EcsUnit);
		char buf[64];
		if (q->unit == EcsBytes) {
			snprintf(buf, sizeof(buf), "0x%jx", q->value);
		} else {
			snprintf(buf, sizeof(buf), "%ju", q->value);
		}
		jmgui_text(buf);
		jmgui_sameline();
		jmgui_text_colored(0.3, 0.3, 0.7, "%s", u->symbol);
	} else if (type == ecs_id(EcsIdentifier)) {
		EcsIdentifier const * q = (EcsIdentifier*)ptr;
		//jmgui_text_colored(1.0, 1.0, 0.7, "%s", q->value);
		clicked = jmgui_text_link(q->value);
	} else {
		char * json = ecs_ptr_to_json(world, type, ptr);
		if (json) {
			jmgui_text(json);
			ecs_os_free(json);
		}
	}
	return clicked;
}



void jmgui_qtable_cols(ecs_world_t * world, ecs_iter_t *it, int row, ecs_entities_t entities)
{
	for (int i = 0; i < entities.count; i++) {
		ecs_entity_t e = entities.ids[i];
		GuiQueryColumn const *column = ecs_get(world, e, GuiQueryColumn);
		if (ecs_field_is_set(it, column->field) == false) {
			continue;
		}
		ecs_size_t size = it->sizes[column->field];
		void * ptr = ecs_field_w_size(it, size, column->field);
		if (ptr == NULL) {
			continue;
		}
		if (it->sources[column->field] == 0) {
			ptr = ECS_ELEM(ptr, size, row);
		}
		bool clicked = jmgui_draw_type(world, column->type, ptr);
		if (clicked) {
			ecs_add_pair(world, e, GuiClicked, it->entities[row]);
		}
		jmgui_table_next_column();
	}
}


int jmgui_qtable_recursive(ecs_entity_t table, ecs_query_t *q, ecs_entity_t estorage, ecs_entities_t cols_entities)
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
			bool has_open = false;
			// First row is reserved for the tree node
			jmgui_table_next_row(0);
			jmgui_table_next_column();

			jmgui_push_id_u64(e);
			if (ecsx_has_children(q->world, e)) {
				// The entity has children, draw a tree node
				has_open = jmgui_tree_node("", (0), 1, 1, 1);
			} else {
				// The entity has no children, draw a regular text
				jmgui_tree_node("", (8 | 256 | 512), 1, 1, 1);
			}
			jmgui_sameline();
			//jmgui_text("");
			jmgui_pop_id();

			
			/*
			jmgui_sameline();
			GuiQueryColumn const *column = ecs_vec_get_t(&gtable->columns, GuiQueryColumn, 0);
			ecs_size_t size = it.sizes[column->field];
			void * ptr = ecs_field_w_size(&it, size, column->field);
			if (ptr) {
				jmgui_draw_type(q->world, column->type, ptr);
			}
			*/

			jmgui_qtable_cols(q->world, &it, i, cols_entities);

			if (has_open) {
				// The entity has children and the node is open, draw the row
				jmgui_qtable_recursive(table, q, e, cols_entities);
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

#include "bgui_entinfo.h"
#include "Ec.h"
#include <stdio.h>
#include "jmgui.h"
#include <ecsx.h>
#include <inttypes.h>
#include "bgui_entlink.h"
#include "bgui_entser.h"
#include "../IconsForkAwesome.h"
#include "ImGuiTreeNodeFlags_.h"



#define NODE_DEFAULT (ImGuiTreeNodeFlags_DrawLinesFull)
#define NODE_LEAF (ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet)



void bgui_entinfo_iterate_components(ecs_world_t *world, ecs_entity_t egui, ecs_entity_t subject)
{
	{
		// Draw table header columns
		char const *name = ecs_get_name(world, subject);
		jmgui_table_begin(name, 7, 0);
		jmgui_table_setup_column("name", 128, 0);
		jmgui_table_setup_column("op", 128|16|32, 6);
		jmgui_table_setup_column("type", 128|16|32, 20);
		jmgui_table_setup_column("n", 128|16|32, 4);
		jmgui_table_setup_column("size", 128|16|32, 4);
		jmgui_table_setup_column("offset", 128|16|32, 4);
		jmgui_table_setup_column("value", 128, 0);
		jmgui_table_header_row();
	}

	const ecs_type_t *type = ecs_get_type(world, subject);
	for (int i = 0; i < type->count; i++) {
		ecs_id_t id = type->array[i];
		ecs_entity_t id_comp = 0;
		ecs_entity_t id_tgt = 0;
		if (ECS_HAS_ID_FLAG(id, PAIR)) {
			id_comp = ecs_pair_first(world, id);
			id_tgt = ecs_pair_second(world, id);
		} else {
			id_comp = id & ECS_COMPONENT_MASK;
		}

		jmgui_push_id_u64(id);

		jmgui_table_next_row(0);
		jmgui_table_next_column();

		// flecs.meta.quantity EcsComponent has zero size. 
		// ecs_get_mut_id() fails if EcsComponent.size is zero.
		EcsComponent const * c = ecs_get(world, id_comp, EcsComponent);
		void * ptr = NULL;
		if (id_comp == EcsConstant) {
			//const ecs_i64_t *v = ecs_get_pair_second(world, constant, EcsConstant, ecs_i64_t);
			ptr = ecs_get_mut_id(world, subject, id);
		} else if (c && (c->size > 0)) {
			ptr = ecs_get_mut_id(world, subject, id);
		}

		jmgui_table_merge_begin();
		//jmgui_table_set_row_color(20,30,30,255);
		bool clicked = false;
		if (ptr) {
			clicked = jmgui_tree_node("", NODE_DEFAULT, 1, 1, 1);
		} else {
			jmgui_tree_node("", NODE_LEAF, 1, 1, 1);
		}
		jmgui_sameline();
		if (id_comp) {
			bgui_entlink_draw(world, egui, id_comp);
		}
		if (id_tgt) {
			jmgui_sameline();
			jmgui_text(ICON_FK_LONG_ARROW_RIGHT);
			jmgui_sameline();
			bgui_entlink_draw(world, egui, id_tgt);
		}
		jmgui_table_merge_end();

		jmgui_table_set_column_index(6);



		if (ptr) {
			if (id == ecs_pair(ecs_id(EcsDocDescription), EcsDocColor)){
				jmgui_color_picker3_str(*(ecs_string_t*)ptr);
			} else if (id_comp == ecs_id(EcsDocDescription)) {
				//jmgui_text(*(ecs_string_t*)ptr);
				jmgui_textf_wrap("%s", *(ecs_string_t*)ptr);
			} else if (id_comp == ecs_id(EcsIdentifier)) {
				EcsIdentifier * identfier = ptr;
				jmgui_text(identfier->value);
			} else if ((id_comp == ecs_id(EcSize)) || (id_comp == ecs_id(EcOffset))) {
				EcInteger * value = ptr;
				EcsUnit const *u = ecs_get(world, value->unit, EcsUnit);
				jmgui_textf("%i", value->value);
				jmgui_sameline();
				jmgui_text_colored(0.3, 0.3, 0.7, "%s", u->symbol);
			} else if (id == ecs_pair(ecs_id(EcsPoly), EcsQuery)){
				const EcsPoly *poly_comp = ptr;
				ecs_query_t *q = poly_comp->poly;
				char * qs = ecs_query_str(q);
				jmgui_textf("%s", qs);
				ecs_os_free(qs);
			} else if (id == ecs_pair(ecs_id(EcsPoly), EcsQuery)){
				const EcsPoly *poly_comp = ptr;
				ecs_query_t *q = poly_comp->poly;
				char * qs = ecs_query_str(q);
				jmgui_textf("%s", qs);
				ecs_os_free(qs);
			} else if (id_tgt == ecs_id(ecs_uptr_t)){
				ecs_i64_t * v = ptr;
				v = ptr;
				jmgui_textf("%ji", *v);
			}
		}

		if (clicked) {
			if (ptr) {
				//bgui_entinfo_draw(world, id_comp, ptr, egui);
				serialize(world, id_comp, ptr, egui);
			}
			jmgui_tree_pop();
		}

		jmgui_pop_id();
		//jmgui_separator();
	}
	jmgui_table_end();
}
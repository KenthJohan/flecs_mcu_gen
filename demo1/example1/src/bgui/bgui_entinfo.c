#include "bgui_entinfo.h"
#include "Ec.h"
#include <stdio.h>
#include "jmgui.h"
#include <ecsx.h>
#include <inttypes.h>
#include "bgui_entlink.h"
#include "../IconsForkAwesome.h"

enum ImGuiTreeNodeFlags_ {
	ImGuiTreeNodeFlags_None = 0,
	ImGuiTreeNodeFlags_Selected = 1 << 0,             // Draw as selected
	ImGuiTreeNodeFlags_Framed = 1 << 1,               // Draw frame with background (e.g. for CollapsingHeader)
	ImGuiTreeNodeFlags_AllowOverlap = 1 << 2,         // Hit testing to allow subsequent widgets to overlap this one
	ImGuiTreeNodeFlags_NoTreePushOnOpen = 1 << 3,     // Don't do a TreePush() when open (e.g. for CollapsingHeader) = no extra indent nor pushing on ID stack
	ImGuiTreeNodeFlags_NoAutoOpenOnLog = 1 << 4,      // Don't automatically and temporarily open node when Logging is active (by default logging will automatically open tree nodes)
	ImGuiTreeNodeFlags_DefaultOpen = 1 << 5,          // Default node to be open
	ImGuiTreeNodeFlags_OpenOnDoubleClick = 1 << 6,    // Open on double-click instead of simple click (default for multi-select unless any _OpenOnXXX behavior is set explicitly). Both behaviors may be combined.
	ImGuiTreeNodeFlags_OpenOnArrow = 1 << 7,          // Open when clicking on the arrow part (default for multi-select unless any _OpenOnXXX behavior is set explicitly). Both behaviors may be combined.
	ImGuiTreeNodeFlags_Leaf = 1 << 8,                 // No collapsing, no arrow (use as a convenience for leaf nodes).
	ImGuiTreeNodeFlags_Bullet = 1 << 9,               // Display a bullet instead of arrow. IMPORTANT: node can still be marked open/close if you don't set the _Leaf flag!
	ImGuiTreeNodeFlags_FramePadding = 1 << 10,        // Use FramePadding (even for an unframed text node) to vertically align text baseline to regular widget height. Equivalent to calling AlignTextToFramePadding() before the node.
	ImGuiTreeNodeFlags_SpanAvailWidth = 1 << 11,      // Extend hit box to the right-most edge, even if not framed. This is not the default in order to allow adding other items on the same line without using AllowOverlap mode.
	ImGuiTreeNodeFlags_SpanFullWidth = 1 << 12,       // Extend hit box to the left-most and right-most edges (cover the indent area).
	ImGuiTreeNodeFlags_SpanLabelWidth = 1 << 13,      // Narrow hit box + narrow hovering highlight, will only cover the label text.
	ImGuiTreeNodeFlags_SpanAllColumns = 1 << 14,      // Frame will span all columns of its container table (label will still fit in current column)
	ImGuiTreeNodeFlags_LabelSpanAllColumns = 1 << 15, // Label will span all columns of its container table
	// ImGuiTreeNodeFlags_NoScrollOnOpen     = 1 << 16,  // FIXME: TODO: Disable automatic scroll on TreePop() if node got just open and contents is not visible
	ImGuiTreeNodeFlags_NavLeftJumpsToParent = 1 << 17, // Nav: left arrow moves back to parent. This is processed in TreePop() when there's an unfullfilled Left nav request remaining.
	ImGuiTreeNodeFlags_CollapsingHeader = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_NoAutoOpenOnLog,

	// [EXPERIMENTAL] Draw lines connecting TreeNode hierarchy. Discuss in GitHub issue #2920.
	// Default value is pulled from style.TreeLinesFlags. May be overridden in TreeNode calls.
	ImGuiTreeNodeFlags_DrawLinesNone = 1 << 18,    // No lines drawn
	ImGuiTreeNodeFlags_DrawLinesFull = 1 << 19,    // Horizontal lines to child nodes. Vertical line drawn down to TreePop() position: cover full contents. Faster (for large trees).
	ImGuiTreeNodeFlags_DrawLinesToNodes = 1 << 20, // Horizontal lines to child nodes. Vertical line drawn down to bottom-most child node. Slower (for large trees).

#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
	ImGuiTreeNodeFlags_NavLeftJumpsBackHere = ImGuiTreeNodeFlags_NavLeftJumpsToParent, // Renamed in 1.92.0
	ImGuiTreeNodeFlags_SpanTextWidth = ImGuiTreeNodeFlags_SpanLabelWidth,              // Renamed in 1.90.7
	ImGuiTreeNodeFlags_AllowItemOverlap = ImGuiTreeNodeFlags_AllowOverlap,             // Renamed in 1.89.7
#endif
};

enum ImGuiDataType_
{
    ImGuiDataType_S8,       // signed char / char (with sensible compilers)
    ImGuiDataType_U8,       // unsigned char
    ImGuiDataType_S16,      // short
    ImGuiDataType_U16,      // unsigned short
    ImGuiDataType_S32,      // int
    ImGuiDataType_U32,      // unsigned int
    ImGuiDataType_S64,      // long long / __int64
    ImGuiDataType_U64,      // unsigned long long / unsigned __int64
    ImGuiDataType_Float,    // float
    ImGuiDataType_Double,   // double
    ImGuiDataType_Bool,     // bool (provided for user convenience, not supported by scalar widgets)
    ImGuiDataType_String,   // char* (provided for user convenience, not supported by scalar widgets)
    ImGuiDataType_COUNT
};

#define NODE_DEFAULT (ImGuiTreeNodeFlags_DrawLinesFull)
#define NODE_LEAF (ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet)

static bool col_name(ecs_meta_op_t *op, int i)
{
	bool clicked = false;
	jmgui_table_next_column();
	char buf[128];
	//snprintf(buf, sizeof(buf), "%i:%s", i, op->name ? op->name : "?");
	snprintf(buf, sizeof(buf), "%s", op->name ? op->name : "root");
	if (op->kind < EcsOpPop) {
		clicked = jmgui_tree_node(buf, NODE_DEFAULT, 1, 1, 1);
	} else {
		jmgui_tree_node(buf, NODE_DEFAULT | NODE_LEAF, 1, 1, 1);
		clicked = false;
	}
	return clicked;
}


static bool col_op(ecs_meta_op_t *op, int i)
{
	jmgui_table_next_column();
	jmgui_text(ecsx_meta_type_op_kind_str(op->kind));
	return false;
}


static bool col_type(ecs_world_t * world, ecs_entity_t egui, ecs_meta_op_t *op, int i)
{
	jmgui_table_next_column();
	bgui_entlink_draw(world, egui, op->type);
	return false;
}

static bool col_n(ecs_meta_op_t *op, int i)
{
	jmgui_table_next_column();
	char buf[128];
	snprintf(buf, sizeof(buf), "%i", op->op_count);
	jmgui_text(buf);
	return false;
}

static bool col_size(ecs_meta_op_t *op, int i)
{
	jmgui_table_next_column();
	char buf[128];
	snprintf(buf, sizeof(buf), "%i", op->elem_size);
	jmgui_text(buf);
	return false;
}

static bool col_offset(ecs_meta_op_t *op, int i)
{
	jmgui_table_next_column();
	char buf[128];
	snprintf(buf, sizeof(buf), "%i", op->offset);
	jmgui_text(buf);
	return false;
}



static bool col_value(ecs_world_t * world, ecs_entity_t egui, ecs_meta_op_t *op, int i, void * ptr)
{
	void * data = ECS_OFFSET(ptr, op->offset);
	jmgui_push_id_u64(i);
	jmgui_table_next_column();
	switch (op->kind) {
	case EcsOpU8:
		jmgui_input_scalar_n("input", ImGuiDataType_U8, data, op->op_count, NULL, NULL, "%"PRIu8, 0);
		break;
	case EcsOpU16:
		jmgui_input_scalar_n("input", ImGuiDataType_U16, data, op->op_count, NULL, NULL, "%"PRIu16, 0);
		break;
	case EcsOpU32:
		jmgui_input_scalar_n("input", ImGuiDataType_U32, data, op->op_count, NULL, NULL, "%"PRIu32, 0);
		break;
	case EcsOpU64:
		jmgui_input_scalar_n("input", ImGuiDataType_U64, data, op->op_count, NULL, NULL, "%"PRIu64, 0);
		break;
	case EcsOpI8:
		jmgui_input_scalar_n("input", ImGuiDataType_S8, data, op->op_count, NULL, NULL, "%"PRIi8, 0);
		break;
	case EcsOpI16:
		jmgui_input_scalar_n("input", ImGuiDataType_S16, data, op->op_count, NULL, NULL, "%"PRIi16, 0);
		break;
	case EcsOpI32:
		jmgui_input_scalar_n("input", ImGuiDataType_S32, data, op->op_count, NULL, NULL, "%"PRIi32, 0);
		break;
	case EcsOpI64:
		jmgui_input_scalar_n("input", ImGuiDataType_S64, data, op->op_count, NULL, NULL, "%"PRIi64, 0);
		break;
	case EcsOpF32:
		jmgui_slider_float("input", data, -10.0f, 10.0f);
		break;
	case EcsOpF64:
		break;
	case EcsOpString:
		//jmgui_text(*(ecs_string_t*)data);
		jmgui_textf_wrap("%s", *(ecs_string_t*)data);
		break;
	case EcsOpEntity:
		bgui_entlink_draw(world, egui, *(ecs_entity_t*)data);
		break;
	default:
		break;
	}
	jmgui_pop_id();
	return false;
}


// printf("TREE(%02i) %7s, o:%i\n", i, ecsx_meta_type_op_kind_str(op->kind), sp, o);
bool bgui_entinfo_draw(ecs_world_t *world, ecs_entity_t type, void *ptr, ecs_entity_t egui)
{
	const EcsComponent *comp = ecs_get(world, type, EcsComponent);
	if (!comp) {
		//char *path = ecs_get_path(world, type);
		//ecs_err("cannot serialize to JSON, '%s' is not a component", path);
		//ecs_os_free(path);
		return -1;
	}
	const EcsTypeSerializer *ser = ecs_get(world, type, EcsTypeSerializer);
	if (!ser) {
		//char *path = ecs_get_path(world, type);
		//ecs_err("cannot serialize to JSON, '%s' has no reflection data", path);
		//ecs_os_free(path);
		return -1;
	}


	ecs_meta_op_t *ops = ecs_vec_first_t(&ser->ops, ecs_meta_op_t);
	int32_t count = ecs_vec_count(&ser->ops);
	for (int i = 0; i < count; ++i) {
		ecs_meta_op_t *op = ops + i;
		if (op->kind == EcsOpPop) {
			jmgui_tree_pop();
			continue;
		} 
		jmgui_table_next_row(0);
		// Draw row
		bool o = col_name(op, i);
		col_op(op, i);
		col_type(world, egui, op, i);
		col_n(op, i);
		col_size(op, i);
		col_offset(op, i);
		col_value(world, egui, op, i, ptr);
		// Collapse tree view
		if ((op->kind < EcsOpPop) && (o == false)) {
			int s = 1;
			do {
				i++;
				s -= (ops[i].kind == EcsOpPop);
				s += (ops[i].kind < EcsOpPop);
			} while ((ops[i].kind != EcsOpPop) || (s > 0));
		}
	}
	return false;
}



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
				bgui_entinfo_draw(world, id_comp, ptr, egui);
			}
			jmgui_tree_pop();
		}

		jmgui_pop_id();
		//jmgui_separator();
	}
	jmgui_table_end();
}
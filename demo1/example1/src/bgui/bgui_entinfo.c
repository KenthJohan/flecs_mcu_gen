#include "bgui_entinfo.h"
#include "Ec.h"
#include <stdio.h>
#include "jmgui.h"
#include <ecsx.h>
#include <inttypes.h>
#include "bgui_entlink.h"
#include "../IconsForkAwesome.h"
#include "ImGuiTreeNodeFlags_.h"
#include "ImGuiDataType_.h"

#define NODE_DEFAULT (ImGuiTreeNodeFlags_DrawLinesFull)
#define NODE_LEAF (ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet)

void serializeOps(ecs_world_t *world, ecs_meta_op_t *ops, int32_t op_count, const void *ptr, ecs_entity_t egui);
void serializeEnum(ecs_meta_op_t *op, const void *ptr);
int serialize(ecs_world_t *world, ecs_entity_t type, const void *ptr, ecs_entity_t egui);

static bool col_name(ecs_meta_op_t *op, int i)
{
	bool clicked = false;
	jmgui_table_next_column();
	char buf[128];
	// snprintf(buf, sizeof(buf), "%i:%s", i, op->name ? op->name : "?");
	if (op->name) {
		snprintf(buf, sizeof(buf), "%s", op->name);
	} else {
		snprintf(buf, sizeof(buf), "[%i]", op->member_index);
	}
	if (op->kind < EcsOpPop) {
		clicked = jmgui_tree_node(buf, NODE_DEFAULT, 1, 1, 1);
	} else {
		jmgui_tree_node(buf, NODE_DEFAULT | NODE_LEAF, 1, 1, 1);
		clicked = false;
	}
	return clicked;
}

static bool col_op(ecs_meta_op_t *op)
{
	jmgui_table_next_column();
	jmgui_text(ecsx_meta_type_op_kind_str(op->kind));
	return false;
}

static bool col_type(ecs_world_t *world, ecs_entity_t egui, ecs_meta_op_t *op)
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

static bool col_value(ecs_world_t *world, ecs_entity_t egui, ecs_meta_op_t *op, int i, void *ptr)
{
	jmgui_table_next_column();
	switch (op->kind) {
	// Opaque types have in-memory representations that are opaque to
	// the reflection framework and cannot be serialized by just taking
	// a pointer + an offset. See src/addons/script/serialize.c for an
	// example of how to handle opaque types.
	case EcsOpOpaqueStruct:
	case EcsOpOpaqueArray:
	case EcsOpOpaqueVector:
	case EcsOpOpaqueValue:
		break;

	// Forward to type. Used for members of array/vector types.
	case EcsOpForward:
		serialize(world, op->type, ptr, egui);
		break;

	// Serialize single values
	case EcsOpEnum:
		serializeEnum(op, ptr);
		break;
	case EcsOpBitmask:
		// Bitmask serialization requires iterating all the bits in a value
		// and looking up the corresponding constant. For an example, see
		// src/addons/script/serialize.c.
		break;
	case EcsOpBool:
		// printf("%s", (*(const bool *)ptr) ? "true" : "false");
		break;
	case EcsOpChar:
		// printf("'%c'", *(const char *)ptr);
		break;
	case EcsOpByte:
	case EcsOpU8:
		// printf("%" PRIu8, *(const uint8_t *)ptr);
		break;
	case EcsOpU16:
		// printf("%" PRIu16, *(const uint16_t *)ptr);
		break;
	case EcsOpU32:
		// printf("%" PRIu32, *(const uint32_t *)ptr);
		break;
	case EcsOpU64:
		// printf("%" PRIu64, *(const uint64_t *)ptr);
		break;
	case EcsOpI8:
		// printf("%" PRIi8, *(const int8_t *)ptr);
		break;
	case EcsOpI16:
		// printf("%" PRIi16, *(const int16_t *)ptr);
		jmgui_input_scalar_n("input", ImGuiDataType_S16, ptr, 1, NULL, NULL, "%"PRIi16, 0);
		break;
	case EcsOpI32:
		jmgui_input_scalar_n("input", ImGuiDataType_S32, ptr, 1, NULL, NULL, "%"PRIi32, 0);
		// printf("%" PRIi32, *(const int32_t *)ptr);
		break;
	case EcsOpI64:
		// printf("%" PRIi64, *(const int64_t *)ptr);
		break;
	case EcsOpF32:
		jmgui_input_scalar_n("input", ImGuiDataType_Float, ptr, 1, NULL, NULL, "%f", 0);
		// printf("%.2f", (double)*(const float *)ptr);
		break;
	case EcsOpF64:
		// printf("%.2f", *(const double *)ptr);
		break;
	case EcsOpUPtr:
		// printf("%" PRIuPTR, *(const uintptr_t *)ptr);
		break;
	case EcsOpIPtr:
		// printf("%" PRIiPTR, *(const intptr_t *)ptr);
		break;
	case EcsOpString:
		jmgui_textf_wrap("%s", *(ecs_string_t*)ptr);
		// printf("\"%s\"", *(const char **)ptr);
		break;
	case EcsOpEntity: {
		bgui_entlink_draw(world, egui, *(ecs_entity_t*)ptr);
		break;
	}
	case EcsOpId: {
		char *name = ecs_id_str(world, *(ecs_id_t *)ptr);
		// printf("%s\n", name);
		ecs_os_free(name);
		break;
	}
	case EcsOpPop:
		break;
	case EcsOpScope:
	case EcsOpPrimitive:
		// Not serializable
		break;
	default:
		break;
	}
	return false;
}

// Serialize a pointer of a specified type.
int serialize(ecs_world_t *world, ecs_entity_t type, const void *ptr, ecs_entity_t egui)
{
	// The TypeSerializer component contains a vector of instructions that tells
	// us how to serialize the type.
	const EcsTypeSerializer *s = ecs_get(world, type, EcsTypeSerializer);
	if (!s) {
		printf("type does not have reflection data\n");
		return -1;
	}

	ecs_meta_op_t *ops = ecs_vec_first(&s->ops);
	int32_t count = ecs_vec_count(&s->ops);
	serializeOps(world, ops + 1, count - 2, ptr, egui);

	return 0;
}

// Serialize contents of struct or collection scope
void serializeScope(ecs_world_t *world, ecs_meta_op_t *ops, const void *ptr, ecs_entity_t egui)
{
	// A scope starts with a Push and ends with a Pop, so trim the first and
	// last instruction before forwarding.
	serializeOps(world, ops + 1, ops->op_count - 2, ptr, egui);
}

// Serialize a struct scope
void serializeStruct(ecs_world_t *world, ecs_meta_op_t *ops, const void *ptr, ecs_entity_t egui)
{
	serializeScope(world, ops, ptr, egui);
}

// Serialize an array scope
void serializeArray(ecs_world_t *world, ecs_meta_op_t *ops, int32_t elem_count, const void *ptr, ecs_entity_t egui)
{
	// Iterate elements of the array. Skip the first and last instruction
	// since they are PushStruct and Pop.
	for (int i = 0; i < elem_count; i++) {
		ops[1].member_index = i;
		serializeScope(world, ops, ptr, egui);
		ptr = ECS_OFFSET(ptr, ops->elem_size);
	}
}

// Serialize a vector scope
void serializeVector(ecs_world_t *world, ecs_meta_op_t *ops, const void *ptr, ecs_entity_t egui)
{
	const ecs_vec_t *vec = ptr;
	serializeArray(world, ops, vec->count, vec->array, egui);
}

// Serialize enum
void serializeEnum(ecs_meta_op_t *op, const void *ptr)
{
	const int32_t *value = ptr; // Assume i32, but should use underlying type
	ecs_enum_constant_t *c = ecs_map_get_deref(op->is.constants, ecs_enum_constant_t, (ecs_map_key_t)*value);
}

// Iterate over instruction array
void serializeOps(ecs_world_t *world, ecs_meta_op_t *ops, int32_t op_count, const void *base, ecs_entity_t egui)
{
	for (int i = 0; i < op_count; i++) {
		ecs_meta_op_t *op = &ops[i];
		// printf("%30s %i: %s\n", ecsx_meta_type_op_kind_str(op->kind), stack, op->name);
		//  Get pointer for current field
		void *ptr = ECS_OFFSET(base, op->offset);

		jmgui_table_next_row(0);
		jmgui_push_id_u64((uintptr_t)ptr);

		bool expand = col_name(op, i);
		col_op(op);
		col_type(world, egui, op);
		col_n(op, i);
		col_size(op, i);
		col_offset(op, i);
		if (expand) {
			switch (op->kind) {
			case EcsOpPushStruct:
				serializeStruct(world, op, ptr, egui);
				break;
			case EcsOpPushArray:
				serializeArray(world, op, ecs_meta_op_get_elem_count(op, ptr), ptr, egui);
				break;
			case EcsOpPushVector:
				serializeVector(world, op, ptr, egui);
				break;
			default:
				break;
			}
			jmgui_tree_pop();
		} else {
			col_value(world, egui, op, i, ptr);
		}
		jmgui_pop_id();

		i += op->op_count - 1; // Skip over already processed instructions
	}
}

void bgui_entinfo_iterate_components(ecs_world_t *world, ecs_entity_t egui, ecs_entity_t subject)
{
	{
		// Draw table header columns
		char const *name = ecs_get_name(world, subject);
		jmgui_table_begin(name, 7, 0);
		jmgui_table_setup_column("name", 128, 0);
		jmgui_table_setup_column("op", 128 | 16 | 32, 6);
		jmgui_table_setup_column("type", 128 | 16 | 32, 20);
		jmgui_table_setup_column("n", 128 | 16 | 32, 4);
		jmgui_table_setup_column("size", 128 | 16 | 32, 4);
		jmgui_table_setup_column("offset", 128 | 16 | 32, 4);
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
		EcsComponent const *c = ecs_get(world, id_comp, EcsComponent);
		void *ptr = NULL;
		if (id_comp == EcsConstant) {
			// const ecs_i64_t *v = ecs_get_pair_second(world, constant, EcsConstant, ecs_i64_t);
			ptr = ecs_get_mut_id(world, subject, id);
		} else if (c && (c->size > 0)) {
			ptr = ecs_get_mut_id(world, subject, id);
		}

		jmgui_table_merge_begin();
		// jmgui_table_set_row_color(20,30,30,255);
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
			if (id == ecs_pair(ecs_id(EcsDocDescription), EcsDocColor)) {
				jmgui_color_picker3_str(*(ecs_string_t *)ptr);
			} else if (id_comp == ecs_id(EcsDocDescription)) {
				// jmgui_text(*(ecs_string_t*)ptr);
				jmgui_textf_wrap("%s", *(ecs_string_t *)ptr);
			} else if (id_comp == ecs_id(EcsIdentifier)) {
				EcsIdentifier *identfier = ptr;
				jmgui_text(identfier->value);
			} else if ((id_comp == ecs_id(EcSize)) || (id_comp == ecs_id(EcOffset))) {
				EcInteger *value = ptr;
				EcsUnit const *u = ecs_get(world, value->unit, EcsUnit);
				jmgui_textf("%i", value->value);
				jmgui_sameline();
				jmgui_text_colored(0.3, 0.3, 0.7, "%s", u->symbol);
			} else if (id == ecs_pair(ecs_id(EcsPoly), EcsQuery)) {
				const EcsPoly *poly_comp = ptr;
				ecs_query_t *q = poly_comp->poly;
				char *qs = ecs_query_str(q);
				jmgui_textf("%s", qs);
				ecs_os_free(qs);
			} else if (id == ecs_pair(ecs_id(EcsPoly), EcsQuery)) {
				const EcsPoly *poly_comp = ptr;
				ecs_query_t *q = poly_comp->poly;
				char *qs = ecs_query_str(q);
				jmgui_textf("%s", qs);
				ecs_os_free(qs);
			} else if (id_tgt == ecs_id(ecs_uptr_t)) {
				ecs_i64_t *v = ptr;
				v = ptr;
				jmgui_textf("%ji", *v);
			}
		}

		if (clicked) {
			if (ptr) {
				// bgui_entinfo_draw(world, id_comp, ptr, egui);
				serialize(world, id_comp, ptr, egui);
			}
			jmgui_tree_pop();
		}

		jmgui_pop_id();
		// jmgui_separator();
	}
	jmgui_table_end();
}
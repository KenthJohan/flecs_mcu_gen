#include "bgui_entser.h"

#include <stdio.h>
#include <inttypes.h>
#include <ecsx.h>

#include "jmgui.h"
#include "bgui_entlink.h"
#include "../IconsForkAwesome.h"
#include "ImGuiTreeNodeFlags_.h"

#define NODE_DEFAULT (ImGuiTreeNodeFlags_DrawLinesFull)
#define NODE_LEAF (ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet)

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

void serializeOps(ecs_world_t *world, ecs_meta_op_t *ops, int32_t op_count, const void *ptr, ecs_entity_t egui);


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
	serializeOps(world, ops+1, count-2, ptr, egui);

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
	ecs_enum_constant_t *c = ecs_map_get_deref(op->is.constants,
	ecs_enum_constant_t, (ecs_map_key_t)*value);
}

// Iterate over instruction array
void serializeOps(ecs_world_t *world, ecs_meta_op_t *ops, int32_t op_count, const void *base, ecs_entity_t egui)
{
	for (int i = 0; i < op_count; i++) {
		ecs_meta_op_t *op = &ops[i];
		//printf("%30s %i: %s\n", ecsx_meta_type_op_kind_str(op->kind), stack, op->name);
		// Get pointer for current field
		void *ptr = ECS_OFFSET(base, op->offset);

		jmgui_table_next_row(0);
		jmgui_push_id_u64((uintptr_t)ptr);

		bool expand = col_name(op, i);
		col_op(op);
		col_type(world, egui, op);

		switch (op->kind) {
		// Instructions that forward to a type scope, like a (nested) struct or
		// collection
		case EcsOpPushStruct:
			if (expand) {
				serializeStruct(world, op, ptr, egui);
			}
			break;
		case EcsOpPushArray:
			if (expand) {
				serializeArray(world, op, ecs_meta_op_get_elem_count(op, ptr), ptr, egui);
			}
			break;
		case EcsOpPushVector:
			if (expand) {
				serializeVector(world, op, ptr, egui);
			}
			break;

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
			//printf("%s", (*(const bool *)ptr) ? "true" : "false");
			break;
		case EcsOpChar:
			//printf("'%c'", *(const char *)ptr);
			break;
		case EcsOpByte:
		case EcsOpU8:
			//printf("%" PRIu8, *(const uint8_t *)ptr);
			break;
		case EcsOpU16:
			//printf("%" PRIu16, *(const uint16_t *)ptr);
			break;
		case EcsOpU32:
			//printf("%" PRIu32, *(const uint32_t *)ptr);
			break;
		case EcsOpU64:
			//printf("%" PRIu64, *(const uint64_t *)ptr);
			break;
		case EcsOpI8:
			//printf("%" PRIi8, *(const int8_t *)ptr);
			break;
		case EcsOpI16:
			//printf("%" PRIi16, *(const int16_t *)ptr);
			break;
		case EcsOpI32:
			//printf("%" PRIi32, *(const int32_t *)ptr);
			break;
		case EcsOpI64:
			//printf("%" PRIi64, *(const int64_t *)ptr);
			break;
		case EcsOpF32:
			//printf("%.2f", (double)*(const float *)ptr);
			break;
		case EcsOpF64:
			//printf("%.2f", *(const double *)ptr);
			break;
		case EcsOpUPtr:
			//printf("%" PRIuPTR, *(const uintptr_t *)ptr);
			break;
		case EcsOpIPtr:
			//printf("%" PRIiPTR, *(const intptr_t *)ptr);
			break;
		case EcsOpString:
			//printf("\"%s\"", *(const char **)ptr);
			break;
		case EcsOpEntity: {
			char *name = ecs_get_path(world, *(ecs_entity_t *)ptr);
			//printf("%s\n", name);
			ecs_os_free(name);
			break;
		}
		case EcsOpId: {
			char *name = ecs_id_str(world, *(ecs_id_t *)ptr);
			//printf("%s\n", name);
			ecs_os_free(name);
			break;
		}
		case EcsOpPop:
			break;
		case EcsOpScope:
		case EcsOpPrimitive:
			// Not serializable
			break;
		}

		if (expand) {
			jmgui_tree_pop();
		}
		jmgui_pop_id();

		i += op->op_count - 1; // Skip over already processed instructions
	}
}

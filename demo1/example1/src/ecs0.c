#include "ecs0.h"
#include <stdlib.h>

int ecs0_get_entities_by_query(ecs_world_t *world, ecs_query_t *query, ecs_entity_t entities[], int count)
{
	ecs_iter_t it = ecs_query_iter(world, query);
	int i = 0;
	while (ecs_query_next(&it)) {
		for (int j = 0; j < it.count; j++) {
			if (i >= count) {
				goto exit_return;
			}
			entities[i] = it.entities[j];
			i++;
		}
	}
exit_return:
	return i;
}

int ecs0_get_entities_from_parent(ecs_world_t *world, ecs_entity_t parent, ecs_id_t component, ecs_entity_t entities[], int count)
{
	ecs_query_t *q = ecs_query(world, {.terms = {
	                                   {ecs_childof(parent), .inout = EcsInOutFilter},
	                                   {component, .inout = EcsInOutFilter},
	                                   }});
	int k = ecs0_get_entities_by_query(world, q, entities, count);
	ecs_query_fini(q);
	return k;
}

bool ecs0_has_children(ecs_world_t *world, ecs_entity_t entity)
{
	ecs_iter_t it = ecs_children(world, entity);
	bool r = ecs_iter_is_true(&it);
	ecs_iter_fini(&it);
	return r;
}

static ecs_primitive_kind_t flecs_expr_op_to_primitive_kind(ecs_meta_type_op_kind_t kind)
{
	return kind - EcsOpPrimitive;
}

static int flecs_expr_ser_type_ops(
const ecs_world_t *world,
ecs_meta_type_op_t *ops,
int32_t op_count,
const void *base,
ecs_strbuf_t *str,
int32_t in_array,
bool is_expr);

#define flecs_uto(T, value) (T)(value)
#define flecs_ito(T, value) (T)(value)

int ecs0_flecs_expr_ser_primitive(
const ecs_world_t *world,
ecs_primitive_kind_t kind,
const void *base,
ecs_strbuf_t *str,
bool is_expr)
{
	switch (kind) {
	case EcsBool:
		if (*(const bool *)base) {
			ecs_strbuf_appendlit(str, "true");
		} else {
			ecs_strbuf_appendlit(str, "false");
		}
		break;
	case EcsChar: {
		/*
		char chbuf[3];
		char ch = *(const char*)base;
		if (ch) {
		    flecs_chresc(chbuf, *(const char*)base, '"');
		    if (is_expr) ecs_strbuf_appendch(str, '"');
		    ecs_strbuf_appendstr(str, chbuf);
		    if (is_expr) ecs_strbuf_appendch(str, '"');
		} else {
		    ecs_strbuf_appendch(str, '0');
		}
		break;
		*/
	}
	case EcsByte:
		ecs_strbuf_appendint(str, flecs_uto(int64_t, *(const uint8_t *)base));
		break;
	case EcsU8:
		ecs_strbuf_appendint(str, flecs_uto(int64_t, *(const uint8_t *)base));
		break;
	case EcsU16:
		ecs_strbuf_appendint(str, flecs_uto(int64_t, *(const uint16_t *)base));
		break;
	case EcsU32:
		ecs_strbuf_appendint(str, flecs_uto(int64_t, *(const uint32_t *)base));
		break;
	case EcsU64:
		ecs_strbuf_append(str, "%llu", *(const uint64_t *)base);
		break;
	case EcsI8:
		ecs_strbuf_appendint(str, flecs_ito(int64_t, *(const int8_t *)base));
		break;
	case EcsI16:
		ecs_strbuf_appendint(str, flecs_ito(int64_t, *(const int16_t *)base));
		break;
	case EcsI32:
		ecs_strbuf_appendint(str, flecs_ito(int64_t, *(const int32_t *)base));
		break;
	case EcsI64:
		ecs_strbuf_appendint(str, *(const int64_t *)base);
		break;
	case EcsF32:
		ecs_strbuf_appendflt(str, (double)*(const float *)base, 0);
		break;
	case EcsF64:
		ecs_strbuf_appendflt(str, *(const double *)base, 0);
		break;
	case EcsIPtr:
		ecs_strbuf_appendint(str, flecs_ito(int64_t, *(const intptr_t *)base));
		break;
	case EcsUPtr:
		ecs_strbuf_append(str, "%u", *(const uintptr_t *)base);
		break;
	case EcsString: {
		const char *value = *ECS_CONST_CAST(const char **, base);
		if (value) {
			if (!is_expr) {
				ecs_strbuf_appendstr(str, value);
			} else {
				ecs_size_t length = flecs_stresc(NULL, 0, '"', value);
				if (length == ecs_os_strlen(value)) {
					ecs_strbuf_appendch(str, '"');
					ecs_strbuf_appendstrn(str, value, length);
					ecs_strbuf_appendch(str, '"');
				} else {
					char *out = ecs_os_malloc(length + 3);
					flecs_stresc(out + 1, length, '"', value);
					out[0] = '"';
					out[length + 1] = '"';
					out[length + 2] = '\0';
					ecs_strbuf_appendstr(str, out);
					ecs_os_free(out);
				}
			}
		} else {
			ecs_strbuf_appendlit(str, "null");
		}
		break;
	}
	case EcsEntity: {
		ecs_entity_t e = *(const ecs_entity_t *)base;
		if (!e) {
			ecs_strbuf_appendlit(str, "#0");
		} else {
			ecs_get_path_w_sep_buf(world, 0, e, ".", NULL, str, false);
		}
		break;
	}
	case EcsId: {
		ecs_id_t id = *(const ecs_id_t *)base;
		if (!id) {
			ecs_strbuf_appendlit(str, "#0");
		} else {
			ecs_id_str_buf(world, id, str);
		}
		break;
	}
	default:
		ecs_err("invalid primitive kind");
		return -1;
	}

	return 0;
}

/* Serialize enumeration */
static int flecs_expr_ser_enum(
const ecs_world_t *world,
ecs_meta_type_op_t *op,
const void *base,
ecs_strbuf_t *str)
{
	const EcsEnum *enum_type = ecs_get(world, op->type, EcsEnum);
	ecs_check(enum_type != NULL, ECS_INVALID_PARAMETER, NULL);

	int32_t val = *(const int32_t *)base;

	/* Enumeration constants are stored in a map that is keyed on the
	 * enumeration value. */
	ecs_enum_constant_t *c = ecs_map_get_deref(&enum_type->constants,
	ecs_enum_constant_t, (ecs_map_key_t)val);
	if (!c) {
		char *path = ecs_get_path(world, op->type);
		ecs_err("value %d is not valid for enum type '%s'", val, path);
		ecs_os_free(path);
		goto error;
	}

	ecs_strbuf_appendstr(str, ecs_get_name(world, c->constant));

	return 0;
error:
	return -1;
}

/* Serialize bitmask */
static int flecs_expr_ser_bitmask(
const ecs_world_t *world,
ecs_meta_type_op_t *op,
const void *ptr,
ecs_strbuf_t *str)
{
	const EcsBitmask *bitmask_type = ecs_get(world, op->type, EcsBitmask);
	ecs_check(bitmask_type != NULL, ECS_INVALID_PARAMETER, NULL);
	uint32_t value = *(const uint32_t *)ptr;

	ecs_strbuf_list_push(str, "", "|");

	/* Multiple flags can be set at a given time. Iterate through all the flags
	 * and append the ones that are set. */
	ecs_map_iter_t it = ecs_map_iter(&bitmask_type->constants);
	int count = 0;
	while (ecs_map_next(&it)) {
		ecs_bitmask_constant_t *c = ecs_map_ptr(&it);
		ecs_map_key_t key = ecs_map_key(&it);
		if ((value & key) == key) {
			ecs_strbuf_list_appendstr(str, ecs_get_name(world, c->constant));
			count++;
			value -= (uint32_t)key;
		}
	}

	if (value != 0) {
		/* All bits must have been matched by a constant */
		char *path = ecs_get_path(world, op->type);
		ecs_err(
		"value for bitmask %s contains bits (%u) that cannot be mapped to constant",
		path, value);
		ecs_os_free(path);
		goto error;
	}

	if (!count) {
		ecs_strbuf_list_appendstr(str, "0");
	}

	ecs_strbuf_list_pop(str, "");

	return 0;
error:
	return -1;
}

/* Serialize elements of a contiguous array */
static int expr_ser_elements(
const ecs_world_t *world,
ecs_meta_type_op_t *ops,
int32_t op_count,
const void *base,
int32_t elem_count,
int32_t elem_size,
ecs_strbuf_t *str,
bool is_array)
{
	ecs_strbuf_list_push(str, "[", ", ");

	const void *ptr = base;

	int i;
	for (i = 0; i < elem_count; i++) {
		ecs_strbuf_list_next(str);
		if (flecs_expr_ser_type_ops(
		    world, ops, op_count, ptr, str, is_array, true)) {
			return -1;
		}
		ptr = ECS_OFFSET(ptr, elem_size);
	}

	ecs_strbuf_list_pop(str, "]");

	return 0;
}

static int expr_ser_type_elements(
const ecs_world_t *world,
ecs_entity_t type,
const void *base,
int32_t elem_count,
ecs_strbuf_t *str,
bool is_array)
{
	const EcsTypeSerializer *ser = ecs_get(
	world, type, EcsTypeSerializer);
	ecs_assert(ser != NULL, ECS_INTERNAL_ERROR, NULL);

	const EcsComponent *comp = ecs_get(world, type, EcsComponent);
	ecs_assert(comp != NULL, ECS_INTERNAL_ERROR, NULL);

	ecs_meta_type_op_t *ops = ecs_vec_first_t(&ser->ops, ecs_meta_type_op_t);
	int32_t op_count = ecs_vec_count(&ser->ops);
	return expr_ser_elements(
	world, ops, op_count, base, elem_count, comp->size, str, is_array);
}

/* Serialize array */
static int expr_ser_array(
const ecs_world_t *world,
ecs_meta_type_op_t *op,
const void *ptr,
ecs_strbuf_t *str)
{
	const EcsArray *a = ecs_get(world, op->type, EcsArray);
	ecs_assert(a != NULL, ECS_INTERNAL_ERROR, NULL);

	return expr_ser_type_elements(
	world, a->type, ptr, a->count, str, true);
}

/* Serialize vector */
static int expr_ser_vector(
const ecs_world_t *world,
ecs_meta_type_op_t *op,
const void *base,
ecs_strbuf_t *str)
{
	const ecs_vec_t *value = base;
	const EcsVector *v = ecs_get(world, op->type, EcsVector);
	ecs_assert(v != NULL, ECS_INTERNAL_ERROR, NULL);

	int32_t count = ecs_vec_count(value);
	void *array = ecs_vec_first(value);

	/* Serialize contiguous buffer of vector */
	return expr_ser_type_elements(world, v->type, array, count, str, false);
}

static int flecs_expr_ser_type_op(
const ecs_world_t *world,
ecs_meta_type_op_t *op,
const void *ptr,
ecs_strbuf_t *str,
bool is_expr)
{
	ecs_assert(ptr != NULL, ECS_INVALID_PARAMETER, NULL);

	switch (op->kind) {
	case EcsOpPush:
	case EcsOpPop:
		/* Should not be parsed as single op */
		ecs_throw(ECS_INVALID_PARAMETER, NULL);
		break;
	case EcsOpEnum:
		if (flecs_expr_ser_enum(world, op, ECS_OFFSET(ptr, op->offset), str)) {
			goto error;
		}
		break;
	case EcsOpBitmask:
		if (flecs_expr_ser_bitmask(world, op, ECS_OFFSET(ptr, op->offset), str)) {
			goto error;
		}
		break;
	case EcsOpArray:
		if (expr_ser_array(world, op, ECS_OFFSET(ptr, op->offset), str)) {
			goto error;
		}
		break;
	case EcsOpVector:
		if (expr_ser_vector(world, op, ECS_OFFSET(ptr, op->offset), str)) {
			goto error;
		}
		break;
	case EcsOpScope:
	case EcsOpPrimitive:
	case EcsOpBool:
	case EcsOpChar:
	case EcsOpByte:
	case EcsOpU8:
	case EcsOpU16:
	case EcsOpU32:
	case EcsOpU64:
	case EcsOpI8:
	case EcsOpI16:
	case EcsOpI32:
	case EcsOpI64:
	case EcsOpF32:
	case EcsOpF64:
	case EcsOpUPtr:
	case EcsOpIPtr:
	case EcsOpEntity:
	case EcsOpId:
	case EcsOpString:
	case EcsOpOpaque:
		if (ecs0_flecs_expr_ser_primitive(world, flecs_expr_op_to_primitive_kind(op->kind),
		    ECS_OFFSET(ptr, op->offset), str, is_expr)) {
			/* Unknown operation */
			ecs_err("unknown serializer operation kind (%d)", op->kind);
			goto error;
		}
		break;
	default:
		ecs_throw(ECS_INVALID_PARAMETER, "invalid operation");
	}

	return 0;
error:
	return -1;
}

/* Iterate over a slice of the type ops array */
static int flecs_expr_ser_type_ops(
const ecs_world_t *world,
ecs_meta_type_op_t *ops,
int32_t op_count,
const void *base,
ecs_strbuf_t *str,
int32_t in_array,
bool is_expr)
{
	for (int i = 0; i < op_count; i++) {
		ecs_meta_type_op_t *op = &ops[i];

		if (in_array <= 0) {
			if (op->name) {
				ecs_strbuf_list_next(str);
				ecs_strbuf_append(str, "%s: ", op->name);
			}

			int32_t elem_count = op->count;
			if (elem_count > 1) {
				/* Serialize inline array */
				if (expr_ser_elements(world, op, op->op_count, base,
				    elem_count, op->size, str, true)) {
					return -1;
				}

				i += op->op_count - 1;
				continue;
			}
		}

		switch (op->kind) {
		case EcsOpPush:
			ecs_strbuf_list_push(str, "{", ", ");
			in_array--;
			break;
		case EcsOpPop:
			ecs_strbuf_list_pop(str, "}");
			in_array++;
			break;
		case EcsOpArray:
		case EcsOpVector:
		case EcsOpEnum:
		case EcsOpBitmask:
		case EcsOpScope:
		case EcsOpPrimitive:
		case EcsOpBool:
		case EcsOpChar:
		case EcsOpByte:
		case EcsOpU8:
		case EcsOpU16:
		case EcsOpU32:
		case EcsOpU64:
		case EcsOpI8:
		case EcsOpI16:
		case EcsOpI32:
		case EcsOpI64:
		case EcsOpF32:
		case EcsOpF64:
		case EcsOpUPtr:
		case EcsOpIPtr:
		case EcsOpEntity:
		case EcsOpId:
		case EcsOpString:
		case EcsOpOpaque:
			if (flecs_expr_ser_type_op(world, op, base, str, is_expr)) {
				goto error;
			}
			break;
		default:
			ecs_throw(ECS_INVALID_PARAMETER, "invalid operation");
		}
	}

	return 0;
error:
	return -1;
}

/* Iterate over the type ops of a type */
static int flecs_expr_ser_type(
const ecs_world_t *world,
const ecs_vec_t *v_ops,
const void *base,
ecs_strbuf_t *str,
bool is_expr)
{
	ecs_meta_type_op_t *ops = ecs_vec_first_t(v_ops, ecs_meta_type_op_t);
	int32_t count = ecs_vec_count(v_ops);
	return flecs_expr_ser_type_ops(world, ops, count, base, str, 0, is_expr);
}

int ecs_ptr_to_expr_buf(
const ecs_world_t *world,
ecs_entity_t type,
const void *ptr,
ecs_strbuf_t *buf_out)
{
	const EcsTypeSerializer *ser = ecs_get(
	world, type, EcsTypeSerializer);
	if (ser == NULL) {
		char *path = ecs_get_path(world, type);
		ecs_err("cannot serialize value for type '%s'", path);
		ecs_os_free(path);
		goto error;
	}

	if (flecs_expr_ser_type(world, &ser->ops, ptr, buf_out, true)) {
		goto error;
	}

	return 0;
error:
	return -1;
}

int ecs0_expr_ser_bitmask(const ecs_world_t *world, ecs_entity_t type, const void *ptr, ecs_strbuf_t *str)
{
	const EcsBitmask *bitmask_type = ecs_get(world, type, EcsBitmask);
	ecs_check(bitmask_type != NULL, ECS_INVALID_PARAMETER, NULL);
	uint32_t value = *(const uint32_t *)ptr;

	ecs_strbuf_list_push(str, "", "|");

	/* Multiple flags can be set at a given time. Iterate through all the flags
	 * and append the ones that are set. */
	ecs_map_iter_t it = ecs_map_iter(&bitmask_type->constants);
	int count = 0;
	while (ecs_map_next(&it)) {
		ecs_bitmask_constant_t *c = ecs_map_ptr(&it);
		ecs_map_key_t key = ecs_map_key(&it);
		if ((value & key) == key) {
			ecs_strbuf_list_appendstr(str, ecs_get_name(world, c->constant));
			count++;
			value -= (uint32_t)key;
		}
	}

	if (value != 0) {
		/* All bits must have been matched by a constant */
		char *path = ecs_get_path(world, type);
		ecs_err(
		"value for bitmask %s contains bits (%u) that cannot be mapped to constant",
		path, value);
		ecs_os_free(path);
		goto error;
	}

	if (!count) {
		ecs_strbuf_list_appendstr(str, "0");
	}

	ecs_strbuf_list_pop(str, "");

	return 0;
error:
	return -1;
}
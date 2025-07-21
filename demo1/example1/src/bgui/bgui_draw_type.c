#include "bgui_draw_type.h"
#include "Ec.h"
#include <stdio.h>
#include "jmgui.h"

#define flecs_ito(T, value) (T)(value)
#define flecs_uto(T, value) (T)(value)

static int fgui_json_ser_type_ops(const ecs_world_t *world, ecs_meta_type_op_t *ops, int32_t op_count, const void *base, ecs_strbuf_t *str, int32_t in_array);

ecs_primitive_kind_t flecs_json_op_to_primitive_kind(ecs_meta_type_op_kind_t kind)
{
	return kind - EcsOpPrimitive;
}

void flecs_json_membern(ecs_strbuf_t *buf, const char *name, int32_t name_len)
{
	ecs_strbuf_list_appendch(buf, '"');
	ecs_strbuf_appendstrn(buf, name, name_len);
	ecs_strbuf_appendlit(buf, "\":");
}

void flecs_json_member(ecs_strbuf_t *buf, const char *name)
{
	flecs_json_membern(buf, name, ecs_os_strlen(name));
}

void flecs_json_object_push(ecs_strbuf_t *buf)
{
	ecs_strbuf_list_push(buf, "{", ", ");
}

void flecs_json_object_pop(ecs_strbuf_t *buf)
{
	ecs_strbuf_list_pop(buf, "}");
}

void flecs_json_array_push(ecs_strbuf_t *buf)
{
	ecs_strbuf_list_push(buf, "[", ", ");
}

void flecs_json_array_pop(ecs_strbuf_t *buf)
{
	ecs_strbuf_list_pop(buf, "]");
}

void flecs_json_id(ecs_strbuf_t *buf, const ecs_world_t *world, ecs_id_t id)
{
	ecs_strbuf_appendch(buf, '[');

	if (ECS_IS_PAIR(id)) {
		ecs_entity_t first = ecs_pair_first(world, id);
		ecs_entity_t second = ecs_pair_second(world, id);
		ecs_strbuf_appendch(buf, '"');
		ecs_get_path_w_sep_buf(world, 0, first, ".", "", buf, true);
		ecs_strbuf_appendch(buf, '"');
		ecs_strbuf_appendch(buf, ',');
		ecs_strbuf_appendch(buf, '"');
		ecs_get_path_w_sep_buf(world, 0, second, ".", "", buf, true);
		ecs_strbuf_appendch(buf, '"');
	} else {
		ecs_strbuf_appendch(buf, '"');
		ecs_get_path_w_sep_buf(world, 0, id & ECS_COMPONENT_MASK, ".", "", buf, true);
		ecs_strbuf_appendch(buf, '"');
	}

	ecs_strbuf_appendch(buf, ']');
}

void flecs_json_path(ecs_strbuf_t *buf, const ecs_world_t *world, ecs_entity_t e)
{
	ecs_strbuf_appendch(buf, '"');
	ecs_get_path_w_sep_buf(world, 0, e, ".", "", buf, true);
	ecs_strbuf_appendch(buf, '"');
}

/* Serialize a primitive value */
int flecs_expr_ser_primitive(const ecs_world_t *world, ecs_primitive_kind_t kind, const void *base, ecs_strbuf_t *str, bool is_expr)
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
		char chbuf[3];
		char ch = *(const char *)base;
		if (ch) {
			flecs_chresc(chbuf, *(const char *)base, '"');
			if (is_expr)
				ecs_strbuf_appendch(str, '"');
			ecs_strbuf_appendstr(str, chbuf);
			if (is_expr)
				ecs_strbuf_appendch(str, '"');
		} else {
			ecs_strbuf_appendch(str, '0');
		}
		break;
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

typedef struct json_serializer_ctx_t {
	ecs_strbuf_t *str;
	bool is_collection;
	bool is_struct;
} json_serializer_ctx_t;

static int json_ser_custom_value(const ecs_serializer_t *ser, ecs_entity_t type, const void *value)
{
	json_serializer_ctx_t *json_ser = ser->ctx;
	if (json_ser->is_collection) {
		ecs_strbuf_list_next(json_ser->str);
	}
	return ecs_ptr_to_json_buf(ser->world, type, value, json_ser->str);
}

static int json_ser_custom_member(const ecs_serializer_t *ser, const char *name)
{
	json_serializer_ctx_t *json_ser = ser->ctx;
	if (!json_ser->is_struct) {
		ecs_err("serializer::member can only be called for structs");
		return -1;
	}
	flecs_json_member(json_ser->str, name);
	return 0;
}

static int json_ser_custom_type(const ecs_world_t *world, ecs_meta_type_op_t *op, const void *base, ecs_strbuf_t *str)
{
	const EcsOpaque *ct = ecs_get(world, op->type, EcsOpaque);
	ecs_assert(ct != NULL, ECS_INVALID_OPERATION,
	"entity %s in opaque type serializer instruction is not an opaque type",
	ecs_get_name(world, op->type));
	ecs_assert(ct->as_type != 0, ECS_INVALID_OPERATION,
	"opaque type %s has not populated as_type field",
	ecs_get_name(world, op->type));
	ecs_assert(ct->serialize != NULL, ECS_INVALID_OPERATION,
	"opaque type %s does not have serialize interface",
	ecs_get_name(world, op->type));

	const EcsType *pt = ecs_get(world, ct->as_type, EcsType);
	ecs_assert(pt != NULL, ECS_INVALID_OPERATION,
	"opaque type %s is missing flecs.meta.Type component",
	ecs_get_name(world, op->type));

	ecs_type_kind_t kind = pt->kind;
	bool is_collection = false;
	bool is_struct = false;

	if (kind == EcsStructType) {
		flecs_json_object_push(str);
		is_struct = true;
	} else if (kind == EcsArrayType || kind == EcsVectorType) {
		flecs_json_array_push(str);
		is_collection = true;
	}

	json_serializer_ctx_t json_ser = {
	.str = str,
	.is_struct = is_struct,
	.is_collection = is_collection};

	ecs_serializer_t ser = {
	.world = world,
	.value = json_ser_custom_value,
	.member = json_ser_custom_member,
	.ctx = &json_ser};

	if (ct->serialize(&ser, base)) {
		return -1;
	}

	if (kind == EcsStructType) {
		flecs_json_object_pop(str);
	} else if (kind == EcsArrayType || kind == EcsVectorType) {
		flecs_json_array_pop(str);
	}

	return 0;
}

/* Serialize enumeration */
static int flecs_json_ser_enum(
const ecs_world_t *world,
ecs_meta_type_op_t *op,
const void *base,
ecs_strbuf_t *str)
{
	const EcsEnum *enum_type = ecs_get(world, op->type, EcsEnum);
	ecs_check(enum_type != NULL, ECS_INVALID_PARAMETER, NULL);

	int32_t value = *(const int32_t *)base;

	/* Enumeration constants are stored in a map that is keyed on the
	 * enumeration value. */
	ecs_enum_constant_t *constant = ecs_map_get_deref(&enum_type->constants,
	ecs_enum_constant_t, (ecs_map_key_t)value);
	if (!constant) {
		/* If the value is not found, it is not a valid enumeration constant */
		char *name = ecs_get_path(world, op->type);
		ecs_err("enumeration value '%d' of type '%s' is not a valid constant",
		value, name);
		ecs_os_free(name);
		goto error;
	}

	ecs_strbuf_appendch(str, '"');
	ecs_strbuf_appendstr(str, ecs_get_name(world, constant->constant));
	ecs_strbuf_appendch(str, '"');

	return 0;
error:
	return -1;
}

/* Serialize bitmask */
static int flecs_json_ser_bitmask(
const ecs_world_t *world,
ecs_meta_type_op_t *op,
const void *ptr,
ecs_strbuf_t *str)
{
	const EcsBitmask *bitmask_type = ecs_get(world, op->type, EcsBitmask);
	ecs_check(bitmask_type != NULL, ECS_INVALID_PARAMETER, NULL);

	uint32_t value = *(const uint32_t *)ptr;
	if (!value) {
		ecs_strbuf_appendch(str, '0');
		return 0;
	}

	ecs_strbuf_list_push(str, "\"", "|");

	/* Multiple flags can be set at a given time. Iterate through all the flags
	 * and append the ones that are set. */
	ecs_map_iter_t it = ecs_map_iter(&bitmask_type->constants);
	while (ecs_map_next(&it)) {
		ecs_bitmask_constant_t *constant = ecs_map_ptr(&it);
		ecs_map_key_t key = ecs_map_key(&it);
		if ((value & key) == key) {
			ecs_strbuf_list_appendstr(str,
			ecs_get_name(world, constant->constant));
			value -= (uint32_t)key;
		}
	}

	if (value != 0) {
		/* All bits must have been matched by a constant */
		char *name = ecs_get_path(world, op->type);
		ecs_err("bitmask value '%u' of type '%s' contains invalid/unknown bits",
		value, name);
		ecs_os_free(name);
		goto error;
	}

	ecs_strbuf_list_pop(str, "\"");

	return 0;
error:
	return -1;
}

/* Serialize elements of a contiguous array */
static int flecs_json_ser_elements(
const ecs_world_t *world,
ecs_meta_type_op_t *ops,
int32_t op_count,
const void *base,
int32_t elem_count,
int32_t elem_size,
ecs_strbuf_t *str,
bool is_array)
{
	flecs_json_array_push(str);

	const void *ptr = base;

	int i;
	for (i = 0; i < elem_count; i++) {
		ecs_strbuf_list_next(str);
		if (fgui_json_ser_type_ops(world, ops, op_count, ptr, str, is_array)) {
			return -1;
		}
		ptr = ECS_OFFSET(ptr, elem_size);
	}

	flecs_json_array_pop(str);

	return 0;
}

static int flecs_json_ser_type_elements(
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

	return flecs_json_ser_elements(
	world, ops, op_count, base, elem_count, comp->size, str, is_array);
}

/* Serialize array */
static int json_ser_array(
const ecs_world_t *world,
ecs_meta_type_op_t *op,
const void *ptr,
ecs_strbuf_t *str)
{
	const EcsArray *a = ecs_get(world, op->type, EcsArray);
	ecs_assert(a != NULL, ECS_INTERNAL_ERROR, NULL);

	return flecs_json_ser_type_elements(
	world, a->type, ptr, a->count, str, true);
}

/* Serialize vector */
static int json_ser_vector(
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
	return flecs_json_ser_type_elements(world, v->type, array, count, str, false);
}

/* Forward serialization to the different type kinds */
static int fgui_json_ser_type_op(
const ecs_world_t *world,
ecs_meta_type_op_t *op,
const void *ptr,
ecs_strbuf_t *str)
{
	void *vptr = ECS_OFFSET(ptr, op->offset);
	bool large_int = false;
	if (op->kind == EcsOpI64) {
		if (*(int64_t *)vptr >= 2147483648) {
			large_int = true;
		}
	} else if (op->kind == EcsOpU64) {
		if (*(uint64_t *)vptr >= 2147483648) {
			large_int = true;
		}
	}

	if (large_int) {
		ecs_strbuf_appendch(str, '"');
	}

	switch (op->kind) {
	case EcsOpPush:
	case EcsOpPop:
		/* Should not be parsed as single op */
		ecs_throw(ECS_INVALID_PARAMETER, NULL);
		break;
	case EcsOpF32:
		ecs_strbuf_appendflt(str,
		(ecs_f64_t) * (const ecs_f32_t *)vptr, '"');
		break;
	case EcsOpF64:
		ecs_strbuf_appendflt(str,
		*(ecs_f64_t *)vptr, '"');
		break;
	case EcsOpEnum:
		if (flecs_json_ser_enum(world, op, vptr, str)) {
			goto error;
		}
		break;
	case EcsOpBitmask:
		if (flecs_json_ser_bitmask(world, op, vptr, str)) {
			goto error;
		}
		break;
	case EcsOpArray:
		if (json_ser_array(world, op, vptr, str)) {
			goto error;
		}
		break;
	case EcsOpVector:
		if (json_ser_vector(world, op, vptr, str)) {
			goto error;
		}
		break;
	case EcsOpOpaque:
		if (json_ser_custom_type(world, op, vptr, str)) {
			goto error;
		}
		break;
	case EcsOpEntity: {
		ecs_entity_t e = *(const ecs_entity_t *)vptr;
		if (!e) {
			ecs_strbuf_appendlit(str, "\"#0\"");
		} else {
			flecs_json_path(str, world, e);
		}
		break;
	}
	case EcsOpId: {
		ecs_id_t id = *(const ecs_id_t *)vptr;
		if (!id) {
			ecs_strbuf_appendlit(str, "\"#0\"");
		} else {
			flecs_json_id(str, world, id);
		}
		break;
	}

	case EcsOpU64:
	case EcsOpI64:
	case EcsOpBool:
	case EcsOpChar:
	case EcsOpByte:
	case EcsOpU8:
	case EcsOpU16:
	case EcsOpU32:
	case EcsOpI8:
	case EcsOpI16:
	case EcsOpI32:
	case EcsOpUPtr:
	case EcsOpIPtr:
	case EcsOpString:
		if (flecs_expr_ser_primitive(world,
		    flecs_json_op_to_primitive_kind(op->kind),
		    ECS_OFFSET(ptr, op->offset), str, true)) {
			ecs_throw(ECS_INTERNAL_ERROR, NULL);
		}
		break;

	case EcsOpPrimitive:
	case EcsOpScope:
	default:
		ecs_throw(ECS_INTERNAL_ERROR, NULL);
	}

	if (large_int) {
		ecs_strbuf_appendch(str, '"');
	}

	return 0;
error:
	return -1;
}

/* Iterate over a slice of the type ops array */
static int fgui_json_ser_type_ops(const ecs_world_t *world, ecs_meta_type_op_t *ops, int32_t op_count, const void *base, ecs_strbuf_t *str, int32_t in_array)
{
	for (int i = 0; i < op_count; i++) {
		ecs_meta_type_op_t *op = &ops[i];

		if (in_array <= 0) {
			if (op->name) {
				flecs_json_member(str, op->name);
			}

			int32_t elem_count = op->count;
			if (elem_count > 1) {
				/* Serialize inline array */
				if (flecs_json_ser_elements(world, op, op->op_count, base,
				    elem_count, op->size, str, true)) {
					return -1;
				}

				i += op->op_count - 1;
				continue;
			}
		}

		switch (op->kind) {
		case EcsOpPush:
			flecs_json_object_push(str);
			in_array--;
			break;
		case EcsOpPop:
			flecs_json_object_pop(str);
			in_array++;
			break;
		case EcsOpArray:
		case EcsOpVector:
		case EcsOpScope:
		case EcsOpEnum:
		case EcsOpBitmask:
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
			if (fgui_json_ser_type_op(world, op, base, str)) {
				goto error;
			}
			break;
		default:
			ecs_throw(ECS_INTERNAL_ERROR, NULL);
		}
	}

	return 0;
error:
	return -1;
}

/* Iterate over the type ops of a type */
int fgui_json_ser_type(
const ecs_world_t *world,
const ecs_vec_t *v_ops,
const void *base,
ecs_strbuf_t *str)
{
	ecs_meta_type_op_t *ops = ecs_vec_first_t(v_ops, ecs_meta_type_op_t);
	int32_t count = ecs_vec_count(v_ops);
	return fgui_json_ser_type_ops(world, ops, count, base, str, 0);
}

static int fgui_array_to_json_buf_w_type_data(
const ecs_world_t *world,
const void *ptr,
int32_t count,
ecs_strbuf_t *buf,
const EcsComponent *comp,
const EcsTypeSerializer *ser)
{
	if (count) {
		ecs_size_t size = comp->size;

		flecs_json_array_push(buf);

		do {
			ecs_strbuf_list_next(buf);
			if (fgui_json_ser_type(world, &ser->ops, ptr, buf)) {
				return -1;
			}

			ptr = ECS_OFFSET(ptr, size);
		} while (--count);

		flecs_json_array_pop(buf);
	} else {
		if (fgui_json_ser_type(world, &ser->ops, ptr, buf)) {
			return -1;
		}
	}

	return 0;
}

int fgui_array_to_json_buf(
const ecs_world_t *world,
ecs_entity_t type,
const void *ptr,
int32_t count,
ecs_strbuf_t *buf)
{
	const EcsComponent *comp = ecs_get(world, type, EcsComponent);
	if (!comp) {
		char *path = ecs_get_path(world, type);
		ecs_err("cannot serialize to JSON, '%s' is not a component", path);
		ecs_os_free(path);
		return -1;
	}

	const EcsTypeSerializer *ser = ecs_get(
	world, type, EcsTypeSerializer);
	if (!ser) {
		char *path = ecs_get_path(world, type);
		ecs_err("cannot serialize to JSON, '%s' has no reflection data", path);
		ecs_os_free(path);
		return -1;
	}

	return fgui_array_to_json_buf_w_type_data(world, ptr, count, buf, comp, ser);
}

bool jmgui_draw_type(ecs_world_t *world, ecs_entity_t type, void const *ptr)
{
	bool clicked = false;
	if (type == ecs_id(EcInteger)) {
		EcInteger const *q = (EcInteger *)ptr;
		EcsUnit const *u = ecs_get(world, q->unit, EcsUnit);
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
		EcsIdentifier const *q = (EcsIdentifier *)ptr;
		// jmgui_text_colored(1.0, 1.0, 0.7, "%s", q->value);
		clicked = jmgui_text_link(q->value);
	} else {
		char *json = ecs_ptr_to_json(world, type, ptr);
		if (json) {
			jmgui_text(json);
			ecs_os_free(json);
		}
	}
	return clicked;
}
#include "Gui.h"
#include <ecsx.h>

ECS_COMPONENT_DECLARE(GuiWindow);
ECS_COMPONENT_DECLARE(GuiType);
ECS_COMPONENT_DECLARE(GuiElement);
ECS_COMPONENT_DECLARE(GuiColor3);
ECS_COMPONENT_DECLARE(GuiQuery);
ECS_COMPONENT_DECLARE(GuiString);
ECS_COMPONENT_DECLARE(GuiTable);
ECS_COMPONENT_DECLARE(GuiQueryColumn);
ECS_TAG_DECLARE(GuiDebugIdUnit);

static void test_query(ecs_world_t *world, ecs_query_t *q, ecs_entity_t parent)
{
	ecs_log_push_(0);
	ecs_iter_t it = ecs_query_iter(world, q);
	ecs_iter_set_group(&it, parent);
	while (ecs_query_next(&it)) {
		for (int i = 0; i < it.count; i++) {
			ecs_entity_t e = it.entities[i];
			ecs_trace("name: %s", ecs_get_name(world, e));
		}
	}
	ecs_log_pop_(0);
}



static void SystemCreateGuiQuery(ecs_iter_t *it)
{
	ecsx_trace_system_iter(it);
	ecs_log_push_(0);
	EcsDocDescription *d = ecs_field(it, EcsDocDescription, 0);
	for (int i = 0; i < it->count; ++i) {
		ecs_entity_t e = it->entities[i];
		if (d[i].value == NULL) {
			continue;
		}
		char const *name = ecs_get_name(it->world, e);
		ecs_trace("Entity: '%s'", name);
		ecs_log_push_(0);
		ecs_trace("ecs_query_init: '%s'", d[i].value);
		ecs_query_t *q = ecs_query_init(it->real_world,
		&(ecs_query_desc_t){
		//.cache_kind = EcsQueryCacheAll,
		//.entity = ecs_entity_init(it->real_world, &(ecs_entity_desc_t){.name = "GuiQuery"}),
		.entity = e,
		.expr = d[i].value,
		.group_by = EcsChildOf});
		if (q == NULL) {
			ecs_err("Failed to create query");
			ecs_enable(it->world, e, false);
			continue;
		}
		test_query(it->world, q, e);
		ecs_set(it->world, e, GuiQuery, {.query = q});
		ecs_log_pop_(0);
	}
	ecs_log_pop_(0);
}



// The constructor should initialize the component value.
ECS_CTOR(GuiString, ptr, {
	// ecs_trace("Ctor");
	ptr->value = NULL;
})

// The destructor should free resources.
ECS_DTOR(GuiString, ptr, {
	// ecs_trace("Dtor");
	ecs_os_free(ptr->value);
})

// The move hook should move resources from one location to another.
ECS_MOVE(GuiString, dst, src, {
	ecs_os_free(dst->value);
	dst->value = src->value;
	src->value = NULL;
})

// The copy hook should copy resources from one location to another.
ECS_COPY(GuiString, dst, src, {
	// ecs_trace("Copy");
	ecs_os_free(dst->value);
	dst->value = ecs_os_strdup(src->value);
})

static ECS_CTOR(GuiTable, ptr, {
    ecs_os_zeromem(ptr);
    ecs_vec_init_t(NULL, &ptr->columns, GuiQueryColumn, 0);
})

static
ECS_DTOR(GuiTable, ptr, {
    ecs_vec_fini_t(NULL, &ptr->columns, GuiQueryColumn);
})

static
ECS_MOVE(GuiTable, dst, src, {
    ecs_vec_fini_t(NULL, &dst->columns, GuiQueryColumn);
    dst->columns = src->columns;
    src->columns = (ecs_vec_t){0};
})

void GuiImport(ecs_world_t *world)
{
	ECS_MODULE(world, Gui);
	ecs_set_name_prefix(world, "Gui");
	ECS_COMPONENT_DEFINE(world, GuiString);
	ECS_COMPONENT_DEFINE(world, GuiWindow);
	ECS_COMPONENT_DEFINE(world, GuiType);
	ECS_COMPONENT_DEFINE(world, GuiElement);
	ECS_COMPONENT_DEFINE(world, GuiColor3);
	ECS_COMPONENT_DEFINE(world, GuiQuery);
	ECS_COMPONENT_DEFINE(world, GuiTable);
	ECS_COMPONENT_DEFINE(world, GuiQueryColumn);

	// ecs_add_id(world, ecs_id(GuiElement), EcsTraversable);
	// ecs_add_id(world, ecs_id(GuiElement), EcsInheritable);

    ecs_set_hooks(world, GuiTable, {
        .ctor = ecs_ctor(GuiTable),
        .dtor = ecs_dtor(GuiTable),
        .move = ecs_move(GuiTable)
    });

	ecs_set_hooks(world, GuiString,
	{
	/* Resource management hooks. These hooks should primarily be used for
	 * managing memory used by the component. */
	.ctor = ecs_ctor(GuiString),
	.move = ecs_move(GuiString),
	.copy = ecs_copy(GuiString),
	.dtor = ecs_dtor(GuiString),
	});

	GuiDebugIdUnit = ecs_unit_init(world,
	&(ecs_unit_desc_t){
	.entity = ecs_entity(world, {.name = "DebugId"}),
	.quantity = EcsData,
	.symbol = "did"});



	// ecs_entity_t component_kind = ecs_lookup(world, "flecs.meta.TypeKind");
	// ecs_entity_t component_primitive = ecs_lookup(world, "flecs.meta.PrimitiveKind");
	ecs_struct(world,
	{.entity = ecs_id(GuiQueryColumn),
	.members = {
	{.name = "field", .type = ecs_id(ecs_u32_t)},
	{.name = "type", .type = ecs_id(ecs_entity_t)},
	{.name = "offset", .type = ecs_id(ecs_u32_t)},
	}});


    ecs_entity_t e_GuiQueryColumnVector = ecs_vector_init(world, &(ecs_vector_desc_t){
        .entity = ecs_entity(world, {.name = "GuiQueryColumnVector"}),
        .type = ecs_id(GuiQueryColumn)
    });

	ecs_struct(world,
	{.entity = ecs_id(GuiTable),
	.members = {
	{.name = "table_dummy", .type = ecs_id(ecs_i32_t)},
	{.name = "columns", .type = e_GuiQueryColumnVector},
	}});

	ecs_enum(world,
	{.entity = ecs_id(GuiType),
	.constants = {
	{.name = "Unknown", .value = GuiTypeUnknown},
	{.name = "Window", .value = GuiTypeWindow},
	{.name = "Tabs", .value = GuiTypeTabs},
	{.name = "Tab", .value = GuiTypeTab},
	{.name = "TreeNode", .value = GuiTypeTreeNode},
	{.name = "Text", .value = GuiTypeText},
	{.name = "NodeTreeReflection", .value = GuiTypeNodeTreeReflection},
	{.name = "ColumnComponent", .value = GuiTypeColumnComponent},
	{.name = "TypeEntityInfo", .value = GuiTypeEntityInfo},
	{.name = "InputText", .value = GuiTypeInputText}
}});

	ecs_struct(world,
	{.entity = ecs_id(GuiString),
	.members = {
	{.name = "value", .type = ecs_id(ecs_string_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(GuiQuery),
	.members = {
	{.name = "query", .type = ecs_id(ecs_uptr_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(GuiElement),
	.members = {
	{.name = "type", .type = ecs_id(GuiType)},
	{.name = "storage", .type = ecs_id(ecs_entity_t)},
	{.name = "members", .type = ecs_id(ecs_entity_t), .count = 8},
	{.name = "id", .type = ecs_id(ecs_u32_t), .unit = GuiDebugIdUnit},
	}});

	ecs_struct(world,
	{.entity = ecs_id(GuiWindow),
	.members = {
	{.name = "dummy", .type = ecs_id(ecs_i32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(GuiColor3),
	.members = {
	{.name = "r", .type = ecs_id(ecs_f32_t)},
	{.name = "g", .type = ecs_id(ecs_f32_t)},
	{.name = "b", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "CreateGuiQuery", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = SystemCreateGuiQuery,
	.immediate = true,
	.query.terms = {
	{.id = ecs_pair(ecs_id(EcsDocDescription), ecs_id(GuiQuery))},
	{.id = ecs_id(GuiQuery), .oper = EcsNot},
	}});
}

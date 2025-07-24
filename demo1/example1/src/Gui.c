#include "Gui.h"
#include <ecsx.h>
#include <stdio.h>

ECS_COMPONENT_DECLARE(GuiRoot);
ECS_COMPONENT_DECLARE(GuiWindow);
ECS_COMPONENT_DECLARE(GuiType);
ECS_COMPONENT_DECLARE(GuiElement);
ECS_COMPONENT_DECLARE(GuiColor3);
ECS_COMPONENT_DECLARE(GuiObserverDesc);
ECS_COMPONENT_DECLARE(GuiString);
ECS_COMPONENT_DECLARE(GuiTable);
ECS_COMPONENT_DECLARE(GuiQueryColumn);
ECS_COMPONENT_DECLARE(GuiEventClick);
ECS_TAG_DECLARE(GuiDebugIdUnit);
ECS_TAG_DECLARE(GuiClicked);

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
	ecs_log_set_level(0);
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
		ecs_log_pop_(0);
	}
	ecs_log_pop_(0);
}



static void OnResize(ecs_iter_t *it)
{
	ecs_world_t *world = it->world;
	// Event payload can be obtained from the it->param member
	GuiEventClick *p = it->param;
	printf("%08jX %08jX\n", p->egui, p->subject);

	for (int i = 0; i < it->count; i++) {
		ecs_entity_t e = it->entities[i];
		printf(" - %s: %s: %s. ", ecs_get_name(world, it->event), ecs_get_name(world, it->event_id), ecs_get_name(world, e));
		const ecs_type_t *type = ecs_get_type(world, e);
		char *type_str = ecs_type_str(world, type);
		printf("t: %s\n", type_str);
		ecs_os_free(type_str);
	}
}

static void SystemCreateGuiObserver(ecs_iter_t *it)
{
	ecs_world_t *world = it->world;
	ecsx_trace_system_iter(it);
	ecs_log_set_level(0);
	ecs_log_push_(0);
	EcsDocDescription *d = ecs_field(it, EcsDocDescription, 0);
	GuiObserverDesc *o = ecs_field(it, GuiObserverDesc, 1);
	for (int i = 0; i < it->count; ++i) {
		ecs_entity_t e = it->entities[i];
		char const *name = ecs_get_name(it->world, e);
		ecs_entity_t const * ev = o[i].events;
		char const * exp = d[i].value;
		if (ecs_has_id(it->world, e, EcsObserver)) {
			ecs_warn("Entity: '%s' already has an observer", name);
			continue;
		}
		if (exp == NULL) {
			ecs_warn("Entity: '%s' has no expression", name);
			continue;
		}
		ecs_trace("Entity: '%s'", name);
		ecs_log_push_(0);
		ecs_trace("ecs_observer_init: '%s'", exp);
		ecs_observer_init(it->real_world,
		&(ecs_observer_desc_t){
		.entity = e,
		.query.expr = exp,
		.events = {ev[0],ev[1],ev[2],ev[3],ev[4],ev[5],ev[6],ev[7]},
		.callback = OnResize});
		ecs_add_id(world, e, EcsObserver);
		//ecs_set(it->world, e, GuiQuery, {.query = q});
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

void GuiImport(ecs_world_t *world)
{
	ECS_MODULE(world, Gui);
	ecs_set_name_prefix(world, "Gui");
	ECS_COMPONENT_DEFINE(world, GuiRoot);
	ECS_COMPONENT_DEFINE(world, GuiString);
	ECS_COMPONENT_DEFINE(world, GuiWindow);
	ECS_COMPONENT_DEFINE(world, GuiType);
	ECS_COMPONENT_DEFINE(world, GuiElement);
	ECS_COMPONENT_DEFINE(world, GuiColor3);
	ECS_COMPONENT_DEFINE(world, GuiTable);
	ECS_COMPONENT_DEFINE(world, GuiQueryColumn);
	ECS_COMPONENT_DEFINE(world, GuiEventClick);
	ECS_COMPONENT_DEFINE(world, GuiObserverDesc);
	ECS_TAG_DEFINE(world, GuiClicked);

	// ecs_add_id(world, ecs_id(GuiElement), EcsTraversable);
	// ecs_add_id(world, ecs_id(GuiElement), EcsInheritable);

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

	ecs_struct(world,
	{.entity = ecs_id(GuiObserverDesc),
	.members = {
	{.name = "events", .type = ecs_id(ecs_entity_t), .count = 8}
	}});

	ecs_struct(world,
	{.entity = ecs_id(GuiEventClick),
	.members = {
	{.name = "egui", .type = ecs_id(ecs_entity_t)},
	{.name = "subject", .type = ecs_id(ecs_entity_t)},
	}});

	ecs_observer_init(world,
	&(ecs_observer_desc_t){
	.query.expr = "gui.Element",
	.events = {ecs_id(GuiEventClick)},
	.callback = OnResize});

	// ecs_entity_t component_kind = ecs_lookup(world, "flecs.meta.TypeKind");
	// ecs_entity_t component_primitive = ecs_lookup(world, "flecs.meta.PrimitiveKind");
	ecs_struct(world,
	{.entity = ecs_id(GuiQueryColumn),
	.members = {
	{.name = "field", .type = ecs_id(ecs_u32_t)},
	{.name = "type", .type = ecs_id(ecs_entity_t)},
	{.name = "offset", .type = ecs_id(ecs_u32_t)},
	{.name = "on_click", .type = ecs_id(ecs_entity_t)},
	}});

	/*
	ecs_entity_t e_GuiQueryColumnVector = ecs_vector_init(world,
	&(ecs_vector_desc_t){
	.entity = ecs_entity(world, {.name = "GuiQueryColumnVector"}),
	.type = ecs_id(GuiQueryColumn),
	});
	*/

	ecs_struct(world,
	{.entity = ecs_id(GuiTable),
	.members = {
	{.name = "table_dummy", .type = ecs_id(ecs_i32_t)}}});

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
	{.name = "EntityInfo", .value = GuiTypeEntityInfo},
	{.name = "InputText", .value = GuiTypeInputText},
	}});

	ecs_struct(world,
	{.entity = ecs_id(GuiString),
	.members = {
	{.name = "value", .type = ecs_id(ecs_string_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(GuiElement),
	.members = {
	{.name = "type", .type = ecs_id(GuiType)},
	{.name = "storage", .type = ecs_id(ecs_entity_t)},
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
	{.entity = ecs_entity(world, {.name = "CreateGuiQuery", .add = ecs_ids(ecs_dependson(EcsOnStart))}),
	.callback = SystemCreateGuiQuery,
	.immediate = true,
	.query.terms = {
	{.id = ecs_pair(ecs_id(EcsDocDescription), EcsQuery)},
	{.id = EcsQuery, .oper = EcsNot},
	{.id = EcsPrefab, .oper = EcsOptional},
	}});

	
	ecs_system_init(world, &(ecs_system_desc_t)
	{.entity = ecs_entity(world, {.name = "SystemCreateGuiObserver", .add = ecs_ids(ecs_dependson(EcsOnStart))}),
	.callback = SystemCreateGuiObserver,
	.immediate = true,
	.query.terms = {
	{.id = ecs_pair(ecs_id(EcsDocDescription), ecs_id(GuiObserverDesc))},
	{.id = ecs_id(GuiObserverDesc)},
	{.id = EcsObserver, .oper = EcsNot},
	{.id = EcsPrefab, .oper = EcsOptional},
	}});
	
}

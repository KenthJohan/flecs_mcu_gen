#include "Gui.h"
#include <ecsx.h>
#include <stdio.h>
#include "bgui/bgui_ser.h"

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
ECS_COMPONENT_DECLARE(GuiDrawfx);
ECS_TAG_DECLARE(GuiDebugIdUnit);
ECS_TAG_DECLARE(GuiClicked);
ECS_TAG_DECLARE(GuiClicked);

static void test_query(ecs_world_t *world, ecs_query_t *q, ecs_entity_t parent)
{
	ecs_log_push_(0);
	ecs_iter_t it = ecs_query_iter(world, q);
	if (parent) {
		ecs_iter_set_group(&it, parent);
	}
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
	ecs_log_set_level(-1);
	ecsx_trace_system_iter(it);
	ecs_world_t *world = it->world;
	ecs_log_push_(0);
	EcsDocDescription *d = ecs_field(it, EcsDocDescription, 0);
	for (int i = 0; i < it->count; ++i) {
		ecs_entity_t e = it->entities[i];
		char *expr = d[i].value;
		if (expr == NULL) {
			continue;
		}
		ecsx_trace_ent(world, e, "");
		ecs_log_push_(0);
		ecs_trace("ecs_query_init expr: '%s'", expr);
		ecs_query_t *q = ecs_query_init(world,
		&(ecs_query_desc_t){
		.entity = e,
		.expr = expr,
		.group_by = EcsChildOf});
		if (q == NULL) {
			ecs_err("Failed to create query");
			ecs_enable(world, e, false);
			continue;
		}
		test_query(world, q, 0);
		ecs_log_pop_(0);
	}
	ecs_log_pop_(0);
	ecs_log_set_level(0);
}

static void OnResize(ecs_iter_t *it)
{
	ecs_world_t *world = it->world;
	ecs_log_set_level(-1);
	ecsx_trace_system_iter(it);
	ecs_log_push_(0);

	GuiObserverDesc const *a = ecs_get(world, it->system, GuiObserverDesc);
	if (a == NULL) {
		ecs_err("Missing GuiObserverDesc");
		ecs_log_pop_(0);
		return;
	}

	// Event payload can be obtained from the it->param member
	GuiEventClick *p = it->param;

	ecs_trace("GuiObserverDesc:");
	ecs_log_push_(0);
	ecsx_trace_path(world, p->subject, "subject ");
	ecsx_trace_path(world, p->egui, "egui ");
	ecs_log_pop_(0);

	if (a->spawn) {
		ecsx_trace_path(world, a->spawn, "spawn ");
		if (ecs_has_id(world, a->spawn, ecs_id(EcsComponent))) {
			ecs_entity_t e2[] = {p->egui, p->subject};
			void *data[] = {e2};
			ecs_bulk_init(world,
			&(ecs_bulk_desc_t){
			.count = 1,
			.ids = {a->spawn},
			.data = data});
		}
	}

	/*
	for (int i = 0; i < it->count; i++) {
	    ecs_entity_t e = it->entities[i];
	    ecs_trace("event='%s', event_id='%s' e='%s'", ecs_get_name(world, it->event), ecs_get_name(world, it->event_id), ecs_get_name(world, e));
	    const ecs_type_t *type = ecs_get_type(world, e);
	    char *type_str = ecs_type_str(world, type);
	    ecs_trace("t=%s\n", type_str);
	    ecs_os_free(type_str);
	}
	*/

	ecs_log_pop_(0);
	ecs_log_set_level(0);
}

static void SystemCreateGuiObserver(ecs_iter_t *it)
{
	ecs_log_set_level(-1);
	ecs_world_t *world = it->world;
	ecsx_trace_system_iter(it);
	ecs_log_push_(0);
	EcsDocDescription *d = ecs_field(it, EcsDocDescription, 0);
	GuiObserverDesc *o = ecs_field(it, GuiObserverDesc, 1);
	for (int i = 0; i < it->count; ++i) {
		ecs_entity_t e = it->entities[i];
		char const *name = ecs_get_name(world, e);
		ecs_entity_t const *ev = o[i].events;
		char const *exp = d[i].value;
		if (exp == NULL) {
			ecs_warn("Entity: '%s' has no expression", name);
			continue;
		}
		ecs_trace("Entity: '%s'", name);
		ecs_log_push_(0);
		ecs_trace("ecs_observer_init: '%s'", exp);
		ecs_observer_init(world,
		&(ecs_observer_desc_t){
		.entity = e,
		.query.expr = exp,
		.events = {ev[0], ev[1], ev[2], ev[3], ev[4], ev[5], ev[6], ev[7]},
		.callback = OnResize});
		ecs_log_pop_(0);
	}
	ecs_log_pop_(0);
	ecs_log_set_level(0);
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
	ECS_COMPONENT_DEFINE(world, GuiDrawfx);
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

	/*
	ecs_struct(world,
	{.entity = ecs_id(GuiDrawfx),
	.members = {
	{.name = "ptr", .type = ecs_id(ecs_uptr_t)}
	}});
	*/

	ecs_enum_init(world, &(ecs_enum_desc_t){
		.entity = ecs_id(GuiDrawfx),
		.underlying_type = ecs_id(ecs_u64_t),
		.constants = {
			{"Draw1", 1},
			{"query_str", 2},
		}
	});

	//bgui_ser_init(world);


	GuiDebugIdUnit = ecs_unit_init(world,
	&(ecs_unit_desc_t){
	.entity = ecs_entity(world, {.name = "DebugId"}),
	.quantity = EcsData,
	.symbol = "did"});

	ecs_struct(world,
	{.entity = ecs_id(GuiObserverDesc),
	.members = {
	{.name = "events", .type = ecs_id(ecs_entity_t), .count = 8},
	{.name = "spawn", .type = ecs_id(ecs_entity_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(GuiEventClick),
	.members = {
	{.name = "egui", .type = ecs_id(ecs_entity_t)},
	{.name = "subject", .type = ecs_id(ecs_entity_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(GuiQueryColumn),
	.members = {
	{.name = "field", .type = ecs_id(ecs_u32_t)},
	{.name = "type", .type = ecs_id(ecs_entity_t)},
	{.name = "offset", .type = ecs_id(ecs_u32_t)},
	{.name = "fx", .type = ecs_id(GuiDrawfx)},
	}});

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

	ecs_system_init(world,
	&(ecs_system_desc_t){
	.entity = ecs_entity(world, {.name = "CreateGuiQuery", .add = ecs_ids(ecs_dependson(EcsPostFrame))}),
	.callback = SystemCreateGuiQuery,
	.immediate = true,
	.query.terms = {
	{.id = ecs_pair(ecs_id(EcsDocDescription), EcsQuery)},
	{.id = ecs_pair(ecs_id(EcsPoly), EcsQuery), .oper = EcsNot},
	{.id = EcsPrefab, .oper = EcsOptional},
	}});

	ecs_system_init(world,
	&(ecs_system_desc_t){
	.entity = ecs_entity(world, {.name = "SystemCreateGuiObserver", .add = ecs_ids(ecs_dependson(EcsPostFrame))}),
	.callback = SystemCreateGuiObserver,
	.immediate = true,
	.query.terms = {
	{.id = ecs_pair(ecs_id(EcsDocDescription), ecs_id(GuiObserverDesc))},
	{.id = ecs_id(GuiObserverDesc)},
	{.id = ecs_pair(ecs_id(EcsPoly), EcsObserver), .oper = EcsNot},
	{.id = EcsPrefab, .oper = EcsOptional},
	}});
}

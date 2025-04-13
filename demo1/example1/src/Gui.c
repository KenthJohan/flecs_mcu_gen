#include "Gui.h"

ECS_COMPONENT_DECLARE(GuiWindow);
ECS_COMPONENT_DECLARE(GuiColumnComponent);
ECS_COMPONENT_DECLARE(GuiType);
ECS_COMPONENT_DECLARE(GuiElement);
ECS_COMPONENT_DECLARE(GuiColor3);
ECS_COMPONENT_DECLARE(GuiQuery);
ECS_TAG_DECLARE(GuiDebugIdUnit);

static void test_query(ecs_world_t * world, ecs_query_t * q, ecs_entity_t parent)
{
	ecs_iter_t it = ecs_query_iter(world, q);
	ecs_iter_set_group(&it, parent);
	while (ecs_query_next(&it)) {
		for (int i = 0; i < it.count; i++) {
			ecs_entity_t e = it.entities[i];
			ecs_log(-1, "name: %s", ecs_get_name(world, e));
		}
	}
}

static void SystemCreateGuiQuery(ecs_iter_t *it)
{
	EcsDocDescription *d = ecs_field(it, EcsDocDescription, 0);
	for (int i = 0; i < it->count; ++i) {
		ecs_entity_t e = it->entities[i];
		if (d[i].value == NULL) {
			continue;
		}
		ecs_log(-1, "Create query: %s", d[i].value);
		ecs_query_t * q = ecs_query_init(it->real_world, &(ecs_query_desc_t){
			.expr = d[i].value,
			.group_by = EcsChildOf
		});
		if (q == NULL) {
			ecs_err("Failed to create query");
			ecs_enable(it->world, e, false);
			continue;
		}
		test_query(it->world, q, e);
		ecs_set(it->world, e, GuiQuery, {.query = q});
	}
}

void GuiImport(ecs_world_t *world)
{
	ECS_MODULE(world, Gui);
	ecs_set_name_prefix(world, "Gui");
	ECS_COMPONENT_DEFINE(world, GuiWindow);
	ECS_COMPONENT_DEFINE(world, GuiColumnComponent);
	ECS_COMPONENT_DEFINE(world, GuiType);
	ECS_COMPONENT_DEFINE(world, GuiElement);
	ECS_COMPONENT_DEFINE(world, GuiColor3);
	ECS_COMPONENT_DEFINE(world, GuiQuery);
	// ecs_add_id(world, ecs_id(GuiElement), EcsTraversable);
	// ecs_add_id(world, ecs_id(GuiElement), EcsInheritable);

	GuiDebugIdUnit = ecs_unit_init(world,
	&(ecs_unit_desc_t){
	.entity = ecs_entity(world, {.name = "DebugId"}),
	.quantity = EcsData,
	.symbol = "did"});

	ecs_enum(world,
	{.entity = ecs_id(GuiType),
	.constants = {
	{.name = "Unknown", .value = GuiTypeUnknown},
	{.name = "Window", .value = GuiTypeWindow},
	{.name = "Tabs", .value = GuiTypeTabs},
	{.name = "Tab", .value = GuiTypeTab},
	{.name = "NodeTreeReflection", .value = GuiTypeNodeTreeReflection},
	{.name = "ColumnComponent", .value = GuiTypeColumnComponent},
	{.name = "InputText", .value = GuiTypeInputText}}});

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
	{.name = "id", .type = ecs_id(ecs_u32_t), .unit = GuiDebugIdUnit},
	}});

	ecs_struct(world,
	{.entity = ecs_id(GuiWindow),
	.members = {
	{.name = "dummy", .type = ecs_id(ecs_i32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(GuiColumnComponent),
	.members = {
	{.name = "member", .type = ecs_id(ecs_entity_t)},
	{.name = "unit", .type = ecs_id(ecs_entity_t)},
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

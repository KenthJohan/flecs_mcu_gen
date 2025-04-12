#include "Gui.h"

ECS_COMPONENT_DECLARE(GuiWindow);
ECS_COMPONENT_DECLARE(GuiColumnComponent);
ECS_COMPONENT_DECLARE(GuiType);
ECS_COMPONENT_DECLARE(GuiElement);
ECS_COMPONENT_DECLARE(GuiColor3);
ECS_TAG_DECLARE(GuiDebugIdUnit);

void GuiImport(ecs_world_t *world)
{
	ECS_MODULE(world, Gui);
	ecs_set_name_prefix(world, "Gui");
	ECS_COMPONENT_DEFINE(world, GuiWindow);
	ECS_COMPONENT_DEFINE(world, GuiColumnComponent);
	ECS_COMPONENT_DEFINE(world, GuiType);
	ECS_COMPONENT_DEFINE(world, GuiElement);
	ECS_COMPONENT_DEFINE(world, GuiColor3);
	//ecs_add_id(world, ecs_id(GuiElement), EcsTraversable);
	//ecs_add_id(world, ecs_id(GuiElement), EcsInheritable);

	GuiDebugIdUnit = ecs_unit_init(world, &(ecs_unit_desc_t){ 
		.entity = ecs_entity(world, { .name = "DebugId" }),
		.quantity = EcsData,
		.symbol = "did" });

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
	

}

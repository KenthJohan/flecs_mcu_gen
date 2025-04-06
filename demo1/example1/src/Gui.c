#include "Gui.h"

ECS_COMPONENT_DECLARE(GuiWindow);
ECS_COMPONENT_DECLARE(GuiTabs);
ECS_COMPONENT_DECLARE(GuiTab);
ECS_COMPONENT_DECLARE(GuiNodeTreeReflection);
ECS_COMPONENT_DECLARE(GuiColumnComponent);

void GuiImport(ecs_world_t *world)
{
	ECS_MODULE(world, Gui);
	ecs_set_name_prefix(world, "Gui");
	ECS_COMPONENT_DEFINE(world, GuiWindow);
	ECS_COMPONENT_DEFINE(world, GuiTabs);
	ECS_COMPONENT_DEFINE(world, GuiTab);
	ECS_COMPONENT_DEFINE(world, GuiNodeTreeReflection);
	ECS_COMPONENT_DEFINE(world, GuiColumnComponent);

	ecs_struct(world,
	{.entity = ecs_id(GuiWindow),
	.members = {
	{.name = "dummy", .type = ecs_id(ecs_i32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(GuiTabs),
	.members = {
	{.name = "dummy", .type = ecs_id(ecs_i32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(GuiTab),
	.members = {
	{.name = "dummy", .type = ecs_id(ecs_i32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(GuiNodeTreeReflection),
	.members = {
	{.name = "dummy", .type = ecs_id(ecs_i32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(GuiColumnComponent),
	.members = {
	{.name = "dummy", .type = ecs_id(ecs_i32_t)},
	}});
}

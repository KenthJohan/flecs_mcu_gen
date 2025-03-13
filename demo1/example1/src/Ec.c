#include "Ec.h"

ECS_COMPONENT_DECLARE(EcPin);

void EcImport(ecs_world_t *world)
{
	ECS_MODULE(world, Ec);
	ecs_set_name_prefix(world, "Ec");

	ECS_COMPONENT_DEFINE(world, EcPin);

	ecs_struct(world,
	{.entity = ecs_id(EcPin),
	.members = {
	{.name = "slices", .type = ecs_id(ecs_i32_t)},
	{.name = "stacks", .type = ecs_id(ecs_i32_t)},
	}});

}
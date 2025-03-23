#include "Ec.h"

ECS_COMPONENT_DECLARE(EcPinAttribute);
ECS_COMPONENT_DECLARE(EcPin);

void EcImport(ecs_world_t *world)
{
	ECS_MODULE(world, Ec);
	ecs_set_name_prefix(world, "Ec");

	ECS_COMPONENT_DEFINE(world, EcPinAttribute);
	ECS_COMPONENT_DEFINE(world, EcPin);

    ecs_bitmask(world,{
        .entity = ecs_id(EcPinAttribute),
        .constants = {
            { .name = "Inpout", .value = EcPinAttribute_INPUT },
            { .name = "Output", .value = EcPinAttribute_OUTPUT },
            { .name = "InOut", .value = EcPinAttribute_INOUT }
        }
    });

	ecs_struct(world,
	{.entity = ecs_id(EcPin),
	.members = {
	{.name = "pad", .type = ecs_id(ecs_i32_t)},
	{.name = "attribute", .type = ecs_id(EcPinAttribute)},
	}});

}
#include "Ec.h"

ECS_COMPONENT_DECLARE(EcPinAttribute);
ECS_COMPONENT_DECLARE(EcPin);
ECS_COMPONENT_DECLARE(EcPeripheral);
ECS_COMPONENT_DECLARE(EcField);
ECS_COMPONENT_DECLARE(EcRegister);
ECS_COMPONENT_DECLARE(EcSignal);
ECS_COMPONENT_DECLARE(EcQuery);

void EcImport(ecs_world_t *world)
{
	ECS_MODULE(world, Ec);
	ecs_set_name_prefix(world, "Ec");

	ECS_COMPONENT_DEFINE(world, EcPinAttribute);
	ECS_COMPONENT_DEFINE(world, EcPin);
	ECS_COMPONENT_DEFINE(world, EcPeripheral);
	ECS_COMPONENT_DEFINE(world, EcField);
	ECS_COMPONENT_DEFINE(world, EcRegister);
	ECS_COMPONENT_DEFINE(world, EcSignal);
	ECS_COMPONENT_DEFINE(world, EcQuery);

	ecs_bitmask(world, {.entity = ecs_id(EcPinAttribute),
	                   .constants = {
	                   {.name = "Input", .value = EcPinAttribute_INPUT},
	                   {.name = "Output", .value = EcPinAttribute_OUTPUT},
	                   {.name = "InOut", .value = EcPinAttribute_INOUT}}});

	ecs_struct(world,
	{.entity = ecs_id(EcPin),
	.members = {
	{.name = "pad", .type = ecs_id(ecs_i32_t)},
	{.name = "attribute", .type = ecs_id(EcPinAttribute)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EcPeripheral),
	.members = {
	{.name = "dummy", .type = ecs_id(ecs_i32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EcField),
	.members = {
	{.name = "bitoffset", .type = ecs_id(ecs_i32_t)},
	{.name = "bitwidth", .type = ecs_id(ecs_i32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EcRegister),
	.members = {
	{.name = "address", .type = ecs_id(ecs_u32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EcSignal),
	.members = {
	{.name = "dummy", .type = ecs_id(ecs_i32_t)},
	}});
}
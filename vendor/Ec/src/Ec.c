#include "Ec.h"

ECS_COMPONENT_DECLARE(EcPinAttribute);
ECS_COMPONENT_DECLARE(EcPin);
ECS_COMPONENT_DECLARE(EcPeripheral);
ECS_COMPONENT_DECLARE(EcField);
ECS_COMPONENT_DECLARE(EcRegister);
ECS_COMPONENT_DECLARE(EcSignal);
ECS_COMPONENT_DECLARE(EcQuery);
ECS_COMPONENT_DECLARE(EcAf);
ECS_COMPONENT_DECLARE(EcAccess);
ECS_COMPONENT_DECLARE(EcArea);
ECS_COMPONENT_DECLARE(EcGroup);
ECS_COMPONENT_DECLARE(EcTest);

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
	ECS_COMPONENT_DEFINE(world, EcAf);
	ECS_COMPONENT_DEFINE(world, EcAccess);
	ECS_COMPONENT_DEFINE(world, EcArea);
	ECS_COMPONENT_DEFINE(world, EcGroup);
	ECS_COMPONENT_DEFINE(world, EcTest);

	ecs_add_pair(world, ecs_id(EcSignal), EcsOnInstantiate, EcsInherit);


	ecs_bitmask(world,
	{.entity = ecs_id(EcPinAttribute),
	.constants = {
	{.name = "Input", .value = EcPinAttribute_INPUT},
	{.name = "Output", .value = EcPinAttribute_OUTPUT},
	{.name = "InOut", .value = EcPinAttribute_INOUT}}});

	ecs_bitmask(world,
	{.entity = ecs_id(EcAccess),
	.constants = {
	{.name = "R", .value = EcAccess_READ},
	{.name = "W", .value = EcAccess_WRITE},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EcArea),
	.members = {
	{.name = "offset", .type = ecs_id(ecs_i32_t)},
	{.name = "width", .type = ecs_id(ecs_i32_t)},
	{.name = "access", .type = ecs_id(EcAccess)},
	{.name = "unit", .type = ecs_id(ecs_entity_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EcPin),
	.members = {
	{.name = "pad", .type = ecs_id(ecs_i32_t)},
	{.name = "attribute", .type = ecs_id(EcPinAttribute)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EcPeripheral),
	.members = {
	{.name = "peripheral_dummy", .type = ecs_id(ecs_i32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EcField),
	.members = {
	{.name = "bitoffset", .type = ecs_id(ecs_i32_t)},
	{.name = "bitwidth", .type = ecs_id(ecs_i32_t)},
	{.name = "access", .type = ecs_id(EcAccess)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EcRegister),
	.members = {
	{.name = "address", .type = ecs_id(ecs_u32_t)},
	{.name = "access", .type = ecs_id(EcAccess)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EcSignal),
	.members = {
	{.name = "signal_dummy", .type = ecs_id(ecs_i32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EcGroup),
	.members = {
	{.name = "group_dummy", .type = ecs_id(ecs_i32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EcTest),
	.members = {
	{.name = "g1", .type = ecs_id(EcGroup)},
	{.name = "g2", .type = ecs_id(EcGroup)},
	}});

	ecs_enum(world,
	{.entity = ecs_id(EcAf), // Make sure to use existing id
	.constants = {
	{.name = "AF0", .value = EcAF0},
	{.name = "AF1", .value = EcAF1},
	{.name = "AF2", .value = EcAF2},
	{.name = "AF3", .value = EcAF3},
	{.name = "AF4", .value = EcAF4},
	{.name = "AF5", .value = EcAF5},
	{.name = "AF6", .value = EcAF6},
	{.name = "AF7", .value = EcAF7},
	{.name = "AF8", .value = EcAF8},
	{.name = "AF9", .value = EcAF9},
	{.name = "AF10", .value = EcAF10},
	{.name = "AF11", .value = EcAF11},
	{.name = "AF12", .value = EcAF12},
	{.name = "AF13", .value = EcAF13},
	{.name = "AF14", .value = EcAF14},
	{.name = "AF15", .value = EcAF15}}});

	ecs_iter_t it = ecs_children(world, ecs_id(EcAf));
	while (ecs_children_next(&it)) {
		for (int i = 0; i < it.count; i++) {
			ecs_add_id(world, it.entities[i], EcsSymmetric);
		}
	}
}
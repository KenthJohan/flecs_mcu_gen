#pragma once

#include <flecs.h>

typedef uint64_t EcPinAttribute;
#define EcPinAttribute_INPUT 1
#define EcPinAttribute_OUTPUT 2
#define EcPinAttribute_INOUT 4

typedef struct {
	int32_t pad;
	EcPinAttribute attribute;
} EcPin;

typedef struct {
	int32_t dummy;
} EcPeripheral;

typedef struct {
	int32_t bitoffset;
	int32_t bitwidth;
} EcField;

typedef struct {
	uint32_t address;
} EcRegister;

typedef struct {
	int32_t dummy;
} EcSignal;

typedef struct {
	ecs_query_t *q;
} EcQuery;

extern ECS_COMPONENT_DECLARE(EcPinAttribute);
extern ECS_COMPONENT_DECLARE(EcPin);
extern ECS_COMPONENT_DECLARE(EcPeripheral);
extern ECS_COMPONENT_DECLARE(EcField);
extern ECS_COMPONENT_DECLARE(EcRegister);
extern ECS_COMPONENT_DECLARE(EcSignal);
extern ECS_COMPONENT_DECLARE(EcQuery);

void EcImport(ecs_world_t *world);
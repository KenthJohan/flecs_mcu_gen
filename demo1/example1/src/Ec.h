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


extern ECS_COMPONENT_DECLARE(EcPinAttribute);
extern ECS_COMPONENT_DECLARE(EcPin);
extern ECS_COMPONENT_DECLARE(EcPeripheral);

void EcImport(ecs_world_t *world);
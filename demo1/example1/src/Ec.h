#pragma once

#include <flecs.h>

typedef uint64_t EcPinAttribute;
#define EcPinAttribute_INPUT 1
#define EcPinAttribute_OUTPUT 2
#define EcPinAttribute_INOUT 4

typedef uint64_t EcAccess;
#define EcAccess_READ 0x01
#define EcAccess_WRITE 0x02

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
	EcAccess access;
} EcField;

typedef struct {
	uint32_t address;
	EcAccess access;
} EcRegister;

typedef struct {
	int32_t dummy;
} EcSignal;

typedef struct {
	ecs_query_t *q;
} EcQuery;

typedef enum {
	EcAF0,
	EcAF1,
	EcAF2,
	EcAF3,
	EcAF4,
	EcAF5,
	EcAF6,
	EcAF7,
	EcAF8,
	EcAF9,
	EcAF10,
	EcAF11,
	EcAF12,
	EcAF13,
	EcAF14,
	EcAF15
} EcAf;

extern ECS_COMPONENT_DECLARE(EcPinAttribute);
extern ECS_COMPONENT_DECLARE(EcPin);
extern ECS_COMPONENT_DECLARE(EcPeripheral);
extern ECS_COMPONENT_DECLARE(EcField);
extern ECS_COMPONENT_DECLARE(EcRegister);
extern ECS_COMPONENT_DECLARE(EcRegisterAttribute);
extern ECS_COMPONENT_DECLARE(EcSignal);
extern ECS_COMPONENT_DECLARE(EcQuery);
extern ECS_COMPONENT_DECLARE(EcAf);

void EcImport(ecs_world_t *world);
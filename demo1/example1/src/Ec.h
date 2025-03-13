#pragma once

#include <flecs.h>

typedef struct {
	EcsDocDescription desc;
} EcPin;

extern ECS_COMPONENT_DECLARE(EcPin);

void EcImport(ecs_world_t *world);
#pragma once
#include <flecs.h>

int serialize(ecs_world_t *world, ecs_entity_t type, const void *ptr, ecs_entity_t egui);
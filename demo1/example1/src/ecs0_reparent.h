#pragma once
#include <flecs.h>

void ecs0_reparent_by_subname(ecs_world_t *world, char const *filters[], ecs_query_t *q);
void ecs0_reparent_by_subname1(ecs_world_t *world, char const *filters[], ecs_entity_t component);
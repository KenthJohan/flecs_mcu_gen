#pragma once
#include <flecs.h>

void eg_reparent_by_subname(ecs_world_t *world, char const *names[], ecs_query_t *q);
void eg_reparent_by_subname1(ecs_world_t *world, char const *names[], ecs_entity_t component);
#pragma once
#include <flecs.h>

void eg_reparent_by_subname(ecs_world_t *world, char const *names[], ecs_query_t *q);
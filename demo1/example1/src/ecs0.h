#pragma once

#include <flecs.h>

int ecs0_get_entities_by_query(ecs_world_t *world, ecs_query_t * query, ecs_entity_t entities[], int count);
int ecs0_get_entities_from_parent(ecs_world_t *world, ecs_entity_t parent, ecs_id_t component, ecs_entity_t entities[], int count);
#pragma once

#include <flecs.h>

int ecs0_get_entities_by_query(ecs_world_t *world, ecs_query_t *query, ecs_entity_t entities[], int count);
int ecs0_get_entities_from_parent(ecs_world_t *world, ecs_entity_t parent, ecs_id_t component, ecs_entity_t entities[], int count);

bool ecs0_has_children(ecs_world_t *world, ecs_entity_t entity);

int32_t ecs0_sum_offset(ecs_world_t *world, ecs_entity_t const members[], ecs_entity_t *last);
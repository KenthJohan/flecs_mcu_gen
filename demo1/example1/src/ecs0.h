#pragma once

#include <flecs.h>

int ecs0_get_entities_by_query(ecs_world_t *world, ecs_query_t *query, ecs_entity_t entities[], int count);
int ecs0_get_entities_from_parent(ecs_world_t *world, ecs_entity_t parent, ecs_id_t component, ecs_entity_t entities[], int count);

bool ecs0_has_children(ecs_world_t *world, ecs_entity_t entity);

int ecs0_flecs_expr_ser_primitive(
const ecs_world_t *world,
ecs_primitive_kind_t kind,
const void *base,
ecs_strbuf_t *str,
bool is_expr);


int ecs0_expr_ser_bitmask(const ecs_world_t *world, ecs_entity_t type, const void *ptr, ecs_strbuf_t *str);
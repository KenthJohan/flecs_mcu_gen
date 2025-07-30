#pragma once
#include <flecs.h>

bool bgui_entinfo_draw(ecs_world_t *world, ecs_entity_t type, void *ptr, ecs_entity_t egui);

void bgui_entinfo_iterate_components(ecs_world_t *world, ecs_entity_t egui, ecs_entity_t subject);
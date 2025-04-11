#pragma once


#include <flecs.h>

int bgui_children_sum(ecs_world_t *world, ecs_entity_t components[], int count);
void gui_ntt_reflection(ecs_world_t *world, ecs_entity_t parent, ecs_entity_t components[]);
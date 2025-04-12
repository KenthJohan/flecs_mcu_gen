#include "ecs0.h"


int ecs0_get_entities_by_query(ecs_world_t *world, ecs_query_t * query, ecs_entity_t entities[], int count)
{
	ecs_iter_t it = ecs_query_iter(world, query);
	int i = 0;
	while (ecs_query_next(&it)) {
		for (int j = 0; j < it.count; j++) {
			if (i >= count) {
				goto exit_return;
			}
			entities[i] = it.entities[j];
			i++;
		}
	}
exit_return:
	return i;
}

int ecs0_get_entities_from_parent(ecs_world_t *world, ecs_entity_t parent, ecs_id_t component, ecs_entity_t entities[], int count)
{
	ecs_query_t *q = ecs_query(world, {
		.terms = {
			{ ecs_childof(parent), .inout = EcsInOutFilter },
			{ component, .inout = EcsInOutFilter },
			
		}
	});
	int k = ecs0_get_entities_by_query(world, q, entities, count);
	ecs_query_fini(q);
	return k;
}
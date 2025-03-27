#include "eg_reparent.h"
#include "Ec.h"

static bool str_cmp_sub0(char const *a, char const *b)
{
	while (*a && *b) {
		if (*a != *b) {
			return false;
		}
		a++;
		b++;
	}
	return true;
}

static int str_cmp_sub0v(char const *a, char const *bv[])
{
	for (int i = 0; bv[i]; i++) {
		if (str_cmp_sub0(a, bv[i])) {
			return i;
		}
	}
	return -1;
}

void eg_reparent_by_subname(ecs_world_t *world, char const *names[], ecs_query_t *q)
{
	ecs_defer_begin(world);
	int k = 0;
	ecs_iter_t it = ecs_query_iter(world, q);
	while (ecs_query_next(&it)) {
		for (int i = 0; i < it.count; i++) {
			if (k > 1000) {
				return;
			}
			k++;
			int j = str_cmp_sub0v(ecs_get_name(world, it.entities[i]), names);
			if (j < 0) {
				continue;
			}
			ecs_entity_t parent = ecs_get_parent(world, it.entities[i]);
			// Create a child of parent
			ecs_entity_t child = ecs_entity_init(world, &(ecs_entity_desc_t){
			                                            .name = names[j],
			                                            .parent = parent,
			                                            });

			ecs_add_pair(world, it.entities[i], EcsChildOf, child);

			// ecs_add_entity(world, parent, it.entities[i]);
			// ecs_remove_entity(world, parent, it.entities[i]);
			// printf("Reorder: %jX %s\n", child, ecs_get_name(world, child));
		}
	}
	ecs_defer_end(world);
}
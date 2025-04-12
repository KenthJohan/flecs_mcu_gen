#include "ecs0_reparent.h"
#include <ctype.h>

static bool str_cmp_sub0(char const *a, char const *b, char ** a_endptr)
{
	while (*a && *b) {
		if (isdigit(*a) && (*b) == '*') {
			// Do nothing
		} else if (isalpha(*a) && (*b) == '#') {
			// Do nothing
		} else if (*a == *b) {
			// Do nothing
		} else {
			if (a_endptr) {
				*a_endptr = (char *)a;
			}
			return false;
		}
		a++;
		b++;
	}
	if (a_endptr) {
		*a_endptr = (char *)a;
	}
	return true;
}

static int str_cmp_sub0v(char const *a, char const *bv[], char ** a_endptr)
{
	for (int i = 0; bv[i]; i++) {
		if (str_cmp_sub0(a, bv[i], a_endptr)) {
			return i;
		}
	}
	return -1;
}

void ecs0_reparent_by_subname(ecs_world_t *world, char const *names[], ecs_query_t *q)
{
	ecs_defer_begin(world);
	ecs_iter_t it = ecs_query_iter(world, q);
	while (ecs_query_next(&it)) {
		for (int i = 0; i < it.count; i++) {
			char const *name = ecs_get_name(world, it.entities[i]);
			if (!name) {
				continue;
			}
			char * endptr;
			int j = str_cmp_sub0v(name, names, &endptr);
			if (j < 0) {
				continue;
			}
			char namebuf[64] = {0};
			if ((endptr - name) >= (intptr_t)sizeof(namebuf)) {
				ecs_err("name too long: %s", name);
				continue;
			}
			memcpy(namebuf, name, endptr - name);
			namebuf[endptr - name] = 0;

			ecs_entity_t parent = ecs_get_parent(world, it.entities[i]);
			// Create a child of parent
			ecs_entity_t child = ecs_entity_init(world, &(ecs_entity_desc_t){
			                                            .name = namebuf,
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

void ecs0_reparent_by_subname1(ecs_world_t *world, char const *names[], ecs_entity_t component)
{
	ecs_query_t *q = ecs_query(world,
	{
	.terms = {
	{.id = component},
	},
	});
	ecs0_reparent_by_subname(world, names, q);
	ecs_query_fini(q);
}
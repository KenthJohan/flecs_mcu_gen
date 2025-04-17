#include <stdlib.h>
#include <stdio.h>
#include <flecs.h>


int main(int argc, char *argv[])
{
	ecs_os_set_api_defaults();
	ecs_os_api_t os_api = ecs_os_get_api();
	ecs_os_set_api(&os_api);

	ecs_world_t *world = ecs_init();
	ECS_IMPORT(world, FlecsUnits);
	ECS_IMPORT(world, FlecsDoc);
	ECS_IMPORT(world, FlecsStats);

	ecs_set(world, EcsWorld, EcsRest, {.port = 0});
	printf("Remote: %s\n", "https://www.flecs.dev/explorer/?remote=true");

	//ecs_script_run(world, "s0", "braces{}");
	//ecs_script_run(world, "s1", "a{flecs.doc.Description : {\"{braces}\"}}");

	
	ecs_log_set_level(0);
	ecs_script_run_file(world, "script1.flecs");
	ecs_log_set_level(-1);
	


	while (1) {
		ecs_progress(world, 0.0f);
		ecs_sleepf(0.016f);
	}

	return 0;
}

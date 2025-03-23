#include <stdlib.h>
#include <stdio.h>
#include <flecs.h>
#include "Ec.h"
#include <eximgui.h>

int main(int argc, char *argv[])
{

	ecs_os_set_api_defaults();
	ecs_os_api_t os_api = ecs_os_get_api();
	ecs_os_set_api(&os_api);

	ecs_world_t *world = ecs_init();
	ECS_IMPORT(world, FlecsUnits);
	ECS_IMPORT(world, FlecsDoc);
	ECS_IMPORT(world, FlecsStats);

	ECS_IMPORT(world, Ec);

	ecs_set(world, EcsWorld, EcsRest, {.port = 0});
	printf("Remote: %s\n", "https://www.flecs.dev/explorer/?remote=true");

	/*
	ecs_log_set_level(0);
	ecs_script_run_file(world, "config/script1.flecs");
	ecs_log_set_level(-1);

	ecs_log_set_level(0);
	ecs_script_run_file(world, "config/STM32G030.flecs");
	ecs_log_set_level(-1);

	ecs_log_set_level(0);
	ecs_script_run_file(world, "config/STM32G030_p.flecs");
	ecs_log_set_level(-1);

	ecs_log_set_level(0);
	ecs_script_run_file(world, "config/board1.flecs");
	ecs_log_set_level(-1);
	*/

	/*
	{
	    // Create a query for the draw system
	    ecs_entity_t e_draw1 = ecs_lookup(world, "xapp.renderer");
	    ecs_query_t *q = ecs_query(world,
	    {.terms = {
	     {.id = ecs_id(EgBaseOffsetCount), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	     {.id = ecs_id(Transformation), .src.id = EcsSelf, .inout = EcsIn},
	     {.id = ecs_id(EgCamerasState), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn}}});
	    ecs_set(world, e_draw1, EgGpuDraw1, {.query = q});
	}

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_Draw", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_Draw,
	.query.terms = {
	{.id = ecs_id(EgGpuDraw1), .src.id = EcsSelf, .inout = EcsIn},
	{.id = ecs_id(EgShapesRectangle), .src.id = EcsSelf, .inout = EcsInOut},
	{.id = ecs_id(EgGpuDevice), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgGpuPipeline), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgGpuBufferVertex), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgGpuTexture), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsInOut}, // Depth texture
	{.id = ecs_id(EgWindowsWindow), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgGpuWindow), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	}});
	*/

	eximgui_t eximgui = {.clear_color = {0.45f, 0.55f, 0.60f, 1.00f}};
	eximgui_init(&eximgui);
	while (!eximgui.done) {
		eximgui_progress(&eximgui);
		ecs_progress(world, 0.0f);
		ecs_sleepf(0.016f);
	}
	eximgui_fini(&eximgui);



	return 0;
}

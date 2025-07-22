#include <stdlib.h>
#include <stdio.h>
#include <flecs.h>
#include <ecsx.h>


typedef struct {
	float x;
	float y;
} Position;

typedef struct {
	Position a;
	Position b;
} Line;

typedef struct {
	int16_t n[6];
	uint16_t b[6];
	Position start;
	Line lines[10];
} Paint;

ECS_COMPONENT_DECLARE(Position);
ECS_COMPONENT_DECLARE(Line);
ECS_COMPONENT_DECLARE(Paint);



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

	ECS_COMPONENT_DEFINE(world, Position);
	ECS_COMPONENT_DEFINE(world, Line);
	ECS_COMPONENT_DEFINE(world, Paint);

	ecs_struct(world,
	{.entity = ecs_id(Position),
	.members = {
	{.name = "x", .type = ecs_id(ecs_f32_t)},
	{.name = "y", .type = ecs_id(ecs_f32_t)}
	}});

	ecs_struct(world,
	{.entity = ecs_id(Line),
	.members = {
	{.name = "a", .type = ecs_id(Position)},
	{.name = "b", .type = ecs_id(Position)}
	}});

	ecs_struct(world,
	{.entity = ecs_id(Paint),
	.members = {
	{.name = "n", .type = ecs_id(ecs_i16_t), .count = 6},
	{.name = "b", .type = ecs_id(ecs_u8_t), .count = 6},
	{.name = "start", .type = ecs_id(Position)},
	{.name = "lines", .type = ecs_id(Line), .count = 10}
	}});

	ecsx_ops_print(world, ecs_id(Paint));

	/*
	ecs_log_set_level(0);
	ecs_script_run_file(world, "script1.flecs");
	ecs_log_set_level(-1);
	*/
	


	while (1) {
		ecs_progress(world, 0.0f);
		ecs_sleepf(0.016f);
	}

	return 0;
}

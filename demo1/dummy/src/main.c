#include <stdlib.h>
#include <stdio.h>
#include <flecs.h>
#include <ecsx.h>
typedef enum {
	Red,
	Green,
	Blue
} Color;

typedef void (*Fx)(void *ptr);

typedef struct {
	Fx fx;
	Color color;
} Comp;

void fx1(void *ptr)
{
	printf("fx1\n");
}

void fx2(void *ptr)
{
	printf("fx2\n");
}

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

	ECS_COMPONENT(world, Fx);
	ECS_COMPONENT(world, Color);
	ECS_COMPONENT(world, Comp);

	ecs_enum_init(world,
	&(ecs_enum_desc_t){
	.entity = ecs_id(Color), // Make sure to use existing id
	.constants = {
	{.name = "Red", .value = Red},
	{.name = "Green", .value = Green},
	{.name = "Blue", .value = Blue}}});

	ecs_enum_init(world,
	&(ecs_enum_desc_t){
	.underlying_type = ecs_id(ecs_uptr_t),
	.entity = ecs_id(Fx),
	.constants = {
	{.name = "null", .value = (ecs_uptr_t)0},
	{.name = "fx1", .value = (ecs_uptr_t)fx1},
	{.name = "fx2", .value = (ecs_uptr_t)fx2},
	}});

	ecs_struct_init(world,
	&(ecs_struct_desc_t){
	.entity = ecs_id(Comp), // Make sure to use existing id
	.members = {
	{.name = "fx", .type = ecs_id(Fx)},
	{.name = "color", .type = ecs_id(Color)}}});

	ecs_script_run(world, "s1", "a{Comp:{fx:0,color:Green}}", NULL);
	ecs_script_run(world, "s2", "b{Comp:{fx:fx1,color:Green}}", NULL);

	while (1) {
		ecs_progress(world, 0.0f);
		ecs_sleepf(0.016f);
	}

	return 0;
}

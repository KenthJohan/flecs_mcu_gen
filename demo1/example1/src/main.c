#include <stdlib.h>
#include <stdio.h>
#include <flecs.h>
#include "Ec.h"
#include <eximgui.h>

static void Sys_GuiListPins(ecs_iter_t *it)
{
	eximgui_t *eximgui = it->ctx;
	EcPin *pin = ecs_field(it, EcPin, 0); // self, in
	if (eximgui->show_another_window == false) {
		return;
	}
	gui_begin(eximgui, "Pins", &eximgui->show_another_window);
	for (int i = 0; i < it->count; ++i, ++pin) {
		gui_text(eximgui, ecs_get_name(it->world, it->entities[i]));
	}
	gui_end(eximgui);
}


static void Sys_GuiListPeripherals(ecs_iter_t *it)
{
	eximgui_t *eximgui = it->ctx;
	EcPeripheral *per = ecs_field(it, EcPeripheral, 0); // self, in
	if (eximgui->show_demo_window == false) {
		return;
	}
	gui_begin(eximgui, "Pins", &eximgui->show_demo_window);
	for (int i = 0; i < it->count; ++i, ++per) {
		gui_text(eximgui, ecs_get_name(it->world, it->entities[i]));
	}
	gui_end(eximgui);
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

	ECS_IMPORT(world, Ec);

	ecs_set(world, EcsWorld, EcsRest, {.port = 0});
	printf("Remote: %s\n", "https://www.flecs.dev/explorer/?remote=true");

	
	ecs_log_set_level(0);
	ecs_script_run_file(world, "config/script1.flecs");
	ecs_log_set_level(-1);

	ecs_log_set_level(0);
	ecs_script_run_file(world, "config/STM32G030.flecs");
	ecs_log_set_level(-1);
	


	eximgui_t eximgui = {.clear_color = {0.45f, 0.55f, 0.60f, 1.00f}};
	eximgui_init(&eximgui);

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Sys_GuiListPins", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = Sys_GuiListPins,
	.ctx = &eximgui,
	.query.terms = {
	{.id = ecs_id(EcPin), .inout = EcsIn},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Sys_GuiListPeripherals", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = Sys_GuiListPeripherals,
	.ctx = &eximgui,
	.query.terms = {
	{.id = ecs_id(EcPeripheral), .inout = EcsIn},
	}});

	while (!eximgui.done) {
		eximgui_begin_frame(&eximgui);
		ecs_progress(world, 0.0f);
		eximgui_end_frame(&eximgui);
		ecs_sleepf(0.016f);
	}
	eximgui_fini(&eximgui);

	return 0;
}

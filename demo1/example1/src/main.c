#include <stdlib.h>
#include <stdio.h>
#include <flecs.h>
#include <eximgui.h>
#include "Ec.h"
#include "eg_reparent.h"
#include "bgui.h"
#include "Gui.h"


void draw_tree_window(ecs_world_t *world)
{
	gui_begin("Window1", NULL);
	if (gui_tab_begin("Peripherals", 0)) {
		if (gui_tab_item_begin("Peripherals", 0)) {
			ecs_entity_t parent = ecs_lookup(world, "xmcu.STM32G030.peripherals");
			ecs_entity_t components[] = {
				ecs_id(EcRegister),
				ecs_id(EcField),
				0
			};
			gui_ntt_reflection(world, parent, components);
			gui_tab_item_end();
		}
		if (gui_tab_item_begin("Signals", 0)) {
			ecs_entity_t parent = ecs_lookup(world, "xmcu.STM32G030.signals");
			ecs_entity_t components[] = {
				ecs_id(EcSignal),
				0
			};
			gui_ntt_reflection(world, parent, components);
			gui_tab_item_end();
		}
		if (gui_tab_item_begin("Pins", 0)) {
			ecs_entity_t parent = ecs_lookup(world, "xmcu.STM32G030.pins");
			ecs_entity_t components[] = {
				ecs_id(EcPin),
				0
			};
			gui_ntt_reflection(world, parent, components);
			gui_tab_item_end();
		}
		gui_tab_end();
	}
	gui_end();
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
	ECS_IMPORT(world, Gui);

	ecs_set(world, EcsWorld, EcsRest, {.port = 0});
	printf("Remote: %s\n", "https://www.flecs.dev/explorer/?remote=true");

	ecs_log_set_level(0);
	ecs_script_run_file(world, "config/script1.flecs");
	ecs_log_set_level(-1);

	ecs_log_set_level(0);
	ecs_script_run_file(world, "config/STM32G030.flecs");
	ecs_log_set_level(-1);

	{
		char const *names[] = {"DMA", "GPIO", "TIM", "SPI", "I2C", "USART", NULL};
		eg_reparent_by_subname1(world, names, ecs_id(EcPeripheral));
	}

	{
		char const *names[] = {"DMA", "GPIO", "TIM", "SPI", "I2C", "USART", "LPUART", "LPTIM", "I2S", "RCC", "SYS", NULL};
		eg_reparent_by_subname1(world, names, ecs_id(EcSignal));
	}

	{
		char const *names[] = {"USART*", "TIM*", "I2S*", "LPTIM*", "SPI*", "LPUART*", "I2C*", NULL};
		eg_reparent_by_subname1(world, names, ecs_id(EcSignal));
	}

	{
		char const *names[] = {"PA", "PB", "PC", "PD", "PF", NULL};
		eg_reparent_by_subname1(world, names, ecs_id(EcPin));
	}


	eximgui_t eximgui = {.clear_color = {0.45f, 0.55f, 0.60f, 1.00f}};
	eximgui_init(&eximgui);

	ecs_entity_t parent = ecs_lookup(world, "xmcu.STM32G030");
	// print name of parent
	if (parent) {
		printf("Parent: %s\n", ecs_get_name(world, parent));
	} else {
		printf("Parent not found\n");
	}

	while (!eximgui.done) {
		eximgui_begin_frame(&eximgui);
		draw_tree_window(world);
		ecs_progress(world, 0.0f);
		eximgui_end_frame(&eximgui);
		ecs_sleepf(0.016f);
	}
	eximgui_fini(&eximgui);

	return 0;
}

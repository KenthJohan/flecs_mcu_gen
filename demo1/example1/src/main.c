#include <stdlib.h>
#include <stdio.h>
#include <flecs.h>
#include <jmgui.h>
#include <Ec.h>
#include "ecs0_reparent.h"
#include "Gui.h"
#include "GuiDraws.h"

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
	ECS_IMPORT(world, GuiDraws);

	ecs_set(world, EcsWorld, EcsRest, {.port = 0});
	printf("Remote: %s\n", "https://www.flecs.dev/explorer/?remote=true");

	ecs_log_set_level(0);
	ecs_script_run_file(world, "config/STM32G030.flecs");
	ecs_log_set_level(-1);

	{
		char const *names[] = {"DMA", "GPIO", "TIM", "SPI", "I2C", "USART", NULL};
		ecs0_reparent_by_subname1(world, names, ecs_id(EcPeripheral), (ecs_id_t[]){ecs_id(EcGroup), 0});
	}

	{
		char const *names[] = {"DMA", "GPIO", "TIM", "SPI", "I2C", "USART", "LPUART", "LPTIM", "I2S", "RCC", "SYS", NULL};
		ecs0_reparent_by_subname1(world, names, ecs_id(EcSignal), (ecs_id_t[]){ecs_id(EcGroup), 0});
	}

	{
		char const *names[] = {"USART*", "TIM*", "I2S*", "LPTIM*", "SPI*", "LPUART*", "I2C*", NULL};
		ecs0_reparent_by_subname1(world, names, ecs_id(EcSignal), (ecs_id_t[]){ecs_id(EcGroup), 0});
	}

	{
		char const *names[] = {"PA", "PB", "PC", "PD", "PF", NULL};
		ecs0_reparent_by_subname1(world, names, ecs_id(EcPin), (ecs_id_t[]){ecs_id(EcGroup), 0});
	}

	ecs_log_set_level(0);
	ecs_script_run_file(world, "config/script1.flecs");
	ecs_log_set_level(-1);

	jmgui_context_t jmgui = {.clear_color = {0.45f, 0.55f, 0.60f, 1.00f}};
	jmgui_context_init(&jmgui);

	ecs_entity_t parent = ecs_lookup(world, "xmcu.STM32G030");
	// print name of parent
	if (parent) {
		printf("Parent: %s\n", ecs_get_name(world, parent));
	} else {
		printf("Parent not found\n");
	}

	
	/*
	{
		ecs_query_t *q = ecs_query_init(world,
		&(ecs_query_desc_t){
		.expr = "(flecs.core.Identifier, flecs.core.Name), ?{ec.Group || ec.Peripheral || ec.Field || ec.Register}",
		.group_by = EcsChildOf
		});
		ecs_iter_t it = ecs_query_iter(world, q);
		ecs_iter_set_group(&it, ecs_lookup(world, "xmcu.STM32G030.peripherals"));
		while (ecs_query_next(&it)) {
			for (int i = 0; i < it.count; i++) {
				printf("name: %s\n", ecs_get_name(world, it.entities[i]));
			}
		}
	}
	*/
	

	while (!jmgui.done) {
		jmgui_frame_begin(&jmgui);
		ecs_progress(world, 0.0f);
		jmgui_frame_frame(&jmgui);
		ecs_sleepf(0.016f);
	}
	jmgui_context_fini(&jmgui);

	return 0;
}

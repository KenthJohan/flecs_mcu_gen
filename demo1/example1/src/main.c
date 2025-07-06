#include <stdlib.h>
#include <stdio.h>
#include <flecs.h>
#include <jmgui.h>
#include <Ec.h>
#include "ecs0_reparent.h"
#include "Gui.h"
#include "GuiDraws.h"

void main_abort()
{
	exit(1);
}

ecs_os_api_log_t log_backup = NULL;

void main_log(int32_t level, const char *file, int32_t line, const char *msg)
{
	log_backup(level, file, line, msg);
	switch (level) {
	case -3:
		printf("Break here\n");
		break;
	case -4:
		printf("Break here\n");
		break;
	}
}

void sum(
const ecs_function_ctx_t *ctx,
int argc,
const ecs_value_t *argv,
ecs_value_t *result)
{
	(void)ctx;
	(void)argc;

	*(int64_t *)result->ptr =
	*(int64_t *)argv[0].ptr +
	*(int64_t *)argv[1].ptr;
}

int main(int argc, char *argv[])
{
	ecs_os_set_api_defaults();
	ecs_os_api_t os_api = ecs_os_get_api();
	log_backup = os_api.log_;
	os_api.log_ = main_log;
	os_api.abort_ = main_abort;
	ecs_os_set_api(&os_api);

	ecs_world_t *world = ecs_init();
	ECS_IMPORT(world, FlecsUnits);
	ECS_IMPORT(world, FlecsDoc);
	ECS_IMPORT(world, FlecsStats);
	ECS_IMPORT(world, FlecsScriptMath);

	ECS_IMPORT(world, Ec);
	ECS_IMPORT(world, Gui);
	ECS_IMPORT(world, GuiDraws);

	ecs_set(world, EcsWorld, EcsRest, {.port = 0});
	printf("Remote: %s\n", "https://www.flecs.dev/explorer/?remote=true");

	// Define a sum function
	ecs_function(world,
	{.name = "sum",
	.return_type = ecs_id(ecs_i64_t),
	.params = {
	{.name = "a", .type = ecs_id(ecs_i64_t)},
	{.name = "b", .type = ecs_id(ecs_i64_t)}},
	.callback = sum});

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

	ecs_log_set_level(0);

	jmgui_context_t jmgui = {.clear_color = {0.45f, 0.55f, 0.60f, 1.00f}};
	jmgui_context_init(&jmgui);

	ecs_entity_t parent = ecs_lookup(world, "xmcu.STM32G030");
	if (parent) {
		char *path = ecs_get_path(world, parent);
		ecs_log(0, "Found: %s", path);
		ecs_os_free(path);
	} else {
		printf("xmcu.STM32G030 not found\n");
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

/*
https://jsonformatter.org/xml-viewer

*/
#include <stdlib.h>
#include <stdio.h>
#include <flecs.h>
#include <Ec.h>
#include "parse_svd.h"
#include "parse_modes.h"
#include "printer.h"

#include <mxml.h>

void generate_file()
{
	ecs_world_t *world = ecs_mini();
	result_t result = {0};
	result.color_fields = "#6b93d6";
	result.color_peripherals = "#9b2316";
	result.color_registers = "#2b6316";
	result.doc_mode = 1;
	result.file = fopen("config/STM32G030.flecs", "w");
	if (result.file == NULL) {
		printf("Error opening file!\n");
		exit(1);
	}
	fprintf(result.file, "@color #AA99AF\n");
	fprintf(result.file, "xmcu {}\n");
	fprintf(result.file, "module xmcu\n");
	fprintf(result.file, "module STM32G030\n\n");
	parse_svd_init(&result, "../../svd/stm32g0/STM32G030.svd");
	parse_svd_init(&result, "config/STM32G030.svd");
	parse_modes_init(world, &result, "config/GPIO-STM32G03x_gpio_v1_0_Modes.xml");
	ecs_fini(world);
	fclose(result.file);
}

int main(int argc, char *argv[])
{
	generate_file();

	ecs_world_t *world = ecs_init();
	ECS_IMPORT(world, FlecsRest);
	ECS_IMPORT(world, FlecsStats);
	ECS_IMPORT(world, Ec);
	ecs_script_run_file(world, "config/STM32G030.flecs");
	ecs_set(world, EcsWorld, EcsRest, {.port = 0});

	printf("Remote: %s\n", "https://www.flecs.dev/explorer/?remote=true");
	while (1) {
		ecs_progress(world, 0.0f);
		ecs_sleepf(0.016f);
	}
	ecs_fini(world);
	return 0;
}

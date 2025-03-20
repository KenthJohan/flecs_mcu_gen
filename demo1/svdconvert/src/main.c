/*
https://jsonformatter.org/xml-viewer

*/
#include <stdlib.h>
#include <stdio.h>
#include <flecs.h>
#include "parse_svd.h"
#include "parse_modes.h"
#include "printer.h"


int main(int argc, char *argv[])
{
	ecs_world_t *world = ecs_init();

	result_t result = {0, NULL};
	result.file = fopen("../example1/config/STM32G030.flecs", "w");
	if (result.file == NULL) {
		printf("Error opening file!\n");
		exit(1);
	}

	fprintf(result.file, "@color #AA99AF\n");
	fprintf(result.file, "xmcu {}\n");
	fprintf(result.file, "module xmcu\n");
	fprintf(result.file, "module STM32G030\n\n");


	parse_svd_init(&result);
	parse_modes_init(world, &result);


	/*
	ecs_set(world, EcsWorld, EcsRest, {.port = 0});
	printf("Remote: %s\n", "https://www.flecs.dev/explorer/?remote=true");
	while (1) {
		ecs_progress(world, 0.0f);
		ecs_sleepf(0.016f);
	}
	*/

	return 0;
}

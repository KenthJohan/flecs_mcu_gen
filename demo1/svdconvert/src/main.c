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


	result_flecs_entity_open(&result, "peripherals");
	parse_svd_init(&result);
	result_flecs_entity_close(&result);

	result_flecs_entity_open(&result, "pins");
	parse_modes_init(&result);
	result_flecs_entity_close(&result);

	return 0;
}

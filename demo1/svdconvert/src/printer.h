#pragma once
#include <stdio.h>
#include <stdint.h>

typedef struct {
	int doc_mode;
	int ident;
	FILE *file;
	char const * color_peripherals;
	char const * color_registers;
	char const * color_fields;
} result_t;

void result_indent(result_t *result);
void result_flecs_register(result_t *result, const char *address, const char *access, const char *size);
void result_flecs_field(result_t *result, char const *bitoffset, char const *bitwidth, const char *access);
void result_flecs_entity_open(result_t *result, const char *name, char const * extend, char const * brief, char const * color);
void result_flecs_entity_close(result_t *result);
void result_flecs_pair(result_t *result, char const *pre0, char const *a0, char const *pre1, char const *a1);
void result_flecs_peripheral(result_t *result, char const *address, char const *size);
void result_flecs_signal(result_t *result);
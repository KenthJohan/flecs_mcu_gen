#include "printer.h"
#include "str.h"
#include <string.h>

void result_indent(result_t *result)
{
	for (int i = 0; i < result->ident; i++) {
		fprintf(result->file, "\t");
	}
}





void result_flecs_register(result_t *result, const char *offset, const char *access, const char *bitsize)
{
	char const * access0 = "0";
	if (access) {
		if (strcmp(access, "read-write") == 0) {
			access0 = "R|W";
		} else if (strcmp(access, "read-only") == 0) {
			access0 = "R";
		} else if (strcmp(access, "write-only") == 0) {
			access0 = "W";
		}
	}
	result_indent(result);
	fprintf(result->file, "ec.Access : {%s}\n", access0);
	result_indent(result);
	fprintf(result->file, "ec.Size : {size : {%s, unit:flecs.units.Data.Bits}}\n", bitsize);
	result_indent(result);
	fprintf(result->file, "ec.Offset : {offset : {%s, unit:flecs.units.Data.Bytes}}\n", offset);
}

void result_flecs_field(result_t *result, char const *bitoffset, char const *bitwidth, const char *access)
{
	char const * access0 = "0";
	if (access) {
		if (strcmp(access, "read-write") == 0) {
			access0 = "R|W";
		} else if (strcmp(access, "read-only") == 0) {
			access0 = "R";
		} else if (strcmp(access, "write-only") == 0) {
			access0 = "W";
		}
	}
	result_indent(result);
	fprintf(result->file, "ec.Access : {%s}\n", access0);
	result_indent(result);
	fprintf(result->file, "ec.Size : {size : {%s, unit:flecs.units.Data.Bits}}\n", bitwidth);
	result_indent(result);
	fprintf(result->file, "ec.Offset : {offset : {%s, unit:flecs.units.Data.Bits}}\n", bitoffset);
}

void result_flecs_pair(result_t *result, char const *pre0, char const *a0, char const *pre1, char const *a1)
{
	result_indent(result);
	fprintf(result->file, "(%s.%s, %s.%s)\n", pre0, a0, pre1, a1);
}

void result_flecs_entity_open(result_t *result, const char *name, char const * extend, char const * brief, char const * color)
{
	if (color) {
		result_indent(result);
		fprintf(result->file, "@color %s\n", color);
	}
	if (brief && (result->doc_mode == 1)) {
		char buf[256] = {0};
		str_copy_escape(buf, sizeof(buf), brief);
		result_indent(result);
		fprintf(result->file, "@brief %s\n", buf);
	}
	result_indent(result);
	if (extend) {
		fprintf(result->file, "%s : %s {\n", name, extend);
	} else {
		fprintf(result->file, "%s {\n", name);
	}
	result->ident++;
	if (brief && (result->doc_mode == 0)) {
		char buf[256] = {0};
		str_copy_escape(buf, sizeof(buf), brief);
		result_indent(result);
		fprintf(result->file, "(flecs.doc.Description, flecs.doc.Brief) : {\"%s\"}\n", buf);
	}
}

void result_flecs_entity_close(result_t *result)
{
	result->ident--;
	result_indent(result);
	fprintf(result->file, "}\n");
}

void result_flecs_peripheral(result_t *result, char const *address, char const *size)
{
	result_indent(result);
	fprintf(result->file, "%s\n", "ec.Peripheral : {0}");
	result_indent(result);
	fprintf(result->file, "ec.Size : {size : {%s, unit:flecs.units.Data.Bytes}}\n", size);
	result_indent(result);
	fprintf(result->file, "ec.Offset : {offset : {%s, unit:flecs.units.Data.Bytes}}\n", address);
}

void result_flecs_signal(result_t *result)
{
	result_indent(result);
	fprintf(result->file, "%s\n", "ec.Signal : {0}");
}
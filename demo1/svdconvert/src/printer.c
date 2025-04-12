#include "printer.h"
#include "str.h"
#include <string.h>

void result_indent(result_t *result)
{
	for (int i = 0; i < result->ident; i++) {
		fprintf(result->file, "\t");
	}
}



void result_flecs_description(result_t *result, const char *description)
{
	char buf[256] = {0};
	str_copy_escape(buf, sizeof(buf), description);
	result_indent(result);
	fprintf(result->file, "(flecs.doc.Description, flecs.doc.Brief) : {\"%s\"}\n", buf);
}

void result_flecs_register(result_t *result, const char *address, const char *access, const char *size)
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
	fprintf(result->file, "ec.Register : {address:%s, access:%s}\n", address, access0);
	result_indent(result);
	fprintf(result->file, "ec.Area : {offset:%s, width:%s, access:%s, unit:flecs.units.Data.Bytes}\n", address, size, access0);
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
	fprintf(result->file, "ec.Field : {bitoffset:%s, bitwidth:%s, access:%s}\n", bitoffset, bitwidth, access0);
	result_indent(result);
	fprintf(result->file, "ec.Area : {offset:%s, width:%s, access:%s, unit:flecs.units.Data.Bits}\n", bitoffset, bitwidth, access0);
}

void result_flecs_pair(result_t *result, char const *pre0, char const *a0, char const *pre1, char const *a1)
{
	result_indent(result);
	fprintf(result->file, "(%s.%s, %s.%s)\n", pre0, a0, pre1, a1);
}

void result_flecs_entity_open(result_t *result, const char *name, char const * extend)
{
	result_indent(result);
	if (extend) {
		fprintf(result->file, "%s : %s {\n", name, extend);
	} else {
		fprintf(result->file, "%s {\n", name);
	}
	result->ident++;
}

void result_flecs_entity_close(result_t *result)
{
	result->ident--;
	result_indent(result);
	fprintf(result->file, "}\n");
}

void result_flecs_peripheral(result_t *result)
{
	result_indent(result);
	fprintf(result->file, "%s\n", "ec.Peripheral : {0}");
}

void result_flecs_signal(result_t *result)
{
	result_indent(result);
	fprintf(result->file, "%s\n", "ec.Signal : {0}");
}
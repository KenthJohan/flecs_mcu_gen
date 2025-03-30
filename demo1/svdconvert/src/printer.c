#include "printer.h"
#include "str.h"

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

void result_flecs_register(result_t *result, const char *address)
{
	result_indent(result);
	fprintf(result->file, "ec.Register : {address:%s}\n", address);
}

void result_flecs_field(result_t *result, char const *bitoffset, char const *bitwidth)
{
	result_indent(result);
	fprintf(result->file, "ec.Field : {bitoffset:%s, bitwidth:%s}\n", bitoffset, bitwidth);
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
#include "printer.h"

void result_indent(result_t *result)
{
	for (int i = 0; i < result->ident; i++) {
		fprintf(result->file, "\t");
	}
}

void escape_string(char *dst, int size, const char *src)
{
	while (*src && size > 1) {
		switch (*src) {
		case '\n':
			*dst++ = '\\';
			*dst++ = 'n';
			break;
		case '\r':
			*dst++ = '\\';
			*dst++ = 'r';
			break;
		case '\t':
			*dst++ = '\\';
			*dst++ = 't';
			break;
		case '\\':
			*dst++ = '\\';
			*dst++ = '\\';
			break;
		case '\"':
			*dst++ = '\\';
			*dst++ = '\"';
			break;
		default:
			*dst++ = *src;
			break;
		}
		src++;
		size--;
	}
	*dst = 0;
}

void result_flecs_description(result_t *result, const char *description)
{
	char buf[256] = {0};
	escape_string(buf, sizeof(buf), description);
	result_indent(result);
	fprintf(result->file, "(flecs.doc.Description, flecs.doc.Brief) : {\"%s\"}\n", buf);
}

void result_flecs_register(result_t *result, uint32_t address)
{
	result_indent(result);
	fprintf(result->file, "Register : {address:%X}\n", address);
}

void result_flecs_field(result_t *result, char const *bitoffset, char const *bitwidth)
{
	result_indent(result);
	fprintf(result->file, "Field : {bitoffset:%s, bitwidth:%s}\n", bitoffset, bitwidth);
}

void result_flecs_pair(result_t *result, char const *pre0, char const *a0, char const *pre1, char const *a1)
{
	result_indent(result);
	fprintf(result->file, "(%s.%s, %s.%s)\n", pre0, a0, pre1, a1);
}

void result_flecs_entity_open(result_t *result, const char *name)
{
	result_indent(result);
	fprintf(result->file, "%s {\n", name);
	result->ident++;
}

void result_flecs_entity_close(result_t *result)
{
	result->ident--;
	result_indent(result);
	fprintf(result->file, "}\n");
}
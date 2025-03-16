/*
https://jsonformatter.org/xml-viewer

*/
#include <stdlib.h>
#include <stdio.h>
#include <flecs.h>
#include <flecs.h>
#include <libmxml4/mxml.h>

typedef struct {
	int ident;
	FILE * file;
} result_t;

void result_indent(result_t *result)
{
	for (int i = 0; i < result->ident; i++) {
		fprintf(result->file, "\t");
	}
}

void result_flecs_description(result_t * result, const char * description)
{
	result_indent(result);
	fprintf(result->file, "(flecs.doc.Description, flecs.doc.Brief) : {\"%s\"}\n", description);
}

void result_flecs_register(result_t * result, uint32_t address)
{
	result_indent(result);
	fprintf(result->file, "Register : {address:%X}\n", address);
}

void result_flecs_field(result_t * result, char const * bitoffset, char const * bitwidth)
{
	result_indent(result);
	fprintf(result->file, "Field : {bitoffset:%s, bitwidth:%s}\n", bitoffset, bitwidth);
}

void result_flecs_entity_open(result_t * result, const char * name)
{
	result_indent(result);
	fprintf(result->file, "%s {\n", name);
	result->ident++;
}

void result_flecs_entity_close(result_t * result)
{
	result->ident--;
	result_indent(result);
	fprintf(result->file, "}\n");
}


typedef struct {
	mxml_node_t * name;
	mxml_node_t * description;
	mxml_node_t * registers;
} svd_peripheral_t;

typedef struct {
	mxml_node_t * name;
	mxml_node_t * description;
	mxml_node_t * fields;
} svd_register_t;

typedef struct {
	mxml_node_t * name;
	mxml_node_t * description;
	mxml_node_t * bitOffset;
	mxml_node_t * bitWidth;
} svd_field_t;

void iterate_fields(mxml_node_t *node, mxml_node_t *top, result_t * result)
{
	mxml_node_t *child = mxmlGetFirstChild(node);
	while (child != NULL) {
		if (mxmlGetType(child) != MXML_TYPE_ELEMENT) {
			child = mxmlGetNextSibling(child);
			continue;
		}
		svd_field_t field;
		field.name = mxmlFindElement(child, top, "name", NULL, NULL, MXML_DESCEND_FIRST);
		field.description = mxmlFindElement(child, top, "description", NULL, NULL, MXML_DESCEND_FIRST);
		field.bitOffset = mxmlFindElement(child, top, "bitOffset", NULL, NULL, MXML_DESCEND_FIRST);
		field.bitWidth = mxmlFindElement(child, top, "bitWidth", NULL, NULL, MXML_DESCEND_FIRST);

		if (field.name) {
			result_flecs_entity_open(result, mxmlGetOpaque(field.name));
			if (field.description) {
				result_flecs_description(result, mxmlGetOpaque(field.description));
			}
			if (field.bitOffset && field.bitWidth) {
				result_flecs_field(result, mxmlGetOpaque(field.bitOffset), mxmlGetOpaque(field.bitWidth));
			}
			result_flecs_entity_close(result);
		}

		child = mxmlGetNextSibling(child);
	}
}

void iterate_registers(mxml_node_t *node, mxml_node_t *top, result_t * result)
{
	mxml_node_t *child = mxmlGetFirstChild(node);
	while (child != NULL) {
		if (mxmlGetType(child) != MXML_TYPE_ELEMENT) {
			child = mxmlGetNextSibling(child);
			continue;
		}
		svd_register_t regs;
		regs.name = mxmlFindElement(child, top, "name", NULL, NULL, MXML_DESCEND_FIRST);
		regs.description = mxmlFindElement(child, top, "description", NULL, NULL, MXML_DESCEND_FIRST);
		regs.fields = mxmlFindElement(child, top, "fields", NULL, NULL, MXML_DESCEND_FIRST);

		if (regs.name) {
			result_flecs_entity_open(result, mxmlGetOpaque(regs.name));
			if (regs.description) {
				result_flecs_description(result, mxmlGetOpaque(regs.description));
			}
			result_flecs_register(result, 0);
			if (regs.fields) {
				iterate_fields(regs.fields, top, result);
			}
			result_flecs_entity_close(result);
		}

		child = mxmlGetNextSibling(child);
	}
}



void iterate_peripherals(mxml_node_t *node, mxml_node_t *top, result_t * result)
{
	mxml_node_t *child = mxmlGetFirstChild(node);
	while (child != NULL) {
		if (mxmlGetType(child) != MXML_TYPE_ELEMENT) {
			child = mxmlGetNextSibling(child);
			continue;
		}
		svd_peripheral_t peripheral;
		peripheral.name = mxmlFindElement(child, top, "name", NULL, NULL, MXML_DESCEND_FIRST);
		peripheral.description = mxmlFindElement(child, top, "description", NULL, NULL, MXML_DESCEND_FIRST);

		mxmlElementSetAttr(child, "name", mxmlGetOpaque(peripheral.name));
		char const * a = mxmlElementGetAttr(child, "derivedFrom");
		if (a) {
			mxml_node_t * g = mxmlFindElement(top, top, "peripheral", "name", a, MXML_DESCEND_ALL);
			if (g) {
				printf("Derived from %s\n", mxmlGetOpaque(mxmlFindElement(g, top, "name", NULL, NULL, MXML_DESCEND_FIRST)));
				peripheral.registers = mxmlFindElement(g, top, "registers", NULL, NULL, MXML_DESCEND_FIRST);
				peripheral.description = mxmlFindElement(g, top, "description", NULL, NULL, MXML_DESCEND_FIRST);
			}
		} else {
			peripheral.registers = mxmlFindElement(child, top, "registers", NULL, NULL, MXML_DESCEND_FIRST);
		}

		if (peripheral.name) {
			result_flecs_entity_open(result, mxmlGetOpaque(peripheral.name));
			if (peripheral.description) {
				result_flecs_description(result, mxmlGetOpaque(peripheral.description));
			}
			if (peripheral.registers) {
				iterate_registers(peripheral.registers, top, result);
			}
			result_flecs_entity_close(result);
		}

		child = mxmlGetNextSibling(child);
	}
}

int main(int argc, char *argv[])
{

	mxml_node_t *tree;

	mxml_options_t *options = mxmlOptionsNew();
	mxmlOptionsSetTypeValue(options, MXML_TYPE_OPAQUE);
	tree = mxmlLoadFilename(NULL, options, "config/STM32G030.svd");
	mxml_node_t *node = tree;

	

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

	node = mxmlFindElement(node, tree, "peripherals", NULL, NULL, MXML_DESCEND_ALL);
	iterate_peripherals(node, tree, &result);

	return tree != NULL;
}

/*
https://jsonformatter.org/xml-viewer

*/
#include <stdlib.h>
#include <stdio.h>
#include <flecs.h>
#include <flecs.h>
#include <libmxml4/mxml.h>

void fprint_flecs_description(FILE * f, const char * prefix, const char * description)
{
	fprintf(f, "%s(flecs.doc.Description, flecs.doc.Brief) : {\"%s\"}\n", prefix, description);
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
} svd_field_t;

void iterate_fields(mxml_node_t *node, mxml_node_t *top, FILE* file, const char * prefix)
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
		fprintf(file, "%s%s {\n", prefix, mxmlGetOpaque(field.name));
		fprint_flecs_description(file, prefix, mxmlGetOpaque(field.description));
		fprintf(file, "%s}\n", prefix);
		child = mxmlGetNextSibling(child);
	}
}

void iterate_registers(mxml_node_t *node, mxml_node_t *top, FILE* file)
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
		fprintf(file, "%s {\n", mxmlGetOpaque(regs.name));
		fprint_flecs_description(file, "\t", mxmlGetOpaque(regs.description));
		iterate_fields(regs.fields, top, file, "\t");
		fprintf(file, "}\n\n");
		child = mxmlGetNextSibling(child);
	}
}



void iterate_peripherals(mxml_node_t *node, mxml_node_t *top, FILE* file)
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
		peripheral.registers = mxmlFindElement(child, top, "registers", NULL, NULL, MXML_DESCEND_FIRST);
		fprintf(file, "%s {\n", mxmlGetOpaque(peripheral.name));
		fprint_flecs_description(file, "\t", mxmlGetOpaque(peripheral.description));
		fprintf(file, "}\n");
		fprintf(file, "module %s\n", mxmlGetOpaque(peripheral.name));
		iterate_registers(peripheral.registers, top, file);
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

	FILE *file = fopen("output.flecs", "w");
	if (file == NULL) {
		printf("Error opening file!\n");
		exit(1);
	}

	node = mxmlFindElement(node, tree, "peripherals", NULL, NULL, MXML_DESCEND_ALL);
	iterate_peripherals(node, tree, file);

	return tree != NULL;
}

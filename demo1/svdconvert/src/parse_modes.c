#include "parse_modes.h"
/*
https://jsonformatter.org/xml-viewer

*/
#include <stdlib.h>
#include <stdio.h>
#include <flecs.h>
#include <flecs.h>
#include <libmxml4/mxml.h>
#include "parse_svd.h"


typedef struct {
	int ident;
	FILE * file;
} result_t;

typedef struct {
	mxml_node_t * pinsignal;
} gpio_pin_t;

char const * get_af_str(mxml_node_t *node, mxml_node_t *top)
{
	node = mxmlFindElement(node, top, "SpecificParameter", "Name", "GPIO_AF", MXML_DESCEND_ALL);
	if (node == NULL) {
		return "";
	}
	node = mxmlFindElement(node, top, "PossibleValue", NULL, NULL, MXML_DESCEND_ALL);
	if (node == NULL) {
		return "";
	}
	return mxmlGetOpaque(node);
}

void iterate_sigs(mxml_node_t *node, mxml_node_t *top, result_t * result)
{
	while(1) {
		node = mxmlFindElement(node, top, "PinSignal", NULL, NULL, MXML_DESCEND_NONE);
		if (node == NULL) {
			break;
		}
		fprintf(result->file, "PinSignal-Name: %s, %s\n", mxmlElementGetAttr(node, "Name"), get_af_str(node, top));
	}
}

void iterate_pins(mxml_node_t *node, mxml_node_t *top, result_t * result)
{
	while(1) {
		node = mxmlFindElement(node, top, "GPIO_Pin", NULL, NULL, MXML_DESCEND_NONE);
		if (node == NULL) {
			break;
		}
		fprintf(result->file, "PinSignal: %s\n", mxmlElementGetAttr(node, "Name"));
		iterate_sigs(mxmlGetFirstChild(node), top, result);
	}
}

int parse_modes_init()
{

	mxml_node_t *tree;

	mxml_options_t *options = mxmlOptionsNew();
	mxmlOptionsSetTypeValue(options, MXML_TYPE_OPAQUE);
	tree = mxmlLoadFilename(NULL, options, "config/GPIO-STM32G03x_gpio_v1_0_Modes.xml");
	mxml_node_t *node = tree;

	
	result_t result = {0, NULL};
	result.file = fopen("../example1/config/STM32G030_modes.flecs", "w");
	if (result.file == NULL) {
		printf("Error opening file!\n");
		exit(1);
	}

	fprintf(result.file, "@color #AA99AF\n");
	fprintf(result.file, "xmcu {}\n");
	fprintf(result.file, "module xmcu\n");
	fprintf(result.file, "module STM32G030\n\n");

	
	node = mxmlFindElement(node, tree, "IP", NULL, NULL, MXML_DESCEND_ALL);
	iterate_pins(mxmlGetFirstChild(node), tree, &result);

	return tree != NULL;
}

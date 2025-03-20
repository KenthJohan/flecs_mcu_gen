#include "parse_modes.h"
/*
https://jsonformatter.org/xml-viewer

*/
#include <stdlib.h>
#include <stdio.h>
#include <flecs.h>
#include <flecs.h>
#include <libmxml4/mxml.h>
#include "printer.h"




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

/*
void result_print_af(result_t * result, char const * af)
{
	char buf[256] = {0};
	if (strncmp(af, "GPIO_", 5) != 0) {
		return;
	}
	af += 5;
	char * e = strchr(af, '_');
	if (e == NULL) {
		return;
	}
	*e = 0;
	sprintf(buf, "%s", af);
	fprintf(result->file, "%s", buf);
}
*/

void copy_af_string(char *dst, int size, char const *src)
{
	if (strncmp(src, "GPIO_", 5) != 0) {
		return;
	}
	src += 5;
	while(*src && size > 1) {
		if (*src == '_') {
			*dst = 0;
			break;
		}
		*dst++ = *src++;
	}
	*dst = 0;
}

void iterate_sigs(mxml_node_t *node, mxml_node_t *top, result_t * result)
{
	while(1) {
		node = mxmlFindElement(node, top, "PinSignal", NULL, NULL, MXML_DESCEND_NONE);
		if (node == NULL) {
			break;
		}
		char buf[256] = {0};
		copy_af_string(buf, sizeof(buf), get_af_str(node, top));
		result_flecs_pair(result, "AF", buf, "signals", mxmlElementGetAttr(node, "Name"));
	}
}

void copy_alphanumeric(char *dst, char const *src)
{
	while(*src) {
		if (isalnum(*src)) {
			*dst++ = *src;
		}else {
			*dst++ = '_';
		}
		src++;
	}
	*dst = 0;
}

void iterate_pins(mxml_node_t *node, mxml_node_t *top, result_t * result)
{
	while(1) {
		node = mxmlFindElement(node, top, "GPIO_Pin", NULL, NULL, MXML_DESCEND_NONE);
		if (node == NULL) {
			break;
		}
		char buf[256] = {0};
		copy_alphanumeric(buf, mxmlElementGetAttr(node, "Name"));
		result_flecs_entity_open(result, buf);
		iterate_sigs(mxmlGetFirstChild(node), top, result);
		result_flecs_entity_close(result);
	}
}

int parse_modes_init(result_t *result)
{
	mxml_node_t *tree;
	mxml_options_t *options = mxmlOptionsNew();
	mxmlOptionsSetTypeValue(options, MXML_TYPE_OPAQUE);
	tree = mxmlLoadFilename(NULL, options, "config/GPIO-STM32G03x_gpio_v1_0_Modes.xml");
	mxml_node_t *node = tree;
	node = mxmlFindElement(node, tree, "IP", NULL, NULL, MXML_DESCEND_ALL);
	iterate_pins(mxmlGetFirstChild(node), tree, result);
	return tree != NULL;
}

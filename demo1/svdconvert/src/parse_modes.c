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
#include "str.h"

typedef struct {
	mxml_node_t *pinsignal;
} gpio_pin_t;

char const *get_af_str(mxml_node_t *node, mxml_node_t *top)
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

void iterate_sigs(mxml_node_t *node, mxml_node_t *top, result_t *result)
{
	while (1) {
		node = mxmlFindElement(node, top, "PinSignal", NULL, NULL, MXML_DESCEND_NONE);
		if (node == NULL) {
			break;
		}
		char buf[256] = {0};
		str_copy_gpioaf(buf, sizeof(buf), get_af_str(node, top));
		char const *signame = mxmlElementGetAttr(node, "Name");
		result_flecs_pair(result, "AF", buf, "signals", signame);
	}
}

void iterate_sigs1(ecs_world_t *world, ecs_entity_t base, mxml_node_t *node, mxml_node_t *top, result_t *result)
{
	while (1) {
		node = mxmlFindElement(node, top, "PinSignal", NULL, NULL, MXML_DESCEND_NONE);
		if (node == NULL) {
			break;
		}
		char const *signame = mxmlElementGetAttr(node, "Name");
		ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){.name = signame, .add = (ecs_id_t[]){base, 0}});
	}
}

void iterate_pins1(ecs_world_t *world, mxml_node_t *node, mxml_node_t *top, result_t *result)
{
	ecs_entity_t base = ecs_new(world);
	mxml_node_t *node1 = node;
	while (1) {
		node1 = mxmlFindElement(node1, top, "GPIO_Pin", NULL, NULL, MXML_DESCEND_NONE);
		if (node1 == NULL) {
			break;
		}
		iterate_sigs1(world, base, mxmlGetFirstChild(node1), top, result);
	}
	result_flecs_entity_open(result, "signals");
	ecs_query_t *q = ecs_query_init(world,
	&(ecs_query_desc_t){
	.terms = {
	{base},
	}});
	ecs_iter_t it = ecs_query_iter(world, q);
	while (ecs_query_next(&it)) {
		for (int i = 0; i < it.count; i++) {
			result_flecs_entity_open(result, ecs_get_name(world, it.entities[i]));
			result_flecs_entity_close(result);
			// printf("Found %s\n", ecs_get_name(world, it.entities[i]));
		}
	}
	ecs_query_fini(q);
	result_flecs_entity_close(result);
}

void iterate_pins2(ecs_world_t *world, mxml_node_t *node, mxml_node_t *top, result_t *result)
{
	result_flecs_entity_open(result, "pins");
	mxml_node_t *node1 = node;
	while (1) {
		node1 = mxmlFindElement(node1, top, "GPIO_Pin", NULL, NULL, MXML_DESCEND_NONE);
		if (node1 == NULL) {
			break;
		}
		char buf[256] = {0};
		str_copy_af(buf, mxmlElementGetAttr(node1, "Name"));
		result_flecs_entity_open(result, buf);
		iterate_sigs(mxmlGetFirstChild(node1), top, result);
		result_flecs_entity_close(result);
	}
	result_flecs_entity_close(result);
}

int parse_modes_init(ecs_world_t *world, result_t *result)
{
	mxml_node_t *tree;
	mxml_options_t *options = mxmlOptionsNew();
	mxmlOptionsSetTypeValue(options, MXML_TYPE_OPAQUE);
	tree = mxmlLoadFilename(NULL, options, "config/GPIO-STM32G03x_gpio_v1_0_Modes.xml");
	mxml_node_t *node = tree;
	node = mxmlFindElement(node, tree, "IP", NULL, NULL, MXML_DESCEND_ALL);
	iterate_pins1(world, mxmlGetFirstChild(node), tree, result);
	iterate_pins2(world, mxmlGetFirstChild(node), tree, result);
	return tree != NULL;
}

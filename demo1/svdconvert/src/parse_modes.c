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


/*
<PinSignal Name="I2S1_CK"> : node
	<SpecificParameter Name="GPIO_AF">
		<PossibleValue>(return this)</PossibleValue>
	</SpecificParameter>
</PinSignal>
*/
char const *node_extract_af(mxml_node_t *node, mxml_node_t *top)
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



void node_get_uniqie_signals(ecs_world_t *world, ecs_entity_t base, mxml_node_t *node, mxml_node_t *top, result_t *result)
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

void node_print_signals(ecs_world_t *world, mxml_node_t *node, mxml_node_t *top, result_t *result)
{
	ecs_entity_t base = ecs_new(world);
	mxml_node_t *node1 = node;
	while (1) {
		node1 = mxmlFindElement(node1, top, "GPIO_Pin", NULL, NULL, MXML_DESCEND_NONE);
		if (node1 == NULL) {
			break;
		}
		node_get_uniqie_signals(world, base, mxmlGetFirstChild(node1), top, result);
	}
	result_flecs_entity_open(result, "signals", NULL, NULL);
	ecs_query_t *q = ecs_query_init(world,
	&(ecs_query_desc_t){
	.terms = {
	{base},
	}});
	ecs_iter_t it = ecs_query_iter(world, q);
	while (ecs_query_next(&it)) {
		for (int i = 0; i < it.count; i++) {
			result_flecs_entity_open(result, ecs_get_name(world, it.entities[i]), NULL, NULL);
			result_flecs_signal(result);
			result_flecs_entity_close(result);
			// printf("Found %s\n", ecs_get_name(world, it.entities[i]));
		}
	}
	ecs_query_fini(q);
	result_flecs_entity_close(result);
}


/*
<PinSignal Name="EVENTOUT"></PinSignal> : node iteration 1
<PinSignal Name="I2C1_SMBA"></PinSignal> : node iteration 2
<PinSignal Name="I2S1_CK"></PinSignal> : node iteration 3
*/
void node_print_pins_af(mxml_node_t *node, mxml_node_t *top, result_t *result)
{
	while (1) {
		node = mxmlFindElement(node, top, "PinSignal", NULL, NULL, MXML_DESCEND_NONE);
		if (node == NULL) {
			break;
		}
		char const *signame = mxmlElementGetAttr(node, "Name");
		char buf[16] = {0};
		str_copy_gpioaf(buf, sizeof(buf), node_extract_af(node, top));
		ecs_strbuf_t name = ECS_STRBUF_INIT;
		ecs_strbuf_appendstr(&name, buf);
		ecs_strbuf_appendstr(&name, "_");
		ecs_strbuf_appendstr(&name, signame);
		ecs_strbuf_t extend = ECS_STRBUF_INIT;
		ecs_strbuf_appendstr(&extend, "signals.");
		ecs_strbuf_appendstr(&extend, signame);
		result_flecs_entity_open(result, ecs_strbuf_get(&name), ecs_strbuf_get(&extend), NULL);
		result_flecs_entity_close(result);
		ecs_os_free(ecs_strbuf_get(&name));
		ecs_os_free(ecs_strbuf_get(&extend));

		//result_flecs_pair(result, "ec.Af", buf, "signals", signame);
	}
}

void node_print_pins(ecs_world_t *world, mxml_node_t *node, mxml_node_t *top, result_t *result)
{
	result_flecs_entity_open(result, "pins", NULL, NULL);
	mxml_node_t *node1 = node;
	while (1) {
		node1 = mxmlFindElement(node1, top, "GPIO_Pin", NULL, NULL, MXML_DESCEND_NONE);
		if (node1 == NULL) {
			break;
		}
		char buf[256] = {0};
		str_copy_af(buf, mxmlElementGetAttr(node1, "Name"));
		result_flecs_entity_open(result, buf, NULL, NULL);
		result_indent(result);
		fprintf(result->file, "%s\n", "ec.Pin : {}");
		node_print_pins_af(mxmlGetFirstChild(node1), top, result);
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
	node_print_signals(world, mxmlGetFirstChild(node), tree, result);
	node_print_pins(world, mxmlGetFirstChild(node), tree, result);
	return tree != NULL;
}

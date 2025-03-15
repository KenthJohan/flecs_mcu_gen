#include <stdlib.h>
#include <stdio.h>
#include <flecs.h>
#include <flecs.h>
#include <libmxml4/mxml.h>

int main(int argc, char *argv[])
{

	mxml_node_t *tree;

	tree = mxmlLoadFilename(NULL, NULL, "config/STM32G030.svd");
	mxml_node_t *node = tree;

	do {
		node = mxmlFindElement(node, tree, "peripheral", NULL, NULL, MXML_DESCEND_ALL);
		if (node != NULL) {
			mxml_node_t * node_name = mxmlFindElement(node, tree, "name", NULL, NULL, MXML_DESCEND_FIRST);
			mxml_node_t * node_description = mxmlFindElement(node, tree, "description", NULL, NULL, MXML_DESCEND_FIRST);
			printf("Peripheral: %s\n", mxmlGetText(node_name, NULL));
			printf("description: %s\n", mxmlGetText(node_description, NULL));

			
		}
	} while (node != NULL);

	return tree != NULL;
}

/*
https://jsonformatter.org/xml-viewer

*/
#include <stdlib.h>
#include <stdio.h>
#include <mxml.h>

int test(char const * filename)
{
	mxml_node_t *tree;
	mxml_options_t *options = mxmlOptionsNew();
	mxmlOptionsSetTypeValue(options, MXML_TYPE_TEXT);
	tree = mxmlLoadFilename(NULL, options, filename);
	printf("Loaded XML file '%s' %s\n", filename, tree != NULL ? "successfully" : "with errors");
	mxml_node_t *node = tree;
	return tree != NULL;
}

int main(int argc, char *argv[])
{
	test("hej.xml");
	return 0;
}

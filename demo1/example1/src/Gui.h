#pragma once

#include <flecs.h>

typedef struct {
	int32_t dummy;
} GuiWindow;

typedef enum {
	GuiTypeUnknown,
	GuiTypeWindow,
	GuiTypeTabs,
	GuiTypeTab,
	GuiTypeTreeNode,
	GuiTypeText,
	GuiTypeNodeTreeReflection,
	GuiTypeColumnComponent,
	GuiTypeInputText
} GuiType;

typedef struct {
	GuiType type;
	ecs_entity_t storage;
	uint32_t id;
} GuiElement;

typedef struct {
	ecs_query_t *query;
} GuiQuery;

typedef struct {
	float r;
	float g;
	float b;
} GuiColor3;

typedef struct {
	char *value; // Pointer to external memory
} GuiString;

typedef struct {
	int32_t table_dummy;
} GuiTable;

typedef struct {
	ecs_entity_t members[4];
} GuiColumn;

typedef struct {
	int8_t indices[16];
} GuiFields;

typedef struct {
	int8_t index;
} GuiField;

typedef struct {
	GuiColumn column;
	GuiColor3 color;
} GuiTest;

// Resource management hooks. The convenience macros hide details of
// the callback signature, while allowing hooks to be called on multiple
// entities.

extern ECS_COMPONENT_DECLARE(GuiString);
extern ECS_COMPONENT_DECLARE(GuiWindow);
extern ECS_COMPONENT_DECLARE(GuiType);
extern ECS_COMPONENT_DECLARE(GuiElement);
extern ECS_COMPONENT_DECLARE(GuiColor3);
extern ECS_COMPONENT_DECLARE(GuiQuery);
extern ECS_COMPONENT_DECLARE(GuiTable);
extern ECS_COMPONENT_DECLARE(GuiColumn);
extern ECS_COMPONENT_DECLARE(GuiFields);
extern ECS_COMPONENT_DECLARE(GuiField);
extern ECS_COMPONENT_DECLARE(GuiTest);
extern ECS_TAG_DECLARE(GuiDebugIdUnit);

void GuiImport(ecs_world_t *world);
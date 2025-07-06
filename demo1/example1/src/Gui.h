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
	ecs_entity_t members[8];
	uint32_t id; // Unique ID for the element, used for ImGui
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
	ecs_vec_t columns; //ecs_vec_t<GuiQueryColumn> columns
} GuiTable;

typedef struct {
	ecs_u32_t field;
	ecs_entity_t type;
	ecs_u32_t offset;
} GuiQueryColumn;

typedef struct {
	int8_t indices[16];
} GuiFields;

typedef struct {
	int8_t index;
	ecs_entity_t members[4];
} GuiField;

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
extern ECS_COMPONENT_DECLARE(GuiQueryColumn);
extern ECS_TAG_DECLARE(GuiDebugIdUnit);

void GuiImport(ecs_world_t *world);
#pragma once

#include <flecs.h>

typedef struct {
	int32_t dummy;
} GuiWindow;

typedef struct {
	ecs_entity_t member;
	ecs_entity_t unit;
} GuiColumnComponent;


typedef enum {
	GuiTypeUnknown,
	GuiTypeWindow,
	GuiTypeTabs,
	GuiTypeTab,
	GuiTypeNodeTreeReflection,
	GuiTypeColumnComponent,
	GuiTypeInputText
} GuiType;

typedef struct {
	GuiType type;
	ecs_entity_t storage;
	uint32_t id;
} GuiElement;


extern ECS_COMPONENT_DECLARE(GuiWindow);
extern ECS_COMPONENT_DECLARE(GuiColumnComponent);
extern ECS_COMPONENT_DECLARE(GuiType);
extern ECS_COMPONENT_DECLARE(GuiElement);
extern ECS_TAG_DECLARE(GuiDebugIdUnit);


void GuiImport(ecs_world_t *world);
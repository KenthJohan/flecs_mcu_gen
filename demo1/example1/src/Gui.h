#pragma once

#include <flecs.h>

typedef struct {
	int32_t dummy;
} GuiWindow;

typedef struct {
	int32_t dummy;
} GuiTabs;

typedef struct {
	int32_t dummy;
} GuiTab;

typedef struct {
	int32_t dummy;
} GuiNodeTreeReflection;

typedef struct {
	int32_t dummy;
} GuiColumnComponent;


extern ECS_COMPONENT_DECLARE(GuiWindow);
extern ECS_COMPONENT_DECLARE(GuiTabs);
extern ECS_COMPONENT_DECLARE(GuiTab);
extern ECS_COMPONENT_DECLARE(GuiNodeTreeReflection);
extern ECS_COMPONENT_DECLARE(GuiColumnComponent);

void GuiImport(ecs_world_t *world);
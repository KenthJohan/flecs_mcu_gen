#include <stdlib.h>
#include <stdio.h>
#include <flecs.h>
#include <jmgui.h>
#include "Ec.h"
#include "eg_reparent.h"
#include "bgui.h"
#include "Gui.h"
#include "ecs0.h"

static void SystemGuiWindow2(ecs_world_t *world, ecs_entity_t e);

static void SystemGuiWindow1(ecs_world_t *world, ecs_entity_t parent)
{
	ecs_iter_t it = ecs_children(world, parent);
	while (ecs_children_next(&it)) {
		for (int i = 0; i < it.count; i++) {
			ecs_entity_t e = it.entities[i];
			SystemGuiWindow2(world, e);
		}
	}
}


static int get_columns(ecs_world_t *world, ecs_entity_t parent, ecs_entity_t columns[], int n)
{
	ecs_iter_t it = ecs_children(world, parent);
	int k = 0;
	while (ecs_children_next(&it)) {
		for (int i = 0; i < it.count; i++) {
			ecs_entity_t e = it.entities[i];
			GuiColumnComponent const *col = ecs_get(world, e, GuiColumnComponent);
			if (col == NULL) {
				continue;
			}
			if (k < n) {
				columns[k] = e;
				k++;
			}
		}
	}
	return k;
}

static void SystemGuiWindow2(ecs_world_t *world, ecs_entity_t e)
{
	char const *name = ecs_get_name(world, e);
	if (!name) {
		return;
	}
	GuiElement *el = ecs_get_mut(world, e, GuiElement);
	el->id = jmgui_get_id_by_string(name);

	switch (el->type) {
	case GuiTypeTabs:
		if (jmgui_tab_begin(name, 0)) {
			SystemGuiWindow1(world, e);
			jmgui_tab_end();
		}
		break;
	case GuiTypeTab:
		if (jmgui_tab_item_begin(name, 0)) {
			SystemGuiWindow1(world, e);
			jmgui_tab_item_end();
		}
		break;
	case GuiTypeInputText:
		if (1) {
			char buf[128] = {0};
			if (el == NULL) {
				return;
			}
			const EcsDocDescription *ptr = ecs_get(world, el->storage, EcsDocDescription);
			if (ptr == NULL) {
				return;
			}
			if (el->storage == 0) {
				return;
			}
			ecs_os_strncpy(buf, ptr->value, sizeof(buf));
			if (jmgui_input_text(name, buf, sizeof(buf))) {
				/* Safe, value gets copied by copy hook */
				ecs_set(world, el->storage, EcsDocDescription, {.value = ECS_CONST_CAST(char *, buf)});
			}
			el->id = jmgui_get_id_by_string(name);
		}
		break;
	case GuiTypeNodeTreeReflection:
		if (1) {
			ecs_entity_t columns[8] = {0};
			int k = ecs0_get_entities_from_parent(world, e, ecs_id(GuiColumnComponent), columns, 8-1);

			/*
			char buf[1288] = {0};
			char * path = ecs_get_path(world, e);
			snprintf(buf, sizeof(buf), "path: %s, cols: %i", path, columns);
			ecs_os_free(path);
			jmgui_text(buf);
			*/

			jmgui_table_begin(name, k+1, 0);
			el->id = jmgui_get_id_by_string(name);
			jmgui_table_setup_column("Name", 128, 0);
			bgui_ntt_reflection(world, el->storage, columns);
			jmgui_table_end();
		}
		break;

	default:
		break;
	}
}



static void SystemGuiWindow(ecs_iter_t *it)
{
	//GuiWindow *win = ecs_field(it, GuiWindow, 0);
	for (int i = 0; i < it->count; ++i) {
		ecs_defer_begin(it->world);
		ecs_entity_t e = it->entities[i];
		char const *name = ecs_get_name(it->world, e);
		if (!name) {
			continue;
		}
		if (jmgui_begin(name, NULL)) {
			SystemGuiWindow1(it->world, e);
		}
		jmgui_end();
		ecs_defer_end(it->world);
	}
}

int main(int argc, char *argv[])
{

	ecs_os_set_api_defaults();
	ecs_os_api_t os_api = ecs_os_get_api();
	ecs_os_set_api(&os_api);

	ecs_world_t *world = ecs_init();
	ECS_IMPORT(world, FlecsUnits);
	ECS_IMPORT(world, FlecsDoc);
	ECS_IMPORT(world, FlecsStats);

	ECS_IMPORT(world, Ec);
	ECS_IMPORT(world, Gui);

	ecs_set(world, EcsWorld, EcsRest, {.port = 0});
	printf("Remote: %s\n", "https://www.flecs.dev/explorer/?remote=true");


	ecs_log_set_level(0);
	ecs_script_run_file(world, "config/STM32G030.flecs");
	ecs_log_set_level(-1);

	{
		char const *names[] = {"DMA", "GPIO", "TIM", "SPI", "I2C", "USART", NULL};
		eg_reparent_by_subname1(world, names, ecs_id(EcPeripheral));
	}

	{
		char const *names[] = {"DMA", "GPIO", "TIM", "SPI", "I2C", "USART", "LPUART", "LPTIM", "I2S", "RCC", "SYS", NULL};
		eg_reparent_by_subname1(world, names, ecs_id(EcSignal));
	}

	{
		char const *names[] = {"USART*", "TIM*", "I2S*", "LPTIM*", "SPI*", "LPUART*", "I2C*", NULL};
		eg_reparent_by_subname1(world, names, ecs_id(EcSignal));
	}

	{
		char const *names[] = {"PA", "PB", "PC", "PD", "PF", NULL};
		eg_reparent_by_subname1(world, names, ecs_id(EcPin));
	}
	
	ecs_log_set_level(0);
	ecs_script_run_file(world, "config/script1.flecs");
	ecs_log_set_level(-1);

	jmgui_context_t jmgui = {.clear_color = {0.45f, 0.55f, 0.60f, 1.00f}};
	jmgui_context_init(&jmgui);

	ecs_entity_t parent = ecs_lookup(world, "xmcu.STM32G030");
	// print name of parent
	if (parent) {
		printf("Parent: %s\n", ecs_get_name(world, parent));
	} else {
		printf("Parent not found\n");
	}

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "SystemGuiWindow", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = SystemGuiWindow,
	.query.terms = {
	{.id = ecs_id(GuiWindow), .src.id = EcsSelf},
	}});

	while (!jmgui.done) {
		jmgui_frame_begin(&jmgui);
		ecs_progress(world, 0.0f);
		jmgui_frame_frame(&jmgui);
		ecs_sleepf(0.016f);
	}
	jmgui_context_fini(&jmgui);

	return 0;
}

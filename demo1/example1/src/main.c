#include <stdlib.h>
#include <stdio.h>
#include <flecs.h>
#include <eximgui.h>
#include "Ec.h"
#include "eg_reparent.h"
#include "bgui.h"
#include "Gui.h"

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

static void SystemGuiWindow2(ecs_world_t *world, ecs_entity_t e)
{
	char const *name = ecs_get_name(world, e);
	if (!name) {
		return;
	}
	GuiElement const *el = ecs_get(world, e, GuiElement);

	switch (el->type) {
	case GuiTypeTabs:
		if (gui_tab_begin(name, 0)) {
			char buf[128] = {0};
			int r = ecs_os_snprintf(buf, sizeof(buf), "\nElement id: 0x%0x\n\n", gui_get_id_by_string(name));
			gui_debug_locate(buf, buf + r);
			SystemGuiWindow1(world, e);
			gui_tab_end();
		}
		break;
	case GuiTypeTab:
		if (gui_tab_item_begin(name, 0)) {
			SystemGuiWindow1(world, e);
			gui_tab_item_end();
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
			if (gui_input_text(name, buf, sizeof(buf))) {
				/* Safe, value gets copied by copy hook */
				ecs_set(world, el->storage, EcsDocDescription, {.value = ECS_CONST_CAST(char *, buf)});
			}
		}
		break;
	case GuiTypeNodeTreeReflection:
		if (1) {
			gui_dummy(0, 32);
			ecs_entity_t components[8] = {0};
			int k = 0;
			const ecs_type_t *type = ecs_get_type(world, e);
			for (int j = 0; j < type->count; j++) {
				if (ECS_HAS_ID_FLAG(type->array[j], PAIR)) {
					ecs_entity_t rel = ecs_pair_first(world, type->array[j]);
					if ((rel == ecs_id(GuiColumnComponent)) && (k < 8)) {
						ecs_entity_t tgt = ecs_pair_second(world, type->array[j]);
						components[k] = tgt;
						k++;
					}
					// printf("rel: %s, tgt: %s", ecs_get_name(world, rel), ecs_get_name(world, tgt));
				}
			}
			gui_ntt_reflection(world, el->storage, components);
		}
		break;

	default:
		break;
	}
}



static void SystemGuiWindow(ecs_iter_t *it)
{
	GuiWindow *win = ecs_field(it, GuiWindow, 0);
	for (int i = 0; i < it->count; ++i) {
		ecs_entity_t e = it->entities[i];
		char const *name = ecs_get_name(it->world, e);
		if (!name) {
			continue;
		}
		if (gui_begin(name, NULL)) {
			SystemGuiWindow1(it->world, e);
		}
		gui_end();
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

	eximgui_t eximgui = {.clear_color = {0.45f, 0.55f, 0.60f, 1.00f}};
	eximgui_init(&eximgui);

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

	while (!eximgui.done) {
		eximgui_begin_frame(&eximgui);
		ecs_progress(world, 0.0f);
		eximgui_end_frame(&eximgui);
		ecs_sleepf(0.016f);
	}
	eximgui_fini(&eximgui);

	return 0;
}

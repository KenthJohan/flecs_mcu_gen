#include <stdlib.h>
#include <stdio.h>
#include <flecs.h>
#include <eximgui.h>
#include "Ec.h"
#include "eg_reparent.h"

static void Sys_GuiListPins(ecs_iter_t *it)
{
	EcPin *pin = ecs_field(it, EcPin, 0); // self, in
	gui_begin("Pins", NULL);
	for (int i = 0; i < it->count; ++i, ++pin) {
		gui_text(ecs_get_name(it->world, it->entities[i]));
	}
	gui_end();
}

static void Sys_GuiListFields(ecs_iter_t *it, eximgui_t *eximgui)
{
	// eximgui_t *eximgui = it->ctx;
	EcField *field = ecs_field(it, EcField, 0); // self, in
	for (int i = 0; i < it->count; ++i, ++field) {
		gui_text(ecs_get_name(it->world, it->entities[i]));
	}
}

static void Sys_GuiListRegisters(ecs_iter_t *it, eximgui_t *eximgui)
{
	EcRegister *reg = ecs_field(it, EcRegister, 0); // self, in
	for (int i = 0; i < it->count; ++i, ++reg) {
		bool a = gui_tree(ecs_get_name(it->world, it->entities[i]));
		if (a) {
			ecs_iter_t it2 = ecs_query_iter(it->world, eximgui->query2);
			ecs_iter_set_group(&it2, it->entities[i]);
			while (ecs_query_next(&it2)) {
				Sys_GuiListFields(&it2, eximgui);
			}
			gui_tree_pop();
		}
	}
}

static void Sys_GuiListPeripherals(ecs_iter_t *it)
{
	eximgui_t *eximgui = it->ctx;
	EcPeripheral *per = ecs_field(it, EcPeripheral, 0); // self, in
	gui_begin("Peripherals", NULL);
	for (int i = 0; i < it->count; ++i, ++per) {
		// gui_push_id_u64((uint64_t)it->entities[i]);
		bool a = gui_tree(ecs_get_name(it->world, it->entities[i]));
		gui_sameline();
		gui_push_id_u64(1);
		gui_button_small(ecs_get_name(it->world, it->entities[i]));
		gui_pop_id();
		// bool a = gui_collapsing_header(ecs_get_name(it->world, it->entities[i]));
		if (a) {
			ecs_iter_t it2 = ecs_query_iter(it->world, eximgui->query1);
			ecs_iter_set_group(&it2, it->entities[i]);
			while (ecs_query_next(&it2)) {
				Sys_GuiListRegisters(&it2, eximgui);
			}
			gui_tree_pop();
		}
		// gui_pop_id();
	}
	gui_end();
}

void draw_tree(ecs_world_t *world, ecs_entity_t parent, eximgui_t *ex)
{
	ecs_iter_t it = ecs_children(world, parent);
	while (ecs_children_next(&it)) {
		for (int i = 0; i < it.count; i++) {
			char const *name = ecs_get_name(world, it.entities[i]);
			if (!name) {
				continue;
			}
			gui_table_next_row(0);
			gui_table_next_column();
			/*
			char * path = ecs_get_path_w_sep(world, 0, it.entities[i], ".", NULL);
			ecs_os_free(path);
			*/
			bool a = gui_tree(name);

			char str1[32] = {};
			char str2[32] = {};
			char str3[32] = {};
			EcRegister const *reg = ecs_get(world, it.entities[i], EcRegister);
			if (reg) {
				snprintf(str1, sizeof(str1), "%d", reg->address);
			}
			EcField const *field = ecs_get(world, it.entities[i], EcField);
			if (field) {
				snprintf(str2, sizeof(str2), "%d", field->bitoffset);
				snprintf(str3, sizeof(str3), "%d", field->bitwidth);
			}

			gui_table_next_column();
			gui_text(str1);
			gui_table_next_column();
			gui_text(str2);
			gui_table_next_column();
			gui_text(str3);

			if (a) {
				draw_tree(world, it.entities[i], ex);
				gui_tree_pop();
			}
		}
	}
}

void draw_tree0(ecs_world_t *world, ecs_entity_t parent, eximgui_t *ex)
{
	gui_begin("Window1", NULL);
	if (gui_tab_begin("Peripherals", 0)) {
		if (gui_tab_item_begin("Peripherals", 0)) {
			gui_table_begin("Peripherals", 4, 0);
			gui_table_setup_column("Name", 128, 0);
			gui_table_setup_column("address", 16, 12.0f);
			gui_table_setup_column("bitoffset", 16, 12.0f);
			gui_table_setup_column("bitwidth", 16, 18.0f);
			gui_table_header_row();
			parent = ecs_lookup(world, "xmcu.STM32G030.peripherals");
			draw_tree(world, parent, ex);
			gui_table_end();
			gui_tab_item_end();
		}
		if (gui_tab_item_begin("Signals", 0)) {
			gui_table_begin("Peripherals", 4, 0);
			gui_table_setup_column("Name", 128, 0);
			gui_table_setup_column("address", 16, 12.0f);
			gui_table_setup_column("bitoffset", 16, 12.0f);
			gui_table_setup_column("bitwidth", 16, 18.0f);
			gui_table_header_row();
			parent = ecs_lookup(world, "xmcu.STM32G030.signals");
			draw_tree(world, parent, ex);
			gui_table_end();
			gui_tab_item_end();
		}
		if (gui_tab_item_begin("Pins", 0)) {
			gui_table_begin("Peripherals", 4, 0);
			gui_table_setup_column("Name", 128, 0);
			gui_table_setup_column("address", 16, 12.0f);
			gui_table_setup_column("bitoffset", 16, 12.0f);
			gui_table_setup_column("bitwidth", 16, 18.0f);
			gui_table_header_row();
			parent = ecs_lookup(world, "xmcu.STM32G030.pins");
			draw_tree(world, parent, ex);
			gui_table_end();
			gui_tab_item_end();
		}
		gui_tab_end();
	}
	gui_end();
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

	ecs_set(world, EcsWorld, EcsRest, {.port = 0});
	printf("Remote: %s\n", "https://www.flecs.dev/explorer/?remote=true");

	ecs_log_set_level(0);
	ecs_script_run_file(world, "config/script1.flecs");
	ecs_log_set_level(-1);

	ecs_log_set_level(0);
	ecs_script_run_file(world, "config/STM32G030.flecs");
	ecs_log_set_level(-1);

	{
		char const *names[] = {"DMA", "GPIO", "TIM", "SPI", "I2C", "USART", NULL};
		ecs_query_t *q = ecs_query(world,
		{
		.terms = {
		{.id = ecs_id(EcPeripheral)},
		},
		});
		eg_reparent_by_subname(world, names, q);
		ecs_query_fini(q);
	}

	{
		char const *names[] = {"DMA", "GPIO", "TIM", "SPI", "I2C", "USART", "LPUART", "LPTIM", "I2S", "RCC", "SYS", NULL};
		ecs_query_t *q = ecs_query(world,
		{
		.terms = {
		{.id = ecs_id(EcSignal)},
		},
		});
		eg_reparent_by_subname(world, names, q);
		ecs_query_fini(q);
	}

	{
		char const *names[] = {"PA", "PB", "PC", "PD", "PF", NULL};
		ecs_query_t *q = ecs_query(world,
		{
		.terms = {
		{.id = ecs_id(EcPin)},
		},
		});
		eg_reparent_by_subname(world, names, q);
		ecs_query_fini(q);
	}

	eximgui_t eximgui = {.clear_color = {0.45f, 0.55f, 0.60f, 1.00f}};
	eximgui_init(&eximgui);

	eximgui.query1 = ecs_query(world,
	{.terms = {
	 {.id = ecs_id(EcRegister), .inout = EcsIn}},
	.ctx = &eximgui,
	//.group_by_callback = group_by_relation,
	.group_by = EcsChildOf});

	eximgui.query2 = ecs_query(world,
	{.terms = {
	 {.id = ecs_id(EcField), .inout = EcsIn}},
	//.group_by_callback = group_by_relation,
	.group_by = EcsChildOf});

	eximgui.query3 = ecs_query(world,
	{.terms = {
	 {.id = ecs_id(EcPeripheral), .inout = EcsIn}},
	//.group_by_callback = group_by_relation,
	.group_by = EcsChildOf});

	/*
ecs_system(world,
{.entity = ecs_entity(world, {.name = "Sys_GuiListPeripherals", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
.callback = Sys_GuiListPeripherals,
.ctx = &eximgui,
.query.terms = {
{.id = ecs_id(EcPeripheral), .inout = EcsIn},
}});

ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Sys_GuiListPins", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = Sys_GuiListPins,
	.query.terms = {
	{.id = ecs_id(EcPin), .inout = EcsIn},
	}});
	*/

	ecs_entity_t parent = ecs_lookup(world, "xmcu.STM32G030");
	// print name of parent
	if (parent) {
		printf("Parent: %s\n", ecs_get_name(world, parent));
	} else {
		printf("Parent not found\n");
	}

	while (!eximgui.done) {
		eximgui_begin_frame(&eximgui);
		draw_tree0(world, parent, &eximgui);
		ecs_progress(world, 0.0f);
		eximgui_end_frame(&eximgui);
		ecs_sleepf(0.016f);
	}
	eximgui_fini(&eximgui);

	return 0;
}

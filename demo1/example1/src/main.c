#include <stdlib.h>
#include <stdio.h>
#include <flecs.h>
#include "Ec.h"
#include <eximgui.h>

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

static bool str_cmp_sub0(char const *a, char const *b)
{
	while (*a && *b) {
		if (*a != *b) {
			return false;
		}
		a++;
		b++;
	}
	return true;
}

static int str_cmp_sub0v(char const *a, char const *bv[])
{
	for (int i = 0; bv[i]; i++) {
		if (str_cmp_sub0(a, bv[i])) {
			return i;
		}
	}
	return -1;
}

static void Sys_ReorderEntitites(ecs_world_t *world, char const *names[])
{
	ecs_query_t *q = ecs_query(world,
	{
	.terms = {
	{.id = ecs_id(EcPeripheral)},
	},
	});
	ecs_iter_t it = ecs_query_iter(world, q);
	while (ecs_query_next(&it)) {
		const EcPeripheral *r = ecs_field(&it, EcPeripheral, 0);
		for (int i = 0; i < it.count; i++) {
			int j = str_cmp_sub0v(ecs_get_name(world, it.entities[i]), names);
			if (j < 0) {
				continue;
			}
			ecs_entity_t parent = ecs_get_parent(world, it.entities[i]);
			// Create a child of parent
			ecs_entity_t child = ecs_entity_init(world, &(ecs_entity_desc_t){
			                                            .name = names[j],
			                                            .parent = parent,
			                                            });

			ecs_add_pair(world, it.entities[i], EcsChildOf, child);

			// ecs_add_entity(world, parent, it.entities[i]);
			// ecs_remove_entity(world, parent, it.entities[i]);
			// printf("Reorder: %jX %s\n", child, ecs_get_name(world, child));
		}
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

	ecs_set(world, EcsWorld, EcsRest, {.port = 0});
	printf("Remote: %s\n", "https://www.flecs.dev/explorer/?remote=true");

	ecs_log_set_level(0);
	ecs_script_run_file(world, "config/script1.flecs");
	ecs_log_set_level(-1);

	ecs_log_set_level(0);
	ecs_script_run_file(world, "config/STM32G030.flecs");
	ecs_log_set_level(-1);

	// ecs_entity_t parent = ecs_lookup(world, "STM32G030");
	char const *names[] = {"DMA", "GPIO", "TIM", "SPI", "I2C", NULL};
	Sys_ReorderEntitites(world, names);

	eximgui_t eximgui = {.clear_color = {0.45f, 0.55f, 0.60f, 1.00f}};
	eximgui_init(&eximgui);

	eximgui.query1 = ecs_query(world,
	{.terms = {
	 {.id = ecs_id(EcRegister), .inout = EcsIn}},
	//.group_by_callback = group_by_relation,
	.group_by = EcsChildOf});

	eximgui.query2 = ecs_query(world,
	{.terms = {
	 {.id = ecs_id(EcField), .inout = EcsIn}},
	//.group_by_callback = group_by_relation,
	.group_by = EcsChildOf});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Sys_GuiListPins", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = Sys_GuiListPins,
	.query.terms = {
	{.id = ecs_id(EcPin), .inout = EcsIn},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Sys_GuiListPeripherals", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = Sys_GuiListPeripherals,
	.query.terms = {
	{.id = ecs_id(EcPeripheral), .inout = EcsIn},
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

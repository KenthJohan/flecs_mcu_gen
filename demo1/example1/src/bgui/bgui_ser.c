#include "bgui_ser.h"
#include "Ec.h"
#include <stdio.h>
#include "jmgui.h"
#include "../Gui.h"
#include <ecsx.h>
#include <inttypes.h>
#include "bgui_entlink.h"
#include "../IconsForkAwesome.h"


void bgui_ser_query_str(void * ptr)
{
	EcsPoly * p = ptr;
	ecs_query_t *q = p->poly;
	char * qs = ecs_query_str(q);
	jmgui_textf("%s", qs);
	ecs_os_free(qs);
}


void bgui_ser_init(ecs_world_t *world)
{
	ecs_entity_init(world, &(ecs_entity_desc_t){
		.name = "query_str",
		.parent = ecs_id(GuiDrawfx),
		.set = ecs_values( ecs_value_pair_2nd(EcsConstant, ecs_uptr_t, {(uintptr_t)bgui_ser_query_str}) ),
	});
}
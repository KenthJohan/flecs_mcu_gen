#include "bgui_entinfo.h"
#include "Ec.h"
#include <stdio.h>
#include "jmgui.h"
#include <ecsx.h>

enum ImGuiTreeNodeFlags_ {
	ImGuiTreeNodeFlags_None = 0,
	ImGuiTreeNodeFlags_Selected = 1 << 0,             // Draw as selected
	ImGuiTreeNodeFlags_Framed = 1 << 1,               // Draw frame with background (e.g. for CollapsingHeader)
	ImGuiTreeNodeFlags_AllowOverlap = 1 << 2,         // Hit testing to allow subsequent widgets to overlap this one
	ImGuiTreeNodeFlags_NoTreePushOnOpen = 1 << 3,     // Don't do a TreePush() when open (e.g. for CollapsingHeader) = no extra indent nor pushing on ID stack
	ImGuiTreeNodeFlags_NoAutoOpenOnLog = 1 << 4,      // Don't automatically and temporarily open node when Logging is active (by default logging will automatically open tree nodes)
	ImGuiTreeNodeFlags_DefaultOpen = 1 << 5,          // Default node to be open
	ImGuiTreeNodeFlags_OpenOnDoubleClick = 1 << 6,    // Open on double-click instead of simple click (default for multi-select unless any _OpenOnXXX behavior is set explicitly). Both behaviors may be combined.
	ImGuiTreeNodeFlags_OpenOnArrow = 1 << 7,          // Open when clicking on the arrow part (default for multi-select unless any _OpenOnXXX behavior is set explicitly). Both behaviors may be combined.
	ImGuiTreeNodeFlags_Leaf = 1 << 8,                 // No collapsing, no arrow (use as a convenience for leaf nodes).
	ImGuiTreeNodeFlags_Bullet = 1 << 9,               // Display a bullet instead of arrow. IMPORTANT: node can still be marked open/close if you don't set the _Leaf flag!
	ImGuiTreeNodeFlags_FramePadding = 1 << 10,        // Use FramePadding (even for an unframed text node) to vertically align text baseline to regular widget height. Equivalent to calling AlignTextToFramePadding() before the node.
	ImGuiTreeNodeFlags_SpanAvailWidth = 1 << 11,      // Extend hit box to the right-most edge, even if not framed. This is not the default in order to allow adding other items on the same line without using AllowOverlap mode.
	ImGuiTreeNodeFlags_SpanFullWidth = 1 << 12,       // Extend hit box to the left-most and right-most edges (cover the indent area).
	ImGuiTreeNodeFlags_SpanLabelWidth = 1 << 13,      // Narrow hit box + narrow hovering highlight, will only cover the label text.
	ImGuiTreeNodeFlags_SpanAllColumns = 1 << 14,      // Frame will span all columns of its container table (label will still fit in current column)
	ImGuiTreeNodeFlags_LabelSpanAllColumns = 1 << 15, // Label will span all columns of its container table
	// ImGuiTreeNodeFlags_NoScrollOnOpen     = 1 << 16,  // FIXME: TODO: Disable automatic scroll on TreePop() if node got just open and contents is not visible
	ImGuiTreeNodeFlags_NavLeftJumpsToParent = 1 << 17, // Nav: left arrow moves back to parent. This is processed in TreePop() when there's an unfullfilled Left nav request remaining.
	ImGuiTreeNodeFlags_CollapsingHeader = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_NoAutoOpenOnLog,

	// [EXPERIMENTAL] Draw lines connecting TreeNode hierarchy. Discuss in GitHub issue #2920.
	// Default value is pulled from style.TreeLinesFlags. May be overridden in TreeNode calls.
	ImGuiTreeNodeFlags_DrawLinesNone = 1 << 18,    // No lines drawn
	ImGuiTreeNodeFlags_DrawLinesFull = 1 << 19,    // Horizontal lines to child nodes. Vertical line drawn down to TreePop() position: cover full contents. Faster (for large trees).
	ImGuiTreeNodeFlags_DrawLinesToNodes = 1 << 20, // Horizontal lines to child nodes. Vertical line drawn down to bottom-most child node. Slower (for large trees).

#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
	ImGuiTreeNodeFlags_NavLeftJumpsBackHere = ImGuiTreeNodeFlags_NavLeftJumpsToParent, // Renamed in 1.92.0
	ImGuiTreeNodeFlags_SpanTextWidth = ImGuiTreeNodeFlags_SpanLabelWidth,              // Renamed in 1.90.7
	ImGuiTreeNodeFlags_AllowItemOverlap = ImGuiTreeNodeFlags_AllowOverlap,             // Renamed in 1.89.7
#endif
};

#define NODE_DEFAULT (ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_DefaultOpen)
#define NODE_LEAF (ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet)

bool bgui_entinfo_draw(ecs_world_t *world, ecs_entity_t type, void const *ptr)
{
	bool stack[16] = {false};
	int sp = -1;
	char const *name = ecs_get_name(world, type);
	jmgui_table_begin(name, 3, 0);
	jmgui_table_setup_column("name", 128, 0);
	jmgui_table_setup_column("type", 128, 0);
	jmgui_table_setup_column("value", 128, 0);
	jmgui_table_header_row();

	const EcsComponent *comp = ecs_get(world, type, EcsComponent);
	if (!comp) {
		char *path = ecs_get_path(world, type);
		ecs_err("cannot serialize to JSON, '%s' is not a component", path);
		ecs_os_free(path);
		return -1;
	}
	const EcsTypeSerializer *ser = ecs_get(world, type, EcsTypeSerializer);
	if (!ser) {
		char *path = ecs_get_path(world, type);
		ecs_err("cannot serialize to JSON, '%s' has no reflection data", path);
		ecs_os_free(path);
		return -1;
	}
	ecs_meta_type_op_t *ops = ecs_vec_first_t(&ser->ops, ecs_meta_type_op_t);
	int32_t count = ecs_vec_count(&ser->ops);
	for (int i = 0; i < count; ++i) {
		ecs_meta_type_op_t *op = ops + i;

		switch (op->kind) {
		case EcsOpPush:
			break;
		case EcsOpPop:
			break;
		case EcsOpArray:
		case EcsOpVector:
		case EcsOpScope:
		case EcsOpEnum:
		case EcsOpBitmask:
		case EcsOpPrimitive:
		case EcsOpBool:
		case EcsOpChar:
		case EcsOpByte:
		case EcsOpU8:
		case EcsOpU16:
		case EcsOpU32:
		case EcsOpU64:
		case EcsOpI8:
		case EcsOpI16:
		case EcsOpI32:
		case EcsOpI64:
		case EcsOpF32:
		case EcsOpF64:
		case EcsOpUPtr:
		case EcsOpIPtr:
		case EcsOpEntity:
		case EcsOpId:
		case EcsOpString:
		case EcsOpOpaque:
			break;
		default:
			break;
		}

		jmgui_table_next_row(0);
		{
			jmgui_table_next_column();
			char buf[128];
			snprintf(buf, sizeof(buf), "%i:%s", i, op->name ? op->name : "?");

			if (op->kind == EcsOpPush) {
				bool has_open = jmgui_tree_node(buf, NODE_DEFAULT, 1, 1, 1);
				sp++;
				stack[sp] = has_open;
				if (has_open) {
					printf("TREE %10s: %s, open:%s\n", ecsx_meta_type_op_kind_str(op->kind), buf, has_open ? "true" : "false");
				} else {
					printf("TREE %10s: %s, open:%s\n", ecsx_meta_type_op_kind_str(op->kind), buf, has_open ? "true" : "false");
					while (ops[i].kind != EcsOpPop) {
						i++;
					}
				}
			} else {
				jmgui_tree_node(buf, NODE_DEFAULT | NODE_LEAF, 1, 1, 1);
				printf("TREE %10s: %s\n", ecsx_meta_type_op_kind_str(op->kind), buf);
			}

			// printf("%s: op:%s, open:%s\n", buf, ecsx_meta_type_op_kind_str(op->kind), has_open ? "true" : "false");
		}

		{
			jmgui_table_next_column();
			jmgui_text(ecsx_meta_type_op_kind_str(op->kind));
		}

		{
			jmgui_table_next_column();
			jmgui_text("value");
		}

		if (op->kind == EcsOpPop) {
			if ((sp >= 0) && stack[sp]) {
				jmgui_tree_pop();
				printf("TREE %10s  sp:%i\n", ecsx_meta_type_op_kind_str(op->kind), sp);
			}
			sp--;
		}
	}

	jmgui_table_end();
	printf("\n");
	return false;
}

#pragma once

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
};
// Dear ImGui: standalone example application for SDL3 + SDL_GPU

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

// Important note to the reader who wish to integrate imgui_impl_sdlgpu3.cpp/.h in their own engine/app.
// - Unlike other backends, the user must call the function ImGui_ImplSDLGPU_PrepareDrawData() BEFORE issuing a SDL_GPURenderPass containing ImGui_ImplSDLGPU_RenderDrawData.
//   Calling the function is MANDATORY, otherwise the ImGui will not upload neither the vertex nor the index buffer for the GPU. See imgui_impl_sdlgpu3.cpp for more info.

#include "jmgui/jmgui_frame.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlgpu3.h"
#include <stdio.h>  // printf, fprintf
#include <stdlib.h> // abort
#include <SDL3/SDL.h>

// This example doesn't compile with Emscripten yet! Awaiting SDL3 support.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

void jmgui_frame_begin(jmgui_context_t *eximgui)
{
	// Poll and handle events (inputs, window resize, etc.)
	// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
	// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
	// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
	// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
	// [If using SDL_MAIN_USE_CALLBACKS: call ImGui_ImplSDL3_ProcessEvent() from your SDL_AppEvent() function]
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		ImGui_ImplSDL3_ProcessEvent(&event);
		if (event.type == SDL_EVENT_QUIT)
			eximgui->done = true;
		if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(eximgui->window))
			eximgui->done = true;
	}

	// [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppIterate() function]
	if (SDL_GetWindowFlags(eximgui->window) & SDL_WINDOW_MINIMIZED) {
		SDL_Delay(10);
		return;
	}

	// Start the Dear ImGui frame
	ImGui_ImplSDLGPU3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (eximgui->show_demo_window)
		ImGui::ShowDemoWindow(&eximgui->show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");                   // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &eximgui->show_demo_window); // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &eximgui->show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit4("clear color", eximgui->clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGuiIO *io = &ImGui::GetIO();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (eximgui->show_another_window) {
		ImGui::Begin("Another Window", &eximgui->show_another_window); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			eximgui->show_another_window = false;
		ImGui::End();
	}
}

void jmgui_frame_frame(jmgui_context_t *eximgui)
{
	// Rendering
	ImGui::Render();
	ImDrawData *draw_data = ImGui::GetDrawData();
	const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);

	SDL_GPUCommandBuffer *command_buffer = SDL_AcquireGPUCommandBuffer(eximgui->gpu_device); // Acquire a GPU command buffer

	SDL_GPUTexture *swapchain_texture;
	SDL_AcquireGPUSwapchainTexture(command_buffer, eximgui->window, &swapchain_texture, nullptr, nullptr); // Acquire a swapchain texture

	if (swapchain_texture != nullptr && !is_minimized) {
		// This is mandatory: call ImGui_ImplSDLGPU3_PrepareDrawData() to upload the vertex/index buffer!
		ImGui_ImplSDLGPU3_PrepareDrawData(draw_data, command_buffer);

		// Setup and start a render pass
		SDL_GPUColorTargetInfo target_info = {};
		target_info.texture = swapchain_texture;
		target_info.clear_color = SDL_FColor{eximgui->clear_color[0], eximgui->clear_color[1], eximgui->clear_color[2], eximgui->clear_color[3]};
		target_info.load_op = SDL_GPU_LOADOP_CLEAR;
		target_info.store_op = SDL_GPU_STOREOP_STORE;
		target_info.mip_level = 0;
		target_info.layer_or_depth_plane = 0;
		target_info.cycle = false;
		SDL_GPURenderPass *render_pass = SDL_BeginGPURenderPass(command_buffer, &target_info, 1, nullptr);

		// Render ImGui
		ImGui_ImplSDLGPU3_RenderDrawData(draw_data, command_buffer, render_pass);

		SDL_EndGPURenderPass(render_pass);
	}

	// Submit the command buffer
	SDL_SubmitGPUCommandBuffer(command_buffer);
}


#include "jmgui/jmgui_context.h"
#include <SDL3/SDL.h>
#include <stdio.h> // printf, fprintf
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlgpu3.h"

int jmgui_context_init(jmgui_context_t *eximgui, char const * title, int w, int h)
{
	// Setup SDL
	// [If using SDL_MAIN_USE_CALLBACKS: all code below until the main loop starts would likely be your SDL_AppInit() function]
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
		printf("Error: SDL_Init(): %s\n", SDL_GetError());
		return -1;
	}

	// Create SDL window graphics context
	eximgui->window = SDL_CreateWindow(title, w, h, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
	if (eximgui->window == nullptr) {
		printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
		return -1;
	}

	// Create GPU Device
	eximgui->gpu_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_METALLIB, true, nullptr);
	if (eximgui->gpu_device == nullptr) {
		printf("Error: SDL_CreateGPUDevice(): %s\n", SDL_GetError());
		return -1;
	}

	// Claim window for GPU Device
	if (!SDL_ClaimWindowForGPUDevice(eximgui->gpu_device, eximgui->window)) {
		printf("Error: SDL_ClaimWindowForGPUDevice(): %s\n", SDL_GetError());
		return -1;
	}
	SDL_SetGPUSwapchainParameters(eximgui->gpu_device, eximgui->window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_MAILBOX);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO *io = &ImGui::GetIO();
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	// io.Fonts->AddFontDefault();
	ImFont *font = io->Fonts->AddFontFromFileTTF("/usr/share/fonts/truetype/ubuntu/Ubuntu-L.ttf", 22.0f);
	// io->Fonts->Build();
	//  io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//  io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//  io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//  ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
	IM_ASSERT(font != nullptr);

	// https://github.com/ocornut/imgui/blob/master/docs/FONTS.md#excluding-overlapping-ranges
	ImFontConfig cfg2;
	cfg2.MergeMode = true;
	io->Fonts->AddFontFromFileTTF("config/forkawesome-webfont.ttf", 0.0f, &cfg2);

	/*
	//static ImWchar ranges[] = { 0x1, UINT16_C(0x1FFFF), 0 };
	static ImFontConfig cfg = {};
	cfg.MergeMode = true;
	cfg.FontLoaderFlags |= 0;
	io->Fonts->AddFontFromFileTTF("/usr/share/fonts/truetype/noto/NotoColorEmoji.ttf", 16.0f, &cfg);
	*/

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL3_InitForSDLGPU(eximgui->window);
	ImGui_ImplSDLGPU3_InitInfo init_info = {};
	init_info.Device = eximgui->gpu_device;
	init_info.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(eximgui->gpu_device, eximgui->window);
	init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
	ImGui_ImplSDLGPU3_Init(&init_info);

	return 0;
}

int jmgui_context_fini(jmgui_context_t *eximgui)
{
	// Cleanup
	// [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppQuit() function]
	SDL_WaitForGPUIdle(eximgui->gpu_device);
	ImGui_ImplSDL3_Shutdown();
	ImGui_ImplSDLGPU3_Shutdown();
	ImGui::DestroyContext();

	SDL_ReleaseWindowFromGPUDevice(eximgui->gpu_device, eximgui->window);
	SDL_DestroyGPUDevice(eximgui->gpu_device);
	SDL_DestroyWindow(eximgui->window);
	SDL_Quit();
	return 0;
}

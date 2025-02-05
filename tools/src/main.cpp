
#include <windows.h>
#include <Shlwapi.h>
#include <filesystem>
#include <ShellScalingApi.h>

#include <core/logger/Logger.h>
#include <graphics/win32/Window.h>
#include <graphics/dx12/DX12System.h>

#include "SimpleImGuiWindow.h"

bool running = true;

void quit()
{
	running = false;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);

	std::filesystem::path path(buffer);
	path = path.parent_path();
	std::filesystem::current_path(path);
	SetCurrentDirectoryW(path.c_str());

	gallus::core::logger::LOGGER.Initialize(true);

	gallus::graphics::win32::Window window;
	window.Initialize(true, hInstance);
	window.SetSize(glm::ivec2(600, 1200));
	window.SetTitle("Font Embedder");
	window.m_OnQuit += &quit;

	SimpleImGuiWindow imgui;
	gallus::graphics::dx12::DX12System dx12;
	imgui.Initialize(window.GetHWnd(), dx12, window);

	dx12.Initialize(true, window.GetHWnd(), window.GetRealSize(), &window);

	while (running)
	{
	}

	dx12.Destroy();

	window.Destroy();

	gallus::core::logger::LOGGER.Destroy();

	return 0;
}
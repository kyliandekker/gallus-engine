
#include <windows.h>
#include <Shlwapi.h>
#include <filesystem>
#include <ShellScalingApi.h>

#include "core/Engine.h"

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

	gallus::core::ENGINE.Initialize(hInstance, 1920, 1080, "Gallus Engine");
	gallus::core::ENGINE.Destroy();

	return 0;
}
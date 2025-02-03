#ifdef __EDITOR__

#include "editor/imgui/windows/MainWindow.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <string>

#include "utils/string_extensions.h"
#include "core/logger/Logger.h"
#include "editor/imgui/ImGuiDefines.h"

namespace coopscoop
{
	namespace editor
	{
		namespace imgui
		{
			MainWindow::MainWindow(ImGuiWindow& a_Window) : BaseWindow(a_Window, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking, "", "DockSpace", true)
			{}

			bool MainWindow::Initialize()
			{
				return true;
			}

			void MainWindow::Render()
			{
				ImGui::DockSpace(ImGui::GetID("DockSpace"));
			}
		}
	}
}

#endif // __EDITOR__
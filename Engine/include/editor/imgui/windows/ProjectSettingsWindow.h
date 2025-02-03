#pragma once

#ifdef __EDITOR__

#include "editor/imgui/windows/BaseWindow.h"

#include "project/ProjectSettings.h"

namespace renegade
{
	namespace editor
	{
		namespace imgui
		{
			class ImGuiWindow;

			/// <summary>
			/// A window that displays and manages the settings associated with the current project.
			/// </summary>
			class ProjectSettingsWindow : public BaseWindow
			{
			public:
				/// <summary>
				/// Constructs the project settings window.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				ProjectSettingsWindow(ImGuiWindow& a_Window);

				/// <summary>
				/// Renders the main window.
				/// </summary>
				void Render() override;
			};
		}
	}
}

#endif // __EDITOR__
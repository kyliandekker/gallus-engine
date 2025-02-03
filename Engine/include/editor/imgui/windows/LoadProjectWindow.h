#pragma once

#ifdef __EDITOR__

#include "editor/imgui/windows/BaseWindow.h"

namespace renegade
{
	namespace editor
	{
		namespace imgui
		{
			/// <summary>
			/// A window that displays and manages previously loaded projects.
			/// Allows creation of new projects and inspection of existing projects.
			/// </summary>
			class LoadProjectWindow : public BaseWindow
			{
			public:
				/// <summary>
				/// Constructs a project window.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				LoadProjectWindow(ImGuiWindow& a_Window);

				/// <summary>
				/// Renders the project window.
				/// </summary>
				void Render() override;
			};
		}
	}
}

#endif // __EDITOR__
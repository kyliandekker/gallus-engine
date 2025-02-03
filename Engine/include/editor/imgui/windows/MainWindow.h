#pragma once

#ifdef __EDITOR__

#include "editor/imgui/windows/BaseWindow.h"

namespace coopscoop
{
	namespace editor
	{
		namespace imgui
		{
			/// <summary>
			/// Main window that allows docking for other windows.
			/// </summary>
			class MainWindow : public BaseWindow
			{
			public:
				/// <summary>
				/// Constructs the main window.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				MainWindow(ImGuiWindow& a_Window);

				/// <summary>
				/// Initializes all values and behaviours associated with the main window.
				/// </summary>
				/// <returns>True if initialization is successful, otherwise false.</returns>
				bool Initialize() override;

				/// <summary>
				/// Renders the main window.
				/// </summary>
				void Render() override;
			};
		}
	}
}

#endif // __EDITOR__
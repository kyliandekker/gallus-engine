#pragma once

#ifdef _EDITOR

#include "editor/imgui/windows/BaseWindow.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			class CommandList;
		}
	}
	namespace editor
	{
		namespace imgui
		{
			class ImGuiWindow;

			/// <summary>
			/// A window that displays and manages various selectable items in the project.
			/// Allows inspection and changing of selectable items.
			/// </summary>
			class InspectorWindow : public BaseWindow
			{
			public:
				/// <summary>
				/// Constructs an inspector window.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				InspectorWindow(ImGuiWindow& a_Window);

				/// <summary>
				/// Initializes all values and behaviours associated with the inspector window.
				/// </summary>
				/// <returns>True if initialization is successful, otherwise false.</returns>
				bool Initialize() override;

				/// <summary>
				/// Renders the inspector window.
				/// </summary>
				void Render() override;
			};
		}
	}
}

#endif // _EDITOR
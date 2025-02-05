#pragma once

#ifdef _EDITOR

#include "graphics/dx12/DX12PCH.h"

#include <glm/vec2.hpp>

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
			/// A window that displays the scene.
			/// </summary>
			class SceneWindow : public BaseWindow
			{
			public:
				/// <summary>
				/// Constructs the project settings window.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				SceneWindow(ImGuiWindow& a_Window);

				/// <summary>
				/// Renders the scene window.
				/// </summary>
				void Render() override;
			};
		}
	}
}

#endif // _EDITOR
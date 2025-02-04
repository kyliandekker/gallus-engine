#pragma once

#ifdef _EDITOR

#include <glm/vec2.hpp>

#include "editor/imgui/windows/BaseWindow.h"

namespace coopscoop
{
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

#ifdef _EDITOR
#ifdef _RENDER_TEX
				/// <summary>
				/// Renders the scene window.
				/// </summary>
				void UpdateRenderTexture();
#endif // _RENDER_TEX
#endif // _EDITOR
			private:
				glm::ivec2 m_RenderTextureSize;
			};
		}
	}
}

#endif // _EDITOR
#ifdef _EDITOR

#include "editor/imgui/windows/SceneWindow.h"

#include <imgui/imgui_helpers.h>

#include "editor/imgui/font_icon.h"
#include "editor/imgui/ImGuiWindow.h"
#include "core/Engine.h"
#include "graphics/dx12/Texture.h"

namespace gallus
{
	namespace editor
	{
		namespace imgui
		{
			SceneWindow::SceneWindow(ImGuiWindow& a_Window) : BaseWindow(a_Window, ImGuiWindowFlags_NoCollapse, std::string(font::ICON_SCENE) + " Scene", "Scene")
			{}

			void SceneWindow::Render()
			{
				ImVec2 initialPos = ImGui::GetCursorPos();

				ImVec2 toolbarSize = ImVec2(ImGui::GetContentRegionAvail().x, m_Window.GetHeaderSize().y);
				ImGui::BeginToolbar(toolbarSize);

				float x = ImGui::GetContentRegionAvail().x / 2;
				x -= (toolbarSize.y / 2);
				x -= (toolbarSize.y / 2);

				ImGui::SetCursorPosX(x);

				//bool m_Started = core::ENGINE.GetECS().HasStarted(), m_Paused = core::ENGINE.GetECS().IsPaused();
				//if (ImGui::TransparentCheckboxButton(IMGUI_FORMAT_ID(font::ICON_PLAY, BUTTON_ID, "PLAYSTOP_SCENE").c_str(), &m_Started, ImVec2(toolbarSize.y, toolbarSize.y)))
				//{
				//	core::ENGINE.GetECS().SetStarted(m_Started);
				//}
				//ImGui::SameLine();

				//if (ImGui::TransparentCheckboxButton(IMGUI_FORMAT_ID(font::ICON_PAUSE, BUTTON_ID, "PAUSE_SCENE").c_str(), &m_Paused, ImVec2(toolbarSize.y, toolbarSize.y)))
				//{
				//	core::ENGINE.GetECS().SetPaused(m_Paused);
				//}

				ImGui::EndToolbar(ImVec2(ImGui::GetStyle().ItemSpacing.x, 0));

#ifdef _EDITOR
#ifdef _RENDER_TEX
				ImGui::SetCursorPos(ImVec2(initialPos.x, initialPos.y + toolbarSize.y));

				ImVec2 availableSize = ImGui::GetContentRegionAvail();
				ImVec2 textureSize = ImVec2(core::ENGINE.GetDX12().GetRenderTexture()->GetSize().x,
					core::ENGINE.GetDX12().GetRenderTexture()->GetSize().y);

				float textureAspect = textureSize.x / textureSize.y;
				float regionAspect = availableSize.x / availableSize.y;

				// UV coordinates for cropping
				ImVec2 uv0(0.0f, 0.0f);
				ImVec2 uv1(1.0f, 1.0f);

				if (textureAspect > regionAspect)
				{
// Texture is wider than the region; crop horizontally
					float newWidth = regionAspect / textureAspect;
					uv0.x = (1.0f - newWidth) * 0.5f;
					uv1.x = 1.0f - uv0.x;
				}
				else
				{
			 // Texture is taller than the region; crop vertically
					float newHeight = textureAspect / regionAspect;
					uv0.y = (1.0f - newHeight) * 0.5f;
					uv1.y = 1.0f - uv0.y;
				}

				// Render the cropped image
				ImGui::Image((ImTextureID) core::ENGINE.GetDX12().GetRenderTexture()->GetGPUHandle().ptr,
					availableSize, uv0, uv1);

#endif // _RENDER_TEX
#endif // _EDITOR

				//ImGui::SetCursorPosY(y);
				//std::string fpsValue = std::to_string(static_cast<uint64_t>(std::round(core::ENGINE.GetWindow().GetDX12Window().GetFPS().GetFPS()))) + "fps";

				//ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - (ImGui::CalcTextSize(fpsValue.c_str()).x + m_Window.GetWindowPadding().x));
				//ImGui::TextColored(ImVec4(1, 1, 0, 1), fpsValue.c_str());
			}
		}
	}
}

#endif // _EDITOR
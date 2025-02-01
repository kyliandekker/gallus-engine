#ifdef __EDITOR__

#include "editor/imgui/windows/SceneWindow.h"

#include <imgui/imgui_helpers.h>

#include "editor/imgui/ImGuiDefines.h"
#include "editor/imgui/ImGuiWindow.h"
#include "core/Engine.h"

namespace renegade
{
	namespace editor
	{
		namespace imgui
		{
			SceneWindow::SceneWindow(const ImGuiWindow& a_Window) : BaseWindow(a_Window, ImGuiWindowFlags_NoCollapse, "Scene", "Scene")
			{ }

			void SceneWindow::Render()
			{
				ImVec2 toolbarSize = ImVec2(ImGui::GetContentRegionAvail().x, m_Window.HeaderSize().y);
				ImGui::BeginToolbar(toolbarSize);

				float x = ImGui::GetContentRegionAvail().x / 2;
				x -= (toolbarSize.y / 2);
				x -= (toolbarSize.y / 2);

				ImGui::SetCursorPosX(x);

				bool m_Started = core::ENGINE.GetECS().HasStarted(), m_Paused = core::ENGINE.GetECS().IsPaused();
				if (ImGui::TransparentCheckboxButton(IMGUI_FORMAT_ID(ICON_PLAY, BUTTON_ID, "PLAYSTOP_SCENE").c_str(), &m_Started, ImVec2(toolbarSize.y, toolbarSize.y)))
				{
					core::ENGINE.GetECS().SetStarted(m_Started);
				}
				ImGui::SameLine();

				if (ImGui::TransparentCheckboxButton(IMGUI_FORMAT_ID(ICON_PAUSE, BUTTON_ID, "PAUSE_SCENE").c_str(), &m_Paused, ImVec2(toolbarSize.y, toolbarSize.y)))
				{
					core::ENGINE.GetECS().SetPaused(m_Paused);
				}

				ImGui::EndToolbar(ImVec2(ImGui::GetStyle().ItemSpacing.x, 0));

				float y = ImGui::GetCursorPosY();

				ImGui::Image((ImTextureID)core::ENGINE.GetWindow().GetDX12Window().m_RenderTargetSrvHandleGPU.ptr, ImGui::GetContentRegionAvail());

				ImGui::SetCursorPosY(y);
				std::string fpsValue = std::to_string(static_cast<uint64_t>(std::round(core::ENGINE.GetWindow().GetDX12Window().GetFPS().GetFPS()))) + "fps";

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - (ImGui::CalcTextSize(fpsValue.c_str()).x + m_Window.GetWindowPadding().x));
				ImGui::TextColored(ImVec4(1, 1, 0, 1), fpsValue.c_str());
			}
		}
	}
}

#endif // __EDITOR__
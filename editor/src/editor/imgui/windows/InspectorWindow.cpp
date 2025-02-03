#ifdef _EDITOR

#include "editor/imgui/windows/InspectorWindow.h"

#include <imgui/imgui_helpers.h>

//#include "gameplay/EntityComponentSystem.h"
#include "editor/imgui/ImGuiDefines.h"
#include "core/logger/Logger.h"
#include "editor/imgui/ImGuiWindow.h"
#include "core/Engine.h"

namespace coopscoop
{
	namespace editor
	{
		namespace imgui
		{
			InspectorWindow::InspectorWindow(ImGuiWindow& a_Window) : BaseWindow(a_Window, ImGuiWindowFlags_NoCollapse, "Inspector", "Inspector")
			{}

			bool InspectorWindow::Initialize()
			{
				return true;
			}

			void InspectorWindow::Render()
			{
				if (core::ENGINE.GetEditor().GetSelectable())
				{
					core::ENGINE.GetEditor().GetSelectable()->RenderSelectable();
				}
				else
				{
					float x = ImGui::GetCursorPosX();
					float y = ImGui::GetCursorPosY();
					ImVec2 toolbarSize = ImVec2(ImGui::GetContentRegionAvail().x, m_Window.GetHeaderSize().y * 2.5f + m_Window.GetFramePadding().y);
					ImGui::BeginToolbar(toolbarSize);

					ImGui::SetCursorPosY(y + toolbarSize.y);

					ImGui::EndToolbar(m_Window.GetFramePadding());

					if (ImGui::BeginChild(
						IMGUI_FORMAT_ID("", CHILD_ID, "INSPECTOR_DEFAULT").c_str(),
						ImVec2(
						ImGui::GetContentRegionAvail().x - m_Window.GetFramePadding().x,
						ImGui::GetContentRegionAvail().y - m_Window.GetFramePadding().y
						),
						ImGuiChildFlags_Borders
						))
					{

					}
					ImGui::EndChild();
				}
			}
		}
	}
}

#endif // _EDITOR
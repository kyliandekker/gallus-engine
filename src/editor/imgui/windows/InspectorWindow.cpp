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
					//std::lock_guard<std::recursive_mutex> lock(gameplay::m_EntityMutex);
					core::ENGINE.GetEditor().GetSelectable()->RenderSelectable();
				}
				//if (ImGui::Button("Info"))
				//{
				//	LOGF(LOGSEVERITY_INFO, "This is an info message.");
				//}
				//ImGui::SameLine();
				//if (ImGui::Button("Warning"))
				//{
				//	LOGF(LOGSEVERITY_WARNING, "This is a warning message.");
				//}
				//ImGui::SameLine();
				//if (ImGui::Button("Error"))
				//{
				//	LOGF(LOGSEVERITY_ERROR, "This is an error message.");
				//}
				//if (ImGui::Button("Assert"))
				//{
				//	LOGF(LOGSEVERITY_ASSERT, "This is an assert message.");
				//}
				//ImGui::SameLine();
				//if (ImGui::Button("Success"))
				//{
				//	LOGF(LOGSEVERITY_SUCCESS, "This is a success message.");
				//}
				//ImGui::SameLine();
				//if (ImGui::Button("Info Success"))
				//{
				//	LOGF(LOGSEVERITY_INFO_SUCCESS, "This is an info success message.");
				//}
				//ImGui::SameLine();
				//if (ImGui::Button("Awesome"))
				//{
				//	LOGF(LOGSEVERITY_AWESOME, "This is an awesome message.");
				//}
			}
		}
	}
}

#endif // _EDITOR
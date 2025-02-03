#ifdef __EDITOR__

#include "editor/imgui/windows/ProjectSettingsWindow.h"

#include <imgui/imgui_helpers.h>

#include "editor/imgui/ImGuiDefines.h"
#include "core/Engine.h"

namespace renegade
{
	namespace editor
	{
		namespace imgui
		{
			ProjectSettingsWindow::ProjectSettingsWindow(ImGuiWindow& a_Window) : BaseWindow(a_Window, ImGuiWindowFlags_NoCollapse, "Project Settings", "Project Settings") 
			{
			}

			void ProjectSettingsWindow::Render()
			{
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetStyle().ItemSpacing.x);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(m_Window.GetFramePadding().x * 2, m_Window.GetFramePadding().y * 2));
				if (ImGui::BeginChild(
					IMGUI_FORMAT_ID("", CHILD_ID, "PROJECT_SETTINGS_CHILD").c_str(),
					ImVec2(
						ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x,
						ImGui::GetContentRegionAvail().y - ImGui::GetStyle().ItemSpacing.y
					),
					ImGuiChildFlags_Borders
				))
				{
					if (ImGui::CollapsingHeader(IMGUI_FORMAT_ID("Localization", FOLDOUT_ID, "PROJECT_SETTINGS_LOCALIZATION").c_str()))
					{
						ImGui::Indent();
						ImGui::DisplayHeader(m_Window.Bold(), "Subtitle Supported Languages");
						ImGui::Indent();
						for (size_t i = 0; i < (int)project::Language::MAX; i++)
						{
							bool checked = core::ENGINE.GetEditor().GetAssetDatabase().GetProjectSettings().HasLanguage((project::Language)i);
							if (ImGui::Checkbox(IMGUI_FORMAT_ID(" " + project::LanguageToString((project::Language)i), CHECKBOX_ID, "PROJECT_SETTINGS_LOCALIZATION_SUPPORTED_LANGUAGES_" + project::LanguageToString((project::Language)i)).c_str(), &checked))
							{
								if (checked)
								{
									core::ENGINE.GetEditor().GetAssetDatabase().GetProjectSettings().AddLanguage((project::Language)i);
								}
								else
								{
									core::ENGINE.GetEditor().GetAssetDatabase().GetProjectSettings().RemoveLanguage((project::Language)i);
								}
							}
						}
						ImGui::Unindent();
						ImGui::Unindent();
					}
				}
				ImGui::EndChild();
				ImGui::PopStyleVar();
			}
		}
	}
}

#endif // __EDITOR__
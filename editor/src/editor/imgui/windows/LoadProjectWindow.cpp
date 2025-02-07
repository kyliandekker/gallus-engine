//#ifdef _EDITOR
//
//#include "editor/imgui/windows/LoadProjectWindow.h"
//
//#include <thread>
//#include "editor/imgui/font_icon.h"
//#include "utils/string_extensions.h"
//
//#include "editor/imgui/ImGuiWindow.h"
//#include "core/Engine.h"
//#include "file/FileLoader.h"
//
//namespace renegade
//{
//	namespace editor
//	{
//		namespace imgui
//		{
//			LoadProjectWindow::LoadProjectWindow(ImGuiWindow& a_Window) : BaseWindow(a_Window, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking, "", "Load Project", true)
//			{
//			}
//
//			void LoadProjectWindow::Render()
//			{
//				ImGui::PushFont(m_Window.Capital());
//				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetStyle().ItemSpacing.x + (m_Window.GetFramePadding().x * 2));
//				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y + (m_Window.GetFramePadding().x * 2));
//
//				float openProjectButtonWidth = 200, createNewProjectButtonWidth = 250;
//				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - (openProjectButtonWidth + createNewProjectButtonWidth + ImGui::GetStyle().ItemSpacing.x + ImGui::GetStyle().ItemSpacing.x + (m_Window.GetFramePadding().x * 2)));
//				if (ImGui::Button("Open Project", ImVec2(openProjectButtonWidth, 0)))
//				{
//					core::ENGINE.GetFileLoader().EnqueueTask([]() mutable
//					{
//						std::string path;
//						HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
//						if (SUCCEEDED(hr))
//						{
//							if (file::FileLoader::PickContainer(path))
//							{
//								core::ENGINE.GetEditor().GetAssetDatabase().LoadProject(path);
//								return true;
//							}
//						}
//						CoUninitialize();
//						return false;
//					});
//				}
//				ImGui::SameLine();
//				if (ImGui::Button("Create New Project", ImVec2(createNewProjectButtonWidth, 0)))
//				{
//					core::ENGINE.GetFileLoader().EnqueueTask([]() mutable
//					{
//						std::string path;
//						HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
//						if (SUCCEEDED(hr))
//						{
//							if (file::FileLoader::PickContainer(path))
//							{
//								core::ENGINE.GetEditor().GetAssetDatabase().LoadProject(path);
//								return true;
//							}
//						}
//						CoUninitialize();
//						return false;
//					});
//				}
//
//				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetStyle().ItemSpacing.x);
//				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (m_Window.GetFramePadding().x * 2));
//				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(m_Window.GetFramePadding().x * 2, m_Window.GetFramePadding().y * 2));
//				if (ImGui::BeginChild(
//					IMGUI_FORMAT_ID("", CHILD_ID, "LOAD_PROJECT").c_str(),
//					ImVec2(
//						ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x,
//						ImGui::GetContentRegionAvail().y - ImGui::GetStyle().ItemSpacing.y
//					),
//					ImGuiChildFlags_Borders
//				))
//				{
//					for (auto projectPath : core::ENGINE.GetEditor().GetEditorSettings().GetPreviousProjects())
//					{
//						if (ImGui::Button(IMGUI_FORMAT_ID(font::ICON_DELETE, BUTTON_ID, "DELETE_PATH_" + string_extensions::StringToUpper(projectPath)).c_str()))
//						{
//							core::ENGINE.GetEditor().GetEditorSettings().ErasePreviousProject(projectPath);
//						}
//						ImGui::SameLine();
//						if (ImGui::Button(projectPath.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0)))
//						{
//							core::ENGINE.GetEditor().GetAssetDatabase().LoadProject(projectPath);
//						}
//					}
//				}
//				ImGui::EndChild();
//				ImGui::PopStyleVar();
//				ImGui::PopFont();
//			}
//		}
//	}
//}
//
//#endif // _EDITOR
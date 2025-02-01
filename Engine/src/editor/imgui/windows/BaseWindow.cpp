#ifdef __EDITOR__

#include "editor/imgui/windows/BaseWindow.h"

#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>

#include "utils/string_extensions.h"
#include "editor/imgui/ImGuiWindow.h"
#include "core/Engine.h"
#include "editor/imgui/ImGuiDefines.h"

namespace coopscoop
{
	namespace editor
	{
		namespace imgui
		{
			BaseWindow::BaseWindow(const ImGuiWindow& a_Window, ImGuiWindowFlags a_Flags, const std::string& a_Name, std::string a_WindowID, bool a_FullScreen) : m_Flags(a_Flags), m_Name(a_Name), m_WindowID(a_WindowID), m_FullScreen(a_FullScreen), ImGuiUIView(a_Window)
			{ }

			BaseWindow::~BaseWindow() = default;

            bool BaseWindow::Initialize()
            {
                return true;
            }

            bool BaseWindow::Destroy()
            {
                return true;
            }

			bool BaseWindow::WindowBegin()
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, m_Window.GetFramePadding());
				bool b = ImGui::Begin(IMGUI_FORMAT_ID(m_Name, WINDOW_ID, string_extensions::StringToUpper(m_WindowID)).c_str(), &m_Enabled, m_Flags);
				return b;
			}

			void BaseWindow::WindowEnd()
			{
				ImGui::End();
				ImGui::PopStyleVar();
			}

			void BaseWindow::Update()
			{
				if (m_FullScreen)
				{
					ImGui::SetNextWindowPos(ImVec2(0, 0));
					ImGui::SetNextWindowSize(ImVec2(m_Size.x, m_Size.y));
				}

				bool showRender = true;
				if (!m_RenderWindowItself)
				{
					showRender = WindowBegin();
				}

				if (showRender)
				{
					Render();
				}

				if (!m_RenderWindowItself)
				{
					WindowEnd();
				}
			}

			void BaseWindow::SetSize(ImVec2 a_Size)
			{
				m_Size = a_Size;
			}

			bool BaseWindow::IsFullScreen() const
			{
				return m_FullScreen;
			}
		}
	}
}

#endif // __EDITOR__
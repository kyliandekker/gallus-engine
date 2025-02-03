#pragma once

#ifdef __EDITOR__

#include <string>
#include <imgui/imgui.h>

#include "editor/imgui/views/ImGuiUIView.h"

typedef int ImGuiWindowFlags;

namespace coopscoop
{
	namespace editor
	{
		namespace imgui
		{
			class ImGuiWindow;

			class BaseWindow : public ImGuiUIView
			{
			public:
				/// <summary>
				/// Constructs a window.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_Flags">The flags used for disabling or enabling certain behaviours in the window.</param>
				/// <param name="a_Name">The name of the window (displayed at the top).</param>
				/// <param name="a_WindowID">The ID of the window.</param>
				/// <param name="a_FullScreen">Whether the window is full screen or not.</param>
				BaseWindow(ImGuiWindow& a_Window, ImGuiWindowFlags a_Flags, const std::string& a_Name, std::string a_WindowID, bool a_FullScreen = false);

				/// <summary>
				/// Destructor for BaseWindow.
				/// </summary>
				~BaseWindow();

				/// <summary>
				/// Initializes all behaviours and values for the window.
				/// </summary>
				/// <returns>True if initialization is successful, otherwise false.</returns>
				virtual bool Initialize();

				/// <summary>
				/// Destroys and disables the window.
				/// </summary>
				/// <returns>True if destruction is successful, otherwise false.</returns>
				virtual bool Destroy();

				/// <summary>
				/// Renders the window.
				/// </summary>
				virtual bool WindowBegin();

				/// <summary>
				/// Finalizes rendering of the window.
				/// </summary>
				virtual void WindowEnd();

				/// <summary>
				/// Update loop for the window. This is where all ImGui interaction should be like buttons, etc.
				/// </summary>
				void Update();

				/// <summary>
				/// Sets the size of the window.
				/// </summary>
				/// <param name="a_Size"></param>
				void SetSize(ImVec2 a_Size);

				/// <summary>
				/// Retrieves the full screen state of the window.
				/// </summary>
				/// <returns>True if window is in full screen mode, otherwise false.</returns>
				bool IsFullScreen() const;
			protected:
				ImGuiWindowFlags m_Flags = 0; /// The flags associated with certain window behaviours.
				ImVec2 m_Size = ImVec2(0, 0); /// The current size of the window.
				std::string
					m_Name, /// The name of the window.
					m_WindowID; /// The ID of the window.
				bool m_FullScreen = false; /// Whether the window is in full screen mode or not.
				bool m_Enabled = true; /// Whether the window is enabled or not.
				bool m_RenderWindowItself = false; /// Whether the window only needs to render the elements or the window around it.
			};
		}
	}
}

#endif // __EDITOR__
#pragma once

#ifdef __EDITOR__

#include "core/System.h"

#include <wtypes.h>
#include <memory>
#include <imgui/imgui.h>

namespace coopscoop
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
			class ImGuiWindow : public core::System
			{
			public:
				/// <summary>
				/// Initializes the imgui system.
				/// </summary>
				bool Initialize() override;

				/// <summary>
				/// Destroys all imgui resources.
				/// </summary>
				bool Destroy() override;

				void Render(std::shared_ptr<graphics::dx12::CommandList> a_CommandList);
			private:
				/// <summary>
				/// Handles Windows messages for the editor's window.
				/// </summary>
				/// <param name="a_hWnd">Handle to the window.</param>
				/// <param name="a_Msg">Message identifier.</param>
				/// <param name="a_wParam">Additional message information (WPARAM).</param>
				/// <param name="a_lParam">Additional message information (LPARAM).</param>
				/// <returns>The result of the message processing.</returns>
				LRESULT CALLBACK WndProcHandler(HWND a_hWnd, UINT a_Msg, WPARAM a_wParam, LPARAM a_lParam);

				/// <summary>
				/// Creates the ImGui context for Win32.
				/// </summary>
				/// <returns>True if the context creation succeeds, otherwise false.</returns>
				bool CreateContextWin32();

				/// <summary>
				/// Creates the ImGui context for DirectX 12.
				/// </summary>
				/// <returns>True if the context creation succeeds, otherwise false.</returns>
				bool CreateContextDX12();

				/// <summary>
				/// Initializes the ImGui UI components.
				/// </summary>
				void CreateImGui();

				std::string m_IniPath; /// Path to the ImGui configuration file.

				size_t m_SrvIndex = 0;

				ImFont* m_DefaultFont;
				ImFont* m_Capital;
				ImFont* m_CapitalIconFont;
				ImFont* m_BoldFont;
				ImFont* m_IconFont;
				ImFont* m_SmallIconFont;

				float m_FontSize = 17.5f; /// Default font size for ImGui.
				float m_IconFontSize = 25.0f; /// Default font size for ImGui.

				ImVec2 m_FramePadding = ImVec2(8, 8); /// Frame padding for ImGui elements.
				ImVec2 m_WindowPadding = ImVec2(8, 8); /// Window padding for ImGui elements.
				ImVec2 m_HeaderSize; /// Header size for ImGui elements.
			};
		}
	}
}

#endif // __EDITOR__
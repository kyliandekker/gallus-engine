#pragma once

#ifdef _EDITOR

#include "core/System.h"

#include <wtypes.h>
#include <memory>
#include <imgui/imgui.h>
#include <glm/vec2.hpp>

#include "editor/imgui/windows/MainWindow.h"
#include "editor/imgui/windows/ConsoleWindow.h"
#include "editor/imgui/windows/SceneWindow.h"
#include "editor/imgui/windows/ExplorerWindow.h"
#include "editor/imgui/windows/HierarchyWindow.h"
#include "editor/imgui/windows/InspectorWindow.h"
#include "core/FileUtils.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			class DX12System;
			class CommandList;
			class Texture;
		}
	}
	namespace editor
	{
		namespace imgui
		{
			class ImGuiWindow : public core::System
			{
			public:
				ImGuiWindow();

				/// <summary>
				/// Initializes the imgui system.
				/// </summary>
				bool Initialize();

				/// <summary>
				/// Destroys all imgui resources.
				/// </summary>
				bool Destroy() override;
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

				/// <summary>
				/// Called when the render target is created.
				/// </summary>
				void OnRenderTargetCreated();

				/// <summary>
				/// Called when the render target is created.
				/// </summary>
				/// <param name="a_Pos">Position of the window.</param>
				/// <param name="a_Size">Size of the window.</param>
				void Resize(const glm::ivec2& a_Pos, const glm::ivec2& a_Size);

				/// <summary>
				/// Called when the render target is cleaned.
				/// </summary>
				void OnRenderTargetCleaned();
			public:
				void Render(std::shared_ptr<graphics::dx12::CommandList> a_CommandList);

				ImFont* GetCapitalFont() const;
				ImFont* GetBigIconFont() const;
				ImFont* GetIconFont() const;
				ImFont* GetSmallIconFont() const;
				ImFont* GetBoldFont() const;
				float GetFontSize() const;
				ImVec2 GetFramePadding() const;
				ImVec2 GetWindowPadding() const;
				ImVec2 GetHeaderSize() const;

				void SetPreviewTexture(const fs::path& a_Path);
				graphics::dx12::Texture* GetPreviewTexture() const;

				void Update();
				fs::path m_PreviewPath;
			private:
				std::string m_IniPath; /// Path to the ImGui configuration file.

				size_t m_SrvIndex = 0;

				ImFont* m_DefaultFont = nullptr;
				ImFont* m_Capital = nullptr;
				ImFont* m_CapitalIconFont = nullptr;
				ImFont* m_BoldFont = nullptr;
				ImFont* m_IconFont = nullptr;
				ImFont* m_IconFontM = nullptr;
				ImFont* m_SmallIconFont = nullptr;

				float m_FontSize = 25.0f; /// Default font size for ImGui.
				float m_IconFontSize = 20.0f; /// Default font size for ImGui.

				ImVec2 m_FramePadding = ImVec2(16, 16); /// Frame padding for ImGui elements.
				ImVec2 m_WindowPadding = ImVec2(16, 16); /// Window padding for ImGui elements.
				ImVec2 m_HeaderSize;

				// Windows.
				MainWindow m_MainWindow;
				ConsoleWindow m_ConsoleWindow;
				SceneWindow m_SceneWindow;
				ExplorerWindow m_ExplorerWindow;
				HierarchyWindow m_HierarchyWindow;
				InspectorWindow m_InspectorWindow;

				// Preview texture in the Inspector window.
				graphics::dx12::Texture* m_PreviewTexture = nullptr;

				friend graphics::dx12::DX12System;
			};
		}
	}
}

#endif // _EDITOR
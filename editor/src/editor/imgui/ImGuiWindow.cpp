#ifdef _EDITOR

#include "editor/imgui/ImGuiWindow.h"

#include <imgui/implot.h>
#include <imgui/backends/imgui_impl_win32.h>
#include <imgui/backends/imgui_impl_dx12.h>

#include "core/FileUtils.h"
#include "core/logger/Logger.h"
#include "core/Engine.h"
#include "editor/imgui/ImGuiDefines.h"
#include "editor/imgui/font_arial.h"
#include "editor/imgui/font_icon.h"
#include "editor/Editor.h"

#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/Texture.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace coopscoop
{
	namespace editor
	{
		namespace imgui
		{
			ImGuiWindow::ImGuiWindow()
				: m_MainWindow(*this),
				m_ConsoleWindow(*this),
				m_SceneWindow(*this),
				m_ExplorerWindow(*this),
				m_HierarchyWindow(*this),
				m_InspectorWindow(*this)
			{}

			bool ImGuiWindow::Initialize(std::shared_ptr<graphics::dx12::CommandList> a_CommandList)
			{
				IMGUI_CHECKVERSION();
				ImGui::CreateContext();
				ImPlot::CreateContext();

				m_IniPath = std::string(file::FileLoader::GetAppDataPath().generic_string() + SETTINGS_FOLDER + "imgui.ini");
				ImGuiIO& io = ImGui::GetIO();
				io.IniFilename = m_IniPath.c_str();

				if (!CreateContextWin32() || !CreateContextDX12())
				{
					return false;
				}
				core::ENGINE.GetWindow().m_OnMsg += std::bind(&ImGuiWindow::WndProcHandler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

				CreateImGui();

				LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_EDITOR, "Initialized ImGui.");

				m_MainWindow.Initialize();
				m_ConsoleWindow.Initialize();
				m_SceneWindow.Initialize();
				m_ExplorerWindow.Initialize();
				m_HierarchyWindow.Initialize();
				m_InspectorWindow.Initialize();
				//m_LoadProjectWindow.Initialize();

				m_PreviewTexture = &core::ENGINE.GetDX12().GetResourceAtlas().LoadTexture("tex_missing.png", a_CommandList); // Default texture.

				return System::Initialize();
			}

			bool ImGuiWindow::Destroy()
			{
				m_MainWindow.Destroy();
				m_ConsoleWindow.Destroy();
				m_SceneWindow.Destroy();
				m_ExplorerWindow.Destroy();
				m_HierarchyWindow.Destroy();
				m_InspectorWindow.Destroy();
				//m_LoadProjectWindow.Destroy();

				ImGui_ImplDX12_Shutdown();
				ImGui_ImplWin32_Shutdown();
				ImPlot::DestroyContext();
				ImGui::DestroyContext();

				LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_EDITOR, "Destroyed ImGui.");

				return true;
			}

			bool ImGuiWindow::CreateContextWin32()
			{
				if (!ImGui_ImplWin32_Init(core::ENGINE.GetWindow().GetHWnd()))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_EDITOR, "Failed creating WIN32 context for ImGui.");
					return false;
				}

				LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_EDITOR, "Created WIN32 context for ImGui.");
				return true;
			}

			bool ImGuiWindow::CreateContextDX12()
			{
				graphics::dx12::DX12System& dx12window = core::ENGINE.GetDX12();
				m_SrvIndex = dx12window.GetSRV().Allocate();

				if (!ImGui_ImplDX12_Init(dx12window.GetDevice().Get(), graphics::dx12::g_BufferCount,
					DXGI_FORMAT_R8G8B8A8_UNORM, dx12window.GetSRV().GetHeap().Get(),
					dx12window.GetSRV().GetCPUHandle(m_SrvIndex),
					dx12window.GetSRV().GetGPUHandle(m_SrvIndex)))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_EDITOR, "Failed creating DX12 context for ImGui.");
					return false;
				}

				LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_EDITOR, "Created DX12 context for ImGui.");
				return true;
			}

			void ImGuiWindow::CreateImGui()
			{
				ImGuiIO& io = ImGui::GetIO();
				io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
				io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
				io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;

				(void) io;

				m_HeaderSize = ImVec2(m_IconFontSize * 1.5f, m_IconFontSize * 1.5f);

				// setup Dear ImGui style
				ImGui::StyleColorsDark();

				constexpr ImWchar icons_ranges_b[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };

				ImFontConfig font_config_capital;
				font_config_capital.FontDataOwnedByAtlas = false;
				m_Capital = io.Fonts->AddFontFromMemoryTTF(&font::arial, sizeof(font::arial), m_FontSize, &font_config_capital);

				ImFontConfig font_config_icon_capital;
				font_config_icon_capital.FontDataOwnedByAtlas = false;
				m_CapitalIconFont = io.Fonts->AddFontFromMemoryTTF(&font::icon, sizeof(font::icon), m_HeaderSize.x, &font_config_icon_capital, icons_ranges_b);

				ImFontConfig icons_config_b;
				icons_config_b.FontDataOwnedByAtlas = false;
				m_IconFont = io.Fonts->AddFontFromMemoryTTF(&font::icon, sizeof(font::icon), m_IconFontSize, &icons_config_b, icons_ranges_b);

				ImFontConfig small_icons_config_b;
				small_icons_config_b.FontDataOwnedByAtlas = false;
				m_SmallIconFont = io.Fonts->AddFontFromMemoryTTF(&font::icon, sizeof(font::icon), m_FontSize, &small_icons_config_b, icons_ranges_b);

				ImFontConfig font_config_bold;
				font_config_bold.FontDataOwnedByAtlas = false;
				m_BoldFont = io.Fonts->AddFontFromMemoryTTF(&font::arialBold, sizeof(font::arialBold), m_FontSize, &font_config_bold);

				ImFontConfig font_config_default;
				font_config_default.FontDataOwnedByAtlas = false;
				m_DefaultFont = io.Fonts->AddFontFromMemoryTTF(&font::arial, sizeof(font::arial), m_FontSize, &font_config_default);

				ImFont* font1 = io.Fonts->AddFontDefault();
				(void) font1;

				io.Fonts->Build();

				ImGui::StyleColorsDark();

				ImGuiStyle& style = ImGui::GetStyle();
				ImVec4* colors = style.Colors;
				colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
				colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
				colors[ImGuiCol_ChildBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
				colors[ImGuiCol_PopupBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
				colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
				colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
				colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
				colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
				colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
				colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
				colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
				colors[ImGuiCol_ScrollbarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
				colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_CheckMark] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
				colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_SliderGrabActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
				colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
				colors[ImGuiCol_Header] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_HeaderHovered] = ImVec4(0.24f, 0.24f, 0.24f, 0.60f);
				colors[ImGuiCol_HeaderActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
				colors[ImGuiCol_SeparatorHovered] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
				colors[ImGuiCol_SeparatorActive] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
				colors[ImGuiCol_ResizeGrip] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_ResizeGripActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_Tab] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
				colors[ImGuiCol_TabHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_TabActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_TabUnfocused] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
				colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_PlotLines] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
				colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
				colors[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
				colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
				colors[ImGuiCol_TextSelectedBg] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_DragDropTarget] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
				colors[ImGuiCol_NavHighlight] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
				colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
				colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
				colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
#ifndef _RENDER_TEX
				float transparency = 0.4;
				ImGui::GetStyle().Colors[ImGuiCol_FrameBg].w = transparency;
				ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w = transparency;
				ImGui::GetStyle().Colors[ImGuiCol_ChildBg].w = transparency;
#endif // _RENDER_TEX

				style.WindowBorderSize = 1;
				style.WindowRounding = 0;
				style.WindowPadding = ImVec2(0, 0);
				style.ItemInnerSpacing = ImVec2(0, 0);
				style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
				style.IndentSpacing = 25;
				style.ScrollbarSize = 15;
				style.GrabMinSize = 10;
				style.TabRounding = 8;
				style.TabBorderSize = 1;
				style.PopupBorderSize = 1;
				style.FrameBorderSize = 1;
				style.ChildBorderSize = 1;
				style.ChildRounding = 0;
				style.FrameRounding = 0;
				style.PopupRounding = 0;
				style.ScrollbarRounding = 8;
				style.GrabRounding = 8;
				style.LogSliderDeadzone = 4;
				style.FramePadding = ImVec2(0, 0);

				ImPlotStyle& pStyle = ImPlot::GetStyle();

				colors = pStyle.Colors;
				colors[ImPlotCol_Line] = ImVec4(0.66f, 0.66f, 0.66f, 1.00f);
			}

			void ImGuiWindow::OnRenderTargetCreated()
			{
				if (!m_Ready)
				{
					return;
				}

				ImGui_ImplDX12_CreateDeviceObjects();
			}

			void ImGuiWindow::Resize(const glm::ivec2& a_Pos, const glm::ivec2& a_Size)
			{
				ImGuiIO& io = ImGui::GetIO();
				io.DisplaySize.x = a_Size.x;
				io.DisplaySize.y = a_Size.y;

				core::ENGINE.GetEditor().GetEditorSettings().SetSize(a_Size);
			}

			void ImGuiWindow::OnRenderTargetCleaned()
			{
				if (!m_Ready)
				{
					return;
				}

				ImGui_ImplDX12_InvalidateDeviceObjects();
			}

			void ImGuiWindow::SetPreviewTexture(const fs::path& a_Path)
			{
				m_PreviewPath = a_Path;
			}

			void ImGuiWindow::Update()
			{
				if (!m_PreviewPath.empty() && m_PreviewTexture)
				{
					auto cCommandQueue = core::ENGINE.GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
					auto cCommandList = cCommandQueue->GetCommandList();

					m_PreviewTexture->Destroy();
					m_PreviewTexture->Load(m_PreviewPath.filename().generic_string(), cCommandList);

					float fenceValue = cCommandQueue->ExecuteCommandList(cCommandList);
					cCommandQueue->WaitForFenceValue(fenceValue);

					auto dCommandQueue = core::ENGINE.GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
					auto dCommandList = dCommandQueue->GetCommandList();

					m_PreviewTexture->Transition(dCommandList);

					fenceValue = dCommandQueue->ExecuteCommandList(dCommandList);
					dCommandQueue->WaitForFenceValue(fenceValue);

					m_PreviewPath.clear();
				}
			}

			graphics::dx12::Texture* ImGuiWindow::GetPreviewTexture() const
			{
				return m_PreviewTexture;
			}

			void ImGuiWindow::Render(std::shared_ptr<graphics::dx12::CommandList> a_CommandList)
			{
				if (!m_Ready)
				{
					return;
				}

				ImGui_ImplDX12_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();

				ImGuiIO& io = ImGui::GetIO();
				m_MainWindow.SetSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
				m_MainWindow.Update();
				m_ConsoleWindow.Update();
				m_SceneWindow.Update();
				m_ExplorerWindow.Update();
				m_HierarchyWindow.Update();
				m_InspectorWindow.Update();

				ImGui::EndFrame();
				ImGui::Render();

				ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), a_CommandList->GetCommandList().Get());
			}

			LRESULT ImGuiWindow::WndProcHandler(HWND a_hWnd, UINT a_Msg, WPARAM a_wParam, LPARAM a_lParam)
			{
				if (!m_Ready)
				{
					return a_Msg;
				}

				return ImGui_ImplWin32_WndProcHandler(a_hWnd, a_Msg, a_wParam, a_lParam);
			}

			ImFont* ImGuiWindow::GetCapitalFont() const
			{
				return m_Capital;
			}

			ImFont* ImGuiWindow::GetBigIconFont() const
			{
				return m_CapitalIconFont;
			}

			ImFont* ImGuiWindow::GetIconFont() const
			{
				return m_IconFont;
			}

			ImFont* ImGuiWindow::GetSmallIconFont() const
			{
				return m_SmallIconFont;
			}

			ImFont* ImGuiWindow::GetBoldFont() const
			{
				return m_BoldFont;
			}

			float ImGuiWindow::GetFontSize() const
			{
				return m_FontSize;
			}

			ImVec2 ImGuiWindow::GetFramePadding() const
			{
				return m_FramePadding;
			}

			ImVec2 ImGuiWindow::GetWindowPadding() const
			{
				return m_WindowPadding;
			}

			ImVec2 ImGuiWindow::GetHeaderSize() const
			{
				return m_HeaderSize;
			}
		}
	}
}

#endif // _EDITOR
#include "SimpleImGuiWindow.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_win32.h>
#include <imgui/backends/imgui_impl_dx12.h>

#include <graphics/dx12/DX12System.h>
#include <graphics/win32/Window.h>
#include <graphics/dx12/CommandQueue.h>
#include <graphics/dx12/CommandList.h>
#include <core/logger/Logger.h>
#include <editor/imgui/font_arial.h>

using namespace gallus;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void SimpleImGuiWindow::Initialize(HWND a_Hwnd, gallus::graphics::dx12::DX12System& a_System, gallus::graphics::win32::Window& a_Window)
{
	m_hWnd = a_Hwnd;

	// setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	a_System.m_OnInitialize += std::bind(&SimpleImGuiWindow::CreateContextDX12, this, std::placeholders::_1);
	a_System.m_OnRender += std::bind(&SimpleImGuiWindow::Render, this, std::placeholders::_1);
	a_System.m_OnResize += std::bind(&SimpleImGuiWindow::Resize, this, std::placeholders::_1, std::placeholders::_2);
	a_Window.m_OnMsg += std::bind(&SimpleImGuiWindow::ProcessEvents, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

	CreateContextWin32();
	CreateImGui();
}

void SimpleImGuiWindow::CreateContextWin32()
{
	ImGui_ImplWin32_Init(m_hWnd);
}

bool SimpleImGuiWindow::CreateContextDX12(gallus::graphics::dx12::DX12System& a_System)
{
	float m_SrvIndex = a_System.GetSRV().Allocate();

	if (!ImGui_ImplDX12_Init(a_System.GetDevice().Get(), gallus::graphics::dx12::g_BufferCount,
		DXGI_FORMAT_R8G8B8A8_UNORM, a_System.GetSRV().GetHeap().Get(),
		a_System.GetSRV().GetCPUHandle(m_SrvIndex),
		a_System.GetSRV().GetGPUHandle(m_SrvIndex)))
	{
		LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_EDITOR, "Failed creating DX12 context for ImGui.");
		return false;
	}

	LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_EDITOR, "Created DX12 context for ImGui.");
	return true;
}

ImFont* m_DefaultFont = nullptr;
void SimpleImGuiWindow::CreateImGui() const
{
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

	(void) io;

	ImFontConfig font_config_default;
	font_config_default.FontDataOwnedByAtlas = false;
	m_DefaultFont = io.Fonts->AddFontFromMemoryTTF(&font::arial, sizeof(font::arial), 25, &font_config_default);

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

	style.WindowBorderSize = 1;
	style.WindowRounding = 0;
	style.WindowPadding = ImVec2(25, 25);
	style.ItemInnerSpacing = ImVec2(25, 25);
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
	style.FramePadding = ImVec2(25, 25);
}

void SimpleImGuiWindow::ProcessEvents(HWND a_Hwnd, UINT a_Msg, WPARAM a_wParam, LPARAM a_lParam)
{
	ImGui_ImplWin32_WndProcHandler(a_Hwnd, a_Msg, a_wParam, a_lParam);
}

void SimpleImGuiWindow::Resize(const glm::ivec2& a_Pos, const glm::ivec2& a_Size)
{
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize.x = a_Size.x;
	io.DisplaySize.y = a_Size.y;
}

void SimpleImGuiWindow::Render(std::shared_ptr<gallus::graphics::dx12::CommandList> a_CommandList)
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	m_FontEmbedderWindow.Render();

	ImGui::EndFrame();

	ImGui::Render();

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), a_CommandList->GetCommandList().Get());
}
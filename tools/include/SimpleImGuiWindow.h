#pragma once

#include <Windows.h>
#include <memory>
#include <glm/vec2.hpp>
#include "FontEmbedderWindow.h"

namespace gallus
{
	namespace graphics
	{
		namespace win32
		{
			class Window;
		}
		namespace dx12
		{
			class DX12System;
			class CommandList;
		}
	}
}

class SimpleImGuiWindow
{
public:
	void Initialize(HWND a_Hwnd, gallus::graphics::dx12::DX12System& a_System, gallus::graphics::win32::Window& a_Window);
	void CreateContextWin32();
	bool CreateContextDX12(gallus::graphics::dx12::DX12System& a_System);
	void CreateImGui() const;

	void ProcessEvents(HWND a_Hwnd, UINT a_Msg, WPARAM a_wParam, LPARAM a_lParam);
	void Resize(const glm::ivec2& a_Pos, const glm::ivec2& a_Size);
	void Render(std::shared_ptr<gallus::graphics::dx12::CommandList> a_CommandList);
private:
	HWND m_hWnd = nullptr;
	FontEmbedderWindow m_FontEmbedderWindow;
};
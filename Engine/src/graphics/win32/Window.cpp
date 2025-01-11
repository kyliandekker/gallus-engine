#include "graphics/win32/Window.h"

#include "core/Engine.h"
#include "core/logger/Logger.h"

#include <iostream>

namespace coopscoop
{
	namespace graphics
	{
		namespace win32
		{
#define CATEGORY_WINDOW "WINDOW"

			LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
			{
				return core::ENGINE.GetWindow().WndProcHandler(hwnd, msg, wParam, lParam);
			}

			bool Window::Initialize(bool a_Wait, HINSTANCE a_hInstance)
			{
				m_hInstance = a_hInstance;

				LOG(LOGSEVERITY_INFO, CATEGORY_WINDOW, "Initializing window.");
				return ThreadedSystem::Initialize(a_Wait);
			}

			LRESULT Window::WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
			{
				switch (msg)
				{
					case WM_DESTROY:
					{
						PostQuitMessage(0);
						core::ENGINE.Shutdown();
						return 0;
					}
				}
				return DefWindowProc(hwnd, msg, wParam, lParam);
			}

			void Window::Loop()
			{
				MSG msg = {};
				if (GetMessage(&msg, NULL, 0, 0) > 0)
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}

			bool Window::Destroy()
			{
				LOG(LOGSEVERITY_INFO, CATEGORY_WINDOW, "Destroying window.");
				return ThreadedSystem::Destroy();
			}

			void Window::Show()
			{
				::ShowWindow(m_hWnd, 1);
				::UpdateWindow(m_hWnd);
			}

			void Window::Hide()
			{
				::ShowWindow(m_hWnd, 0);
			}

			bool Window::IsFullScreen() const
			{
				return m_Fullscreen;
			}

			void Window::ToggleFullscreen()
			{
				m_Fullscreen = !m_Fullscreen;

				if (m_Fullscreen)
				{
					// Save the current window position and size
					::GetWindowRect(m_hWnd, &m_WindowRect);

					// Adjust window style to borderless fullscreen
					UINT windowStyle = WS_POPUP;
					::SetWindowLongW(m_hWnd, GWL_STYLE, windowStyle);

					// Get monitor information for fullscreen dimensions
					HMONITOR hMonitor = ::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
					MONITORINFO monitorInfo = {};
					monitorInfo.cbSize = sizeof(MONITORINFO);

					if (::GetMonitorInfo(hMonitor, &monitorInfo))
					{
						// Resize and reposition the window to cover the monitor
						::SetWindowPos(m_hWnd, HWND_TOP,
							monitorInfo.rcMonitor.left,
							monitorInfo.rcMonitor.top,
							monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
							monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
							SWP_FRAMECHANGED | SWP_NOACTIVATE);

						::ShowWindow(m_hWnd, SW_MAXIMIZE);
					}
				}
				else
				{
					// Restore the window style
					::SetWindowLongW(m_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

					// Restore the saved window position and size
					::SetWindowPos(m_hWnd, HWND_NOTOPMOST,
						m_WindowRect.left, m_WindowRect.top,
						m_WindowRect.right - m_WindowRect.left,
						m_WindowRect.bottom - m_WindowRect.top,
						SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOSENDCHANGING | SWP_SHOWWINDOW);

					// Restore the window to normal state
					::ShowWindow(m_hWnd, SW_NORMAL);
					::UpdateWindow(m_hWnd);           // Force immediate redraw
				}
			}

			void Window::SetTitle(const std::string& a_Title)
			{
				::SetWindowTextA(m_hWnd, a_Title.c_str());
			}

			glm::ivec2 Window::GetRealSize() const
			{
				RECT rect;
				GetClientRect(m_hWnd, &rect);

				return glm::ivec2(rect.right - rect.left, rect.bottom - rect.top);
			}

			void Window::SetSize(const glm::ivec2& a_Size)
			{
				RECT rect;
				// Get the current window position and size
				GetWindowRect(m_hWnd, &rect);

				// Adjust the window rectangle to account for the window's non-client area (borders, title bar, etc.)
				AdjustWindowRect(&rect, GetWindowLong(m_hWnd, GWL_STYLE), FALSE);

				ChangeSize(glm::ivec2(), a_Size);
			}

			void Window::SetPosition(const glm::ivec2& a_Position)
			{
				RECT rect;
				// Get the current window position and size
				GetWindowRect(m_hWnd, &rect);

				ChangeSize(a_Position, glm::ivec2(rect.right - rect.left, rect.bottom - rect.top));
			}

			void Window::ChangeSize(const glm::ivec2& a_Position, const glm::ivec2& a_Size)
			{
				SetWindowPos(
					m_hWnd,                // Handle to the window
					HWND_TOP,            // Placement order (HWND_TOP keeps it in its current Z-order)
					a_Position.x, 
					a_Position.y, 
					a_Size.x,
					a_Size.y, 
					SWP_NOMOVE | SWP_NOZORDER // Flags to retain position and Z-order
				);
			}

			HWND& Window::GetHWnd()
			{
				return m_hWnd;
			}

			WNDCLASSEX& Window::GetWc()
			{
				return m_Wc;
			}

			bool Window::InitializeThread()
			{
				CreateWindow(m_hInstance);

				LOG(LOGSEVERITY_SUCCESS, CATEGORY_WINDOW, "Initialized window.");

				return ThreadedSystem::InitializeThread();
			}

			void Window::Finalize()
			{
				::DestroyWindow(m_hWnd);
				::UnregisterClassA(m_Wc.lpszClassName, m_Wc.hInstance);

				ThreadedSystem::Finalize();

				LOG(LOGSEVERITY_SUCCESS, CATEGORY_WINDOW, "Destroyed window.");
			}

			bool Window::CreateWindow(HINSTANCE a_hInstance)
			{
				m_Wc = {};

				m_Wc.cbSize = sizeof(WNDCLASSEX);
				m_Wc.style = CS_HREDRAW | CS_VREDRAW;
				m_Wc.cbClsExtra = 0;
				m_Wc.cbWndExtra = 0;

				m_Wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
				m_Wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));

				m_Wc.hIcon = ::LoadIcon(a_hInstance, IDI_APPLICATION);
				m_Wc.hIconSm = ::LoadIcon(a_hInstance, IDI_APPLICATION);

				m_Wc.lpszClassName = "Window";
				m_Wc.lpszMenuName = NULL;

				m_Wc.hInstance = a_hInstance;

				m_Wc.lpfnWndProc = WndProc;

				if (!RegisterClassEx(&m_Wc))
				{
					LOG(LOGSEVERITY_ASSERT, CATEGORY_WINDOW, "Failed window registration.");
					return false;
				}

				SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

				m_hWnd = CreateWindowEx(
					WS_EX_CLIENTEDGE,
					"Window",
					"Game Window",
					WS_OVERLAPPEDWINDOW,
					CW_USEDEFAULT, CW_USEDEFAULT, 230, 400,
					NULL, NULL, a_hInstance, NULL);

				if (!m_hWnd)
				{
					LOG(LOGSEVERITY_ASSERT, CATEGORY_WINDOW, "Failed window creation.");
					return false;
				}

				Show();
				return true;
			}
		}
	}
}
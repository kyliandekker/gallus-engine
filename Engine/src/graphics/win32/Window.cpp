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

			void Window::Initialize(HINSTANCE a_hInstance, uint32_t a_Width, uint32_t a_Height, const std::string& a_WindowTitle)
			{
				m_hInstance = a_hInstance;
				m_WindowSize = glm::vec2(a_Width, a_Height);
				m_WindowTitle = a_WindowTitle;

				ThreadedSystem::Initialize();
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

			void Window::ToggleFullscreen()
			{
				RECT windowRect;

				m_Fullscreen = !m_Fullscreen;
				if (m_Fullscreen)
				{
					::GetWindowRect(m_hWnd, &windowRect);

					UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU |
						WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

					::SetWindowLongW(m_hWnd, GWL_STYLE, windowStyle);

					HMONITOR hMonitor = ::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
					MONITORINFOEX monitorInfo = {};
					monitorInfo.cbSize = sizeof(MONITORINFOEX);
					::GetMonitorInfo(hMonitor, &monitorInfo);
					::SetWindowPos(m_hWnd, HWND_TOP, monitorInfo.rcMonitor.left,
						monitorInfo.rcMonitor.top, monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
						monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
						SWP_FRAMECHANGED | SWP_NOACTIVATE);

					::ShowWindow(m_hWnd, SW_MAXIMIZE);
				}
				else
				{
					// Restore all the window decorators.
					::SetWindowLong(m_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

					::GetWindowRect(m_hWnd, &windowRect);

					::SetWindowPos(m_hWnd, HWND_NOTOPMOST, windowRect.left,
						windowRect.top, windowRect.right - windowRect.left,
						windowRect.bottom - windowRect.top, SWP_FRAMECHANGED | SWP_NOACTIVATE);

					::ShowWindow(m_hWnd, SW_NORMAL);
				}
			}

			void Window::SetTitle(const std::string& a_Title)
			{
				m_WindowTitle = a_Title;
				::SetWindowTextA(m_hWnd, m_WindowTitle.c_str());
			}

			glm::vec2 Window::GetRealSize() const
			{
				RECT rect;
				GetClientRect(m_hWnd, &rect);

				return glm::vec2(static_cast<float>(rect.right - rect.left), static_cast<float>(rect.bottom - rect.top));
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
				CreateWindow(m_hInstance, static_cast<uint32_t>(m_WindowSize.x), static_cast<uint32_t>(m_WindowSize.y), L"Game");
				SetTitle(m_WindowTitle);

				LOG(LOGSEVERITY_SUCCESS, CATEGORY_WINDOW, "Initialized window.");

				return ThreadedSystem::InitializeThread();
			}

			void Window::Finalize()
			{
				::DestroyWindow(m_hWnd);
				::UnregisterClassW(m_Wc.lpszClassName, m_Wc.hInstance);

				ThreadedSystem::Finalize();

				LOG(LOGSEVERITY_SUCCESS, CATEGORY_WINDOW, "Destroyed window.");
			}

			bool Window::CreateWindow(HINSTANCE a_hInstance, uint32_t a_Width, uint32_t a_Height, LPCWSTR a_WindowTitle)
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

				m_Wc.lpszClassName = L"Window";
				m_Wc.lpszMenuName = NULL;

				m_Wc.hInstance = a_hInstance;

				m_Wc.lpfnWndProc = WndProc;

				if (!RegisterClassEx(&m_Wc))
				{
					LOG(LOGSEVERITY_ASSERT, CATEGORY_WINDOW, "Failed window registration.");
					return false;
				}

				m_hWnd = CreateWindowEx(
					WS_EX_CLIENTEDGE,
					L"Window",
					a_WindowTitle,
					WS_MINIMIZEBOX | WS_SYSMENU | WS_SIZEBOX,
					CW_USEDEFAULT, CW_USEDEFAULT, a_Width, a_Height,
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
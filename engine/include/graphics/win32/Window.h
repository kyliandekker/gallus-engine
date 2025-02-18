#pragma once

#include <glm/vec2.hpp>
#include <Windows.h>
#include <string>
#include <queue>

#include "core/System.h"
#include "core/Event.h"

#if defined(CreateWindow)
#undef CreateWindow
#endif

namespace gallus
{
	namespace graphics
	{
		namespace win32
		{
			/// <summary>
			/// Represents a win32 window in the application, with functionality for event polling and rendering 
			/// </summary>
			class Window : public core::ThreadedSystem
			{
			public:
				/// <summary>
				/// Initializes the system, setting up necessary resources.
				/// </summary>
				/// <param name="a_Wait">Determines whether the application waits until the system has been fully initialized.</param>
				/// <param name="a_hInstance">Handle to the instance of the program.</param>
				/// <returns>True if the initialization was successful, otherwise false.</returns>
				bool Initialize(bool a_Wait, HINSTANCE a_hInstance);

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
				/// Loop method for the thread.
				/// </summary>
				void Loop() override;

				/// <summary>
				/// Signals the thread to stop.
				/// </summary>
				bool Destroy() override;

				/// <summary>
				/// Makes the window visible on the screen.
				/// </summary>
				void Show();

				/// <summary>
				/// Hides the window from the screen.
				/// </summary>
				void Hide();

				/// <summary>
				/// Retrieves the full screen state of the window.
				/// </summary>
				/// <returns>True if the screen is fullscreen, otherwise false.</returns>
				bool IsFullScreen() const;

				/// <summary>
				/// Toggles fullscreen mode for the window.
				/// </summary>
				void ToggleFullscreen();

				/// <summary>
				/// Sets the window title.
				/// </summary>
				/// <param name="a_Title">The new title for the window.</param>
				void SetTitle(const std::string& a_Title);

				/// <summary>
				/// Retrieves the real (physical) size of the window.
				/// </summary>
				/// <returns>A 2D vector representing the width and height of the window.</returns>
				glm::ivec2 GetRealSize() const;

				glm::ivec2 GetPosition() const;

				/// <summary>
				/// Sets the size of the window.
				/// </summary>
				/// <param name="a_Size">The size of the window</param>
				void SetSize(const glm::ivec2& a_Size);

				/// <summary>
				/// Sets the position of the window.
				/// </summary>
				/// <param name="a_Position">The position the window will be rendered at.</param>
				void SetPosition(const glm::ivec2& a_Position);

				/// <summary>
				/// Retrieves the handle to the window.
				/// </summary>
				/// <returns>A reference to the window handle (HWND).</returns>
				HWND& GetHWnd();

				/// <summary>
				/// Retrieves the handle to the instance.
				/// </summary>
				/// <returns>A reference to the instance handle (hInstance).</returns>
				HINSTANCE& GethInstance();

				/// <summary>
				/// Retrieves the windows class for the window.
				/// </summary>
				/// <returns></returns>
				WNDCLASSEX& GetWc();

				SimpleEvent<const glm::ivec2&, const glm::ivec2&> m_OnResize;
				SimpleEvent<> m_OnQuit;

				void SetCursor(LPTSTR a_Cursor);

				struct WindowsMsg
				{
					HWND hwnd;
					UINT msg;
					WPARAM wParam;
					LPARAM lParam;
				};
				std::queue<WindowsMsg> m_EventQueue;
				std::mutex g_EventMutex;
			private:
				LPTSTR m_Cursor = IDC_ARROW;

				/// <summary>
				/// Generic size method that changes the size and position of the window.
				/// </summary>
				/// <param name="a_Position">The position the window will be rendered at.</param>
				/// <param name="a_Size">The size of the window</param>
				void ChangeSize(const glm::ivec2& a_Position, const glm::ivec2& a_Size);

				/// <summary>
				/// Initializes the thread.
				/// </summary>
				/// <returns>True if the initialization was successful, otherwise false.</returns>
				bool InitializeThread() override;

				/// <summary>
				/// Destroys the system, releasing resources and performing necessary cleanup.
				/// </summary>
				/// <returns>True if the destruction was successful, otherwise false.</returns>
				void Finalize() override;

				/// <summary>
				/// Creates a win32 window.
				/// </summary>
				/// <param name="a_hInstance">The handle instance of the application.</param>
				/// <returns></returns>
				bool CreateWindow(HINSTANCE a_hInstance);

				HWND m_hWnd = NULL;
				WNDCLASSEX m_Wc = WNDCLASSEX();
				HINSTANCE m_hInstance = NULL;
				RECT m_WindowRect;

				bool m_Fullscreen = false;
			};
		}
	}
}
#pragma once

#include <glm/vec2.hpp>
#include <Windows.h>
#include <string>

#include "core/System.h"

#if defined(CreateWindow)
#undef CreateWindow
#endif

namespace coopscoop
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
				/// Initializes the window with the right parameters.
				/// </summary>
				/// <param name="a_hInstance">Handle to the instance of the program.</param>
				/// <param name="a_Width">The width of the window.</param>
				/// <param name="a_Height">The height of the window.</param>
				/// <param name="a_WindowTitle">The title of the window.</param>
				void Initialize(HINSTANCE a_hInstance, uint32_t a_Width, uint32_t a_Height, const std::string& a_WindowTitle);

				/// <summary>
				/// Handles Windows messages for the editor's window.
				/// </summary>
				/// <param name="a_hWnd">Handle to the window.</param>
				/// <param name="a_Msg">Message identifier.</param>
				/// <param name="a_wParam">Additional message information (WPARAM).</param>
				/// <param name="a_wlParam">Additional message information (LPARAM).</param>
				/// <returns>The result of the message processing.</returns>
				LRESULT CALLBACK WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

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
				glm::vec2 GetRealSize() const;

				/// <summary>
				/// Retrieves the handle to the window.
				/// </summary>
				/// <returns>A reference to the window handle (HWND).</returns>
				HWND& GetHWnd();

				/// <summary>
				/// Retrieves the windows class for the window.
				/// </summary>
				/// <returns></returns>
				WNDCLASSEX& GetWc();
			private:
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

				bool CreateWindow(HINSTANCE a_hInstance, uint32_t a_Width, uint32_t a_Height, LPCWSTR a_WindowTitle);

				HWND m_hWnd = NULL;
				WNDCLASSEX m_Wc = WNDCLASSEX();
				HINSTANCE m_hInstance = NULL;
				glm::ivec2 m_WindowSize;
				std::string m_WindowTitle;

				bool m_Fullscreen = false;
			};
		}
	}
}
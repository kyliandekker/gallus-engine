#pragma once

#include "core/System.h"
#include "graphics/win32/Window.h"
#include "core/input/InputSystem.h"

#include <wtypes.h>

namespace coopscoop
{
	namespace core
	{
		/// <summary>
		/// Represents the main engine class that handles the initialization, shutdown, 
		/// and access to various subsystems of the engine.
		/// </summary>
		class Engine : public System
		{
		public:
			/// <summary>
			/// Initializes the engine and all necessary subsystems with the specified parameters.
			/// </summary>
			/// <returns>True if the engine initializes successfully, otherwise false.</returns>
			bool Initialize(HINSTANCE a_hInstance, uint32_t a_Width, uint32_t a_Height, const std::string& a_Name);

			/// <summary>
			/// Shuts down the engine and cleans up all subsystems.
			/// </summary>
			/// <returns>True if the destruction is successful, otherwise false.</returns>
			bool Destroy() override;

			/// <summary>
			/// Shuts down the engine.
			/// </summary>
			void Shutdown();

			/// <summary>
			/// Retrieves the win32 window.
			/// </summary>
			/// <returns>Reference to the window instance.</returns>
			graphics::win32::Window& GetWindow();
		private:
			graphics::win32::Window m_Window;
			input::InputSystem m_InputSystem;
		};

		/// <summary>
		/// Global instance of the Engine.
		/// </summary>
		inline extern Engine ENGINE = {};
	}
}
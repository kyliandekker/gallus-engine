#pragma once

#include "core/System.h"
#include "graphics/dx12/DX12System.h"
#include "graphics/win32/Window.h"
#include "core/input/InputSystem.h"
#include "gameplay/EntityComponentSystem.h"
#ifdef _EDITOR
#include "editor/Editor.h"
#endif // _EDITOR

#include <wtypes.h>

namespace gallus
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

			/// <summary>
			/// Retrieves the dx12 system.
			/// </summary>
			/// <returns>Reference to the dx12 instance.</returns>
			graphics::dx12::DX12System& GetDX12();

			/// <summary>
			/// Retrieves the ecs.
			/// </summary>
			/// <returns>Reference to the ecs instance.</returns>
			gameplay::EntityComponentSystem& GetECS();

#ifdef _EDITOR
			editor::Editor& GetEditor();
#endif // _EDITOR
		private:
			graphics::win32::Window m_Window;
			graphics::dx12::DX12System m_DX12System;
			input::InputSystem m_InputSystem;
			gameplay::EntityComponentSystem m_ECS;
#ifdef _EDITOR
			editor::Editor m_Editor;
#endif // _EDITOR
		};

		/// <summary>
		/// Global instance of the Engine.
		/// </summary>
		inline extern Engine ENGINE = {};
	}
}
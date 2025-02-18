#include "core/Engine.h"

#include "core/logger/Logger.h"

namespace gallus
{
	namespace core
	{
#define CATEGORY_ENGINE "ENGINE"

		bool Engine::Initialize(HINSTANCE a_hInstance, uint32_t a_Width, uint32_t a_Height, const std::string& a_Name)
		{
#ifdef _OPTIMIZE
			std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
#endif

			// Initialize logger.
			// Logger is a global var unlike all the other systems. Not the prettiest but not too bad either.
			logger::LOGGER.Initialize(true);

			LOG(LOGSEVERITY_INFO, CATEGORY_ENGINE, "Initializing engine.");

			// Initialize the input system, we do not need to wait until it is ready.
			m_InputSystem.Initialize(false);

#ifdef _EDITOR
			m_Editor.Initialize(true);
			glm::vec2 size = m_Editor.GetEditorSettings().Size();
#endif

			// We initialize the window first and set the size and title after it has been created.
			m_Window.m_OnQuit += std::bind(&Engine::Shutdown, this);
			m_Window.Initialize(true, a_hInstance);
			m_Window.SetSize(glm::ivec2(a_Width, a_Height));
			m_Window.SetTitle(a_Name);

#ifdef _EDITOR
			m_Window.SetSize(size);
#endif // _EDITOR

			// TODO: We can initialize DX12 without having to wait for it for now. Later when we introduce the editor it might be important to wait.
			m_DX12System.Initialize(true, m_Window.GetHWnd(), m_Window.GetRealSize(), &m_Window);

			m_ECS.Initialize();

			System::Initialize();

			LOG(LOGSEVERITY_SUCCESS, CATEGORY_ENGINE, "Initialized engine.");

#ifdef _OPTIMIZE
			std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
			std::string microseconds = "Initialization took " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) + " microseconds.";
			std::string nanoseconds = "Initialization took " + std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()) + " nanoseconds.";
			std::string milliseconds = "Initialization took " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()) + " milliseconds.";
			std::string seconds = "Initialization took " + std::to_string(std::chrono::duration_cast<std::chrono::seconds>(end - begin).count()) + " seconds.";

			TEST(microseconds.c_str());
			TEST(nanoseconds.c_str());
			TEST(milliseconds.c_str());
			TEST(seconds.c_str());
#endif

			while (m_Ready.load())
			{
				m_ECS.Update(0);
			}

			return true;
		}

		bool Engine::Destroy()
		{
			LOG(LOGSEVERITY_INFO, CATEGORY_ENGINE, "Destroying engine.");

			m_InputSystem.Destroy();

			m_DX12System.Destroy();

#ifdef _EDITOR
			m_Editor.Destroy();
#endif // _EDITOR

			m_Window.Destroy();

			// Destroy the logger last so we can see possible error messages from other systems.
			logger::LOGGER.Destroy();

			return System::Destroy();
		}

		void Engine::Shutdown()
		{
			m_Ready.store(false);
		}

		graphics::win32::Window& Engine::GetWindow()
		{
			return m_Window;
		}

		graphics::dx12::DX12System& Engine::GetDX12()
		{
			return m_DX12System;
		}

		gameplay::EntityComponentSystem& Engine::GetECS()
		{
			return m_ECS;
		}

#ifdef _EDITOR
		editor::Editor& Engine::GetEditor()
		{
			return m_Editor;
		}
#endif // _EDITOR
	}
}
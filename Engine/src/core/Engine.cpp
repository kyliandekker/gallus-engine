#include "core/Engine.h"

#include "core/logger/Logger.h"

namespace coopscoop
{
	namespace core
	{
#define CATEGORY_ENGINE "ENGINE"

		bool Engine::Initialize(HINSTANCE a_hInstance, uint32_t a_Width, uint32_t a_Height, const std::string& a_Name)
		{
			// Initialize logger.
			// Logger is a global var unlike all the other systems. Not the prettiest but not too bad either.
			logger::LOGGER.Initialize();

			LOG(LOGSEVERITY_INFO, CATEGORY_ENGINE, "Initializing engine.");

			// We initialize the window first and set the size and title after it has been created.
			m_Window.Initialize(a_hInstance);
			m_Window.SetSize(glm::ivec2(a_Width, a_Height));
			m_Window.SetTitle(a_Name);

			m_InputSystem.Initialize();

			System::Initialize();

			LOG(LOGSEVERITY_SUCCESS, CATEGORY_ENGINE, "Initialized engine.");

			while (m_Ready.load())
			{
				if (m_InputSystem.IsKeyPressed('F'))
				{
					TEST();
				}
			}

			return true;
		}

		bool Engine::Destroy()
		{
			LOG(LOGSEVERITY_INFO, CATEGORY_ENGINE, "Destroying engine.");

			m_Window.Destroy();

			m_InputSystem.Destroy();

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
	}
}
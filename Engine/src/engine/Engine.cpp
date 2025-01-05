#include "engine/Engine.h"

#include "engine/logger/Logger.h"

namespace coopscoop
{
	namespace engine
	{
		bool Engine::Initialize(HINSTANCE a_hInstance, uint32_t a_Width, uint32_t a_Height, LPCWSTR a_Name)
		{
			// Initialize logger.
			// Logger is a global var unlike all the other systems. Not the prettiest but not too bad either.
			logger::LOGGER.Initialize();
			// Wait until the system is ready.
			while (!logger::LOGGER.Ready())
			{
				// Wait...
				std::this_thread::yield();
			}

			System::Initialize();

			while (m_Ready)
			{

			}
			return true;
		}

		bool Engine::Destroy()
		{
			logger::LOGGER.Destroy();

			return System::Destroy();
		}
	}
}
#include "engine/Engine.h"

#include "engine/logger/Logger.h"

namespace coopscoop
{
	namespace engine
	{
#define CATEGORY_ENGINE "ENGINE"

		bool Engine::Initialize(HINSTANCE a_hInstance, uint32_t a_Width, uint32_t a_Height, LPCWSTR a_Name)
		{
			// Initialize logger.
			// Logger is a global var unlike all the other systems. Not the prettiest but not too bad either.
			logger::LOGGER.Initialize();

			LOG(LOGSEVERITY_INFO, CATEGORY_ENGINE, "Initializing engine.");

			System::Initialize();

			LOG(LOGSEVERITY_SUCCESS, CATEGORY_ENGINE, "Initialized engine.");

			return true;
		}

		bool Engine::Destroy()
		{
			LOG(LOGSEVERITY_INFO, CATEGORY_ENGINE, "Destroying engine.");

			logger::LOGGER.Stop();

			return System::Destroy();
		}
	}
}
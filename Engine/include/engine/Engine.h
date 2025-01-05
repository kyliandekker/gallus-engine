#pragma once

#include "./System.h"
#include <wtypes.h>

namespace coopscoop
{
	namespace engine
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
			bool Initialize(HINSTANCE a_hInstance, uint32_t a_Width, uint32_t a_Height, LPCWSTR a_Name);

			/// <summary>
			/// Shuts down the engine and cleans up all subsystems.
			/// </summary>
			/// <returns>True if the destruction is successful, otherwise false.</returns>
			bool Destroy() override;
		};

		/// <summary>
		/// Global instance of the Engine.
		/// </summary>
		inline extern Engine ENGINE = {};
	}
}
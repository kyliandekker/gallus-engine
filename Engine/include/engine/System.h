#pragma once

#include <thread>

namespace coopscoop
{
	namespace engine
	{
		/// <summary>
		/// Base class for all subsystems within the engine, providing a common interface 
		/// for initialization, destruction, and readiness checking.
		/// </summary>
		class System
		{
		public:
			/// <summary>
			/// Initializes the subsystem, setting up necessary resources.
			/// </summary>
			/// <returns>True if the initialization was successful, otherwise false.</returns>
			virtual bool Initialize();

			/// <summary>
			/// Destroys the subsystem, releasing resources and performing necessary cleanup.
			/// </summary>
			/// <returns>True if the destruction was successful, otherwise false.</returns>
			virtual bool Destroy();

			/// <summary>
			/// Checks whether the subsystem is ready for use.
			/// </summary>
			/// <returns>True if the subsystem is ready, otherwise false.</returns>
			bool Ready() const;
		protected:
			bool m_Ready = false; /// Flag indicating whether the subsystem is ready.
		};

		/// <summary>
		/// Base class for all subsystems within the engine, providing a common interface 
		/// for initialization, destruction, and readiness checking.
		/// Added functionality for initializing on a separate thread.
		/// </summary>
		class ThreadedSystem
		{
		public:
			/// <summary>
			/// Initializes the subsystem, setting up necessary resources.
			/// </summary>
			/// <returns>True if the initialization was successful, otherwise false.</returns>
			virtual bool Initialize();

			/// <summary>
			/// Destroys the subsystem, releasing resources and performing necessary cleanup.
			/// </summary>
			/// <returns>True if the destruction was successful, otherwise false.</returns>
			virtual bool Destroy();

			/// <summary>
			/// Checks whether the subsystem is ready for use.
			/// </summary>
			/// <returns>True if the subsystem is ready, otherwise false.</returns>
			bool Ready() const
			{
				return m_Ready.load();
			}
		protected:
			/// <summary>
			/// Initializes the thread.
			/// </summary>
			/// <returns>True if the initialization was successful, otherwise false.</returns>
			virtual bool InitializeThread();

			std::thread m_Thread; /// The thread.
			std::atomic<bool> m_Ready{ false }; /// Flag indicating whether the subsystem is ready.
		};
	}
}
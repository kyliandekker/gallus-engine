#pragma once

#include <thread>
#include <mutex>

namespace gallus
{
	namespace core
	{
		/// <summary>
		/// Base class for all systems within the engine, providing a common interface 
		/// for initialization, destruction, and readiness checking.
		/// </summary>
		class System
		{
		public:
			/// <summary>
			/// Initializes the system, setting up necessary resources.
			/// </summary>
			/// <returns>True if the initialization was successful, otherwise false.</returns>
			virtual bool Initialize();

			/// <summary>
			/// Destroys the system, releasing resources and performing necessary cleanup.
			/// </summary>
			/// <returns>True if the destruction was successful, otherwise false.</returns>
			virtual bool Destroy();

			/// <summary>
			/// Checks whether the system is ready for use.
			/// </summary>
			/// <returns>True if the system is ready, otherwise false.</returns>
			bool Ready() const;
		protected:
			std::atomic<bool> m_Ready{ false }; /// Flag indicating whether the system is ready.
		};

		/// <summary>
		/// Base class for all threaded systems within the engine, providing a common interface 
		/// for initialization, destruction, and readiness checking.
		/// </summary>
		class ThreadedSystem : public System
		{
		public:
			/// <summary>
			/// Initializes the system, setting up necessary resources.
			/// </summary>
			/// <param name="a_Wait">Determines whether the application waits until the system has been fully initialized.</param>
			/// <returns>True if the initialization was successful, otherwise false.</returns>
			virtual bool Initialize(bool a_Wait);

			/// <summary>
			/// Signals the thread to stop.
			/// </summary>
			bool Destroy() override;

			/// <summary>
			/// Loop method for the thread.
			/// </summary>
			virtual void Loop() = 0;
		protected:
			/// <summary>
			/// Destroys the system, releasing resources and performing necessary cleanup.
			/// </summary>
			/// <returns>True if the destruction was successful, otherwise false.</returns>
			virtual void Finalize();

			/// <summary>
			/// Initializes the thread.
			/// </summary>
			/// <returns>True if the initialization was successful, otherwise false.</returns>
			virtual bool InitializeThread();

			std::atomic<bool> m_Stop{ false }; /// Flag indicating whether the system needs to be destroyed.

			std::thread m_Thread; /// The thread.

			std::mutex m_ReadyMutex; /// The mutex used for synchronization between the threads for stopping or initializing.
			std::condition_variable m_ReadyCondVar; /// The condition var used for synchronization between the threads for stopping or initializing.
		};
	}
}
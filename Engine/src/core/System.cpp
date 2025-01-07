#include "core/System.h"

namespace coopscoop
{
	namespace core
	{
		/*
			* System
		*/

#pragma region SYSTEM

		bool System::Initialize()
		{
			m_Ready.store(true);
			return true;
		}

		bool System::Destroy()
		{
			m_Ready.store(false);
			return true;
		}

		bool System::Ready() const
		{
			return m_Ready.load();
		}

#pragma endregion SYSTEM

		/*
			* Threaded System
		*/

#pragma region THREADED_SYSTEM

		bool ThreadedSystem::Initialize()
		{
			// NOTE: This function is always called from the main thread.

			// Start the thread and wait afterwards.
			m_Thread = std::thread(&ThreadedSystem::InitializeThread, this);

			// Wait until the system is ready
			{
				std::unique_lock lock(m_ReadyMutex); // Lock the mutex
				m_ReadyCondVar.wait(lock, [this]() { return m_Ready.load(); });
			}

			return true;
		}

		void ThreadedSystem::Finalize()
		{
			// NOTE: This function is always called from the system's thread.

			{
				std::scoped_lock lock(m_ReadyMutex); // Lock the mutex
				m_Ready.store(false);
				m_ReadyCondVar.notify_one(); // Notify the waiting thread that the system has been destroyed successfully.
			}
		}

		bool ThreadedSystem::Destroy()
		{
			// NOTE: This function is always called from the main thread.

			// Signal the thread that it needs to stop.
			m_Stop.store(true);

			// Wait until the system has stopped.
			std::unique_lock lock(m_ReadyMutex);
			m_ReadyCondVar.wait(lock, [this]() { return m_Ready.load() == false; });

			// Join the threads.
			if (m_Thread.joinable())
			{
				m_Thread.join();
			}
			return true;
		}

		bool ThreadedSystem::InitializeThread()
		{
			// NOTE: This function is always called from the system's thread.

			{
				std::scoped_lock lock(m_ReadyMutex); // Lock the mutex
				m_Ready.store(true);
				m_ReadyCondVar.notify_one(); // Notify the waiting thread that the system has been initialized.
			}

			// Loop while the system is ready.
			while (m_Ready.load())
			{
				Loop();

				// Check if the main thread wants the system to stop.
				if (m_Stop.load())
				{
					// Destroy all resources (virtual function that gets overridden, base function sets ready to false and notifies main thread).
					Finalize();
				}
			}
			return true;
		}

#pragma endregion THREADED_SYSTEM
	}
}
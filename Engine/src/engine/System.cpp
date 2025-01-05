#include "engine/System.h"

namespace coopscoop
{
	namespace engine
	{
		bool System::Initialize()
		{
			m_Ready = true;
			return true;
		}

		bool System::Destroy()
		{
			m_Ready = false;
			return true;
		}

		bool System::Ready() const
		{
			return m_Ready;
		}

		bool ThreadedSystem::Initialize()
		{
			m_Thread = std::thread(&ThreadedSystem::InitializeThread, this);

			// Wait until the system is ready
			{
				std::unique_lock lock(m_ReadyMutex); // Lock the mutex
				m_ReadyCondVar.wait(lock, [this]() { return m_Ready.load(); });
			}

			return true;
		}

		void ThreadedSystem::Destroy()
		{
			{
				std::scoped_lock lock(m_ReadyMutex); // Lock the mutex
				m_Ready.store(false);
				m_ReadyCondVar.notify_one(); // Notify the waiting thread
			}
		}

		void ThreadedSystem::Stop()
		{
			m_Stop = true;

			// Wait until the system is ready.
			std::unique_lock lock(m_ReadyMutex);
			m_ReadyCondVar.wait(lock, [this]() { return !m_Ready.load(); });

			if (m_Thread.joinable())
			{
				m_Thread.join();
			}
		}

		void ThreadedSystem::Loop()
		{ }

		bool ThreadedSystem::InitializeThread()
		{
			{
				std::scoped_lock lock(m_ReadyMutex); // Lock the mutex
				m_Ready.store(true);
				m_ReadyCondVar.notify_one(); // Notify the waiting thread
			}

			while (m_Ready.load())
			{
				Loop();
				// Stop post message.
				if (m_Stop)
				{
					Destroy();

					m_Ready.store(false); // Use atomic store
				}
			}
			return true;
		}
	}
}
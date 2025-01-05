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
			return false;
		}

		bool ThreadedSystem::Destroy()
		{
			m_Ready.store(false); // Use atomic store

			if (m_Thread.joinable())
			{
				m_Thread.join();
			}
			return true;
		}

		bool ThreadedSystem::InitializeThread()
		{
			m_Ready.store(true);
			return true;
		}
	}
}
#include "core/input/InputSystem.h"

#include "core/logger/Logger.h"

#include <Windows.h>

namespace coopscoop
{
	namespace core
	{
		namespace input
		{
			void Key::Update(char a_Key)
			{
				m_PreviousState = m_Pressed;
				m_Pressed = GetKeyState(a_Key) & 0x8000;
			}

			bool Key::GetKeyDown(char a_Key) const
			{
				return !m_PreviousState && m_Pressed;
			}

			bool Key::GetKey(char a_Key) const
			{
				return m_PreviousState && m_Pressed;
			}

			bool Key::IsReleased(char a_Key) const
			{
				return m_PreviousState && !m_Pressed;
			}

			bool InputSystem::Initialize(bool a_Wait)
			{
				m_MappedKeys.insert({ 'F', Key() });
				m_MappedKeys.insert({ VK_F11, Key() });

				LOG(LOGSEVERITY_INFO, LOG_CATEGORY_INPUT, "Initializing input system.");
				return ThreadedSystem::Initialize(a_Wait);
			}

			void InputSystem::Loop()
			{
				for (auto& key : m_MappedKeys)
				{
					key.second.Update(key.first);
				}
			}

			bool InputSystem::Destroy()
			{
				LOG(LOGSEVERITY_INFO, LOG_CATEGORY_INPUT, "Destroying input system.");
				return ThreadedSystem::Destroy();
			}

			bool InputSystem::IsKeyHeld(char a_Key) const
			{
				auto it = m_MappedKeys.find(a_Key);
				if (it != m_MappedKeys.end())
				{
					return it->second.GetKey(a_Key);
				}
				return false;
			}

			bool InputSystem::IsKeyPressed(char a_Key) const
			{
				auto it = m_MappedKeys.find(a_Key);
				if (it != m_MappedKeys.end())
				{
					return it->second.GetKeyDown(a_Key);
				}
				return false;
			}

			bool InputSystem::IsKeyReleased(char a_Key) const
			{
				auto it = m_MappedKeys.find(a_Key);
				if (it != m_MappedKeys.end())
				{
					return it->second.IsReleased(a_Key);
				}
				return false;
			}

			bool InputSystem::InitializeThread()
			{
				LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_INPUT, "Initialized input system.");
				return ThreadedSystem::InitializeThread();
			}

			void InputSystem::Finalize()
			{
				LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_INPUT, "Destroyed input system.");
				ThreadedSystem::Finalize();
			}
		}
	}
}
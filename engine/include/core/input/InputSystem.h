#pragma once

#include "core/System.h"

#include <unordered_map>

namespace gallus
{
	namespace core
	{
		namespace input
		{
			class Key
			{
			private:
				bool m_Pressed = false;
				bool m_PreviousState = m_Pressed;

				void Update(char a_Key);

				friend class InputSystem;
			public:
				bool GetKeyDown(char a_Key) const;
				bool GetKey(char a_Key) const;
				bool IsReleased(char a_Key) const;
			};

			class InputSystem : public ThreadedSystem
			{
			public:
				/// <summary>
				/// Initializes the window with the right parameters.
				/// </summary>
				/// <param name="a_Wait">Determines whether the application waits until the system has been fully initialized.</param>
				bool Initialize(bool a_Wait) override;

				/// <summary>
				/// Loop method for the thread.
				/// </summary>
				void Loop() override;

				/// <summary>
				/// Signals the thread to stop.
				/// </summary>
				bool Destroy() override;

				/// <summary>
				/// Checks whether the key is held down by the user.
				/// </summary>
				/// <param name="a_Key">The key that needs to be checked.</param>
				/// <returns>True if the key is being held down, otherwise false.</returns>
				bool IsKeyHeld(char a_Key) const;

				/// <summary>
				/// Checks whether the key is pressed once by the user.
				/// </summary>
				/// <param name="a_Key">The key that needs to be checked.</param>
				/// <returns>True if the key is being pressed down, otherwise false.</returns>
				bool IsKeyPressed(char a_Key) const;

				/// <summary>
				/// Checks whether the key is released by the user.
				/// </summary>
				/// <param name="a_Key">The key that needs to be checked.</param>
				/// <returns>True if the key is released, otherwise false.</returns>
				bool IsKeyReleased(char a_Key) const;
			private:
				/// <summary>
				/// Initializes the thread.
				/// </summary>
				/// <returns>True if the initialization was successful, otherwise false.</returns>
				bool InitializeThread() override;

				/// <summary>
				/// Destroys the system, releasing resources and performing necessary cleanup.
				/// </summary>
				/// <returns>True if the destruction was successful, otherwise false.</returns>
				void Finalize() override;

				std::unordered_map<char, Key> m_MappedKeys;
			};
		}
	}
}
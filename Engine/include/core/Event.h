#pragma once

#include <vector>
#include <functional>

namespace coopscoop
{
	/// <summary>
	/// A generic event class that supports multiple listeners and can notify them with provided arguments.
	/// </summary>
	/// <typeparam name="Args">The types of arguments that will be passed to the listeners when the event is triggered.</typeparam>
	template<typename... Args>
	class Event {
	public:
		/// <summary>
		/// Adds a listener to the event.
		/// The listener is added only if it has not been added previously.
		/// </summary>
		/// <param name="a_Listener">The listener function to add.</param>
		void operator+=(const std::function<void(Args...)>& a_Listener)
		{
			auto it = std::find_if(m_Listeners.begin(), m_Listeners.end(),
				[&](const std::function<void(Args...)>& other)
				{
					return a_Listener.target_type() == other.target_type();
				});

			if (it != m_Listeners.end())
			{
				return;
			}
			m_Listeners.push_back(a_Listener);
		}

		/// <summary>
		/// Removes a listener from the event.
		/// If the listener is not found, no action is taken.
		/// </summary>
		/// <param name="a_Listener">The listener function to remove.</param>
		void operator-=(const std::function<void(Args...)>& a_Listener)
		{
			auto it = std::find_if(m_Listeners.begin(), m_Listeners.end(),
				[&](const std::function<void(Args...)>& other)
				{
					return a_Listener.target_type() == other.target_type();
				});

			if (it != m_Listeners.end())
			{
				m_Listeners.erase(it);
			}
		}

		/// <summary>
		/// Invokes all the listeners with the provided arguments.
		/// </summary>
		/// <param name="args">The arguments to pass to the listeners when the event is triggered.</param>
		void operator()(Args... args) const
		{
			invoke(args...);
		}

		/// <summary>
		/// Invokes all the listeners with the provided arguments.
		/// </summary>
		/// <param name="args">The arguments to pass to the listeners when the event is triggered.</param>
		void invoke(Args... args) const
		{
			for (auto& listener : m_Listeners)
			{
				if (listener)
				{
					listener(args...);
				}
			}
		}

		/// <summary>
		/// Clears all the listeners from the event.
		/// </summary>
		void clear()
		{
			m_Listeners.clear();
		}

	private:
		std::vector<std::function<void(Args...)>> m_Listeners; /// The list of listeners subscribed to the event.
	};

	/// <summary>
	/// A simplified event class that supports only one listener at a time.
	/// </summary>
	/// <typeparam name="Args">The types of arguments that will be passed to the listener when the event is triggered.</typeparam>
	template<typename... Args>
	class SimpleEvent {
	public:

		/// <summary>
		/// Adds a listener to the event.
		/// Replaces the existing listener if one is already present.
		/// </summary>
		/// <param name="a_Listener">The listener function to add.</param>
		void operator+=(const std::function<void(Args...)>& a_Listener)
		{
			listener = a_Listener;
		}

		/// <summary>
		/// Removes the listener from the event.
		/// </summary>
		/// <param name="a_Listener">The listener function to remove.</param>
		void operator-=(const std::function<void(Args...)>& a_Listener)
		{
			listener = nullptr;
		}

		/// <summary>
		/// Invokes the listener with the provided arguments.
		/// </summary>
		/// <param name="args">The arguments to pass to the listener when the event is triggered.</param>
		void operator()(Args... args) const
		{
			invoke(args...);
		}

		/// <summary>
		/// Invokes the listener with the provided arguments.
		/// </summary>
		/// <param name="args">The arguments to pass to the listener when the event is triggered.</param>
		void invoke(Args... args) const
		{
			if (listener)
			{
			listener(args...);
			}
		}

		/// <summary>
		/// Clears the listener from the event.
		/// </summary>
		void clear()
		{
			listener = nullptr;
		}

	private:
		std::function<void(Args...)> listener; /// The single listener for the event.
	};
}
#pragma once

#ifdef __EDITOR__

#include "editor/imgui/windows/BaseWindow.h"

#include <vector>
#include <mutex>

#include "core/logger/Logger.h"
#include "editor/imgui/views/DataTypes/StringTextInput.h"

namespace coopscoop
{
	namespace editor
	{
		namespace imgui
		{
			inline std::mutex MESSAGE_MUTEX;

			class ImGuiWindow;

			/// <summary>
			/// A window that displays and manages the logger messages of the project.
			/// </summary>
			class ConsoleWindow : public BaseWindow
			{
			public:
				/// <summary>
				/// Constructs a console window.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				ConsoleWindow(ImGuiWindow& a_Window);

				/// <summary>
				/// Cleans up and destroys the console window.
				/// </summary>
				~ConsoleWindow();

				/// <summary>
				/// Renders the console window.
				/// </summary>
				void Render() override;

				/// <summary>
				/// Initializes all values and behaviours associated with the console window.
				/// </summary>
				/// <returns>True if initialization is successful, otherwise false.</returns>
				bool Initialize() override;

				/// <summary>
				/// Destroys and disables the console window.
				/// </summary>
				/// <returns>True if destruction is successful, otherwise false.</returns>
				bool Destroy() override;

				/// <summary>
				/// Clears all console messages from the window.
				/// </summary>
				void Clear();

				/// <summary>
				/// Adds a new console message to the console window.
				/// </summary>
				/// <param name="a_Message"></param>
				void AddMessage(const core::logger::LoggerMessage& a_Message);

				/// <summary>
				/// Callback used to retrieve messages from the logger.
				/// </summary>
				/// <param name="a_Message"></param>
				void LoggerCallback(const core::logger::LoggerMessage& a_Message);
			private:
				bool m_NeedsRefresh = true; /// Whether the console needs to refresh the results shown in the console window.
				std::vector<core::logger::LoggerMessage> m_Messages; /// List of messages retrieved from the logger.
				std::vector<size_t> m_FilteredMessages; /// List of messages shown in the console window.

				SearchBarInput m_SearchBar; /// Search bar to filter specific messages in the console window.
			};
		}
	}
}

#endif // __EDITOR__
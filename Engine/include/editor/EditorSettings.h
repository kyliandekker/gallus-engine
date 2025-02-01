#pragma once

#ifdef __EDITOR__

#include <string>
#include <unordered_set>
#include <glm/vec2.hpp>

#define DEFAULT_SIZE_X 1080
#define DEFAULT_SIZE_Y 720

namespace coopscoop
{
	namespace editor
	{
		/// <summary>
		/// Manages editor settings, including window size, log visibility filters, 
		/// and tracking previously opened projects.
		/// </summary>
		class EditorSettings
		{
		public:
			/// <summary>
			/// Loads settings from a persistent storage.
			/// </summary>
			/// <returns>True if the settings were loaded successfully, otherwise false.</returns>
			bool Load();

			/// <summary>
			/// Saves the current settings to persistent storage.
			/// </summary>
			/// <returns>True if the settings were saved successfully, otherwise false.</returns>
			bool Save() const;

			/// <summary>
			/// Sets the editor's window size.
			/// </summary>
			/// <param name="a_Size">The new size as a glm::vec2.</param>
			void SetSize(const glm::vec2& a_Size);

			/// <summary>
			/// Retrieves the current editor window size.
			/// </summary>
			/// <returns>The current window size as a glm::vec2.</returns>
			const glm::vec2& Size() const;

			/// <summary>
			/// Sets whether the console should automatically scroll to the bottom.
			/// </summary>
			/// <param name="a_ScrollToBottom">True to enable auto-scrolling, otherwise false.</param>
			void SetScrollToBottom(bool a_ScrollToBottom);

			/// <summary>
			/// Checks if the console is set to auto-scroll to the bottom.
			/// </summary>
			/// <returns>True if auto-scrolling is enabled, otherwise false.</returns>
			bool ScrollToBottom() const;

			/// <summary>
			/// Sets whether info log messages are displayed.
			/// </summary>
			/// <param name="a_Info">True to show info messages, otherwise false.</param>
			void SetInfo(bool a_Info);

			/// <summary>
			/// Checks if info log messages are displayed.
			/// </summary>
			/// <returns>True if info messages are visible, otherwise false.</returns>
			bool Info() const;

			/// <summary>
			/// Sets whether warning log messages are displayed.
			/// </summary>
			/// <param name="a_Warning">True to show warnings, otherwise false.</param>
			void SetWarning(bool a_Warning);

			/// <summary>
			/// Checks if warning log messages are displayed.
			/// </summary>
			/// <returns>True if warnings are visible, otherwise false.</returns>
			bool Warning() const;

			/// <summary>
			/// Sets whether error log messages are displayed.
			/// </summary>
			/// <param name="a_Error">True to show errors, otherwise false.</param>
			void SetError(bool a_Error);

			/// <summary>
			/// Checks if error log messages are displayed.
			/// </summary>
			/// <returns>True if errors are visible, otherwise false.</returns>
			bool Error() const;

			/// <summary>
			/// Sets whether assertion log messages are displayed.
			/// </summary>
			/// <param name="a_Assert">True to show assertion messages, otherwise false.</param>
			void SetAssert(bool a_Assert);

			/// <summary>
			/// Checks if assertion log messages are displayed.
			/// </summary>
			/// <returns>True if assertions are visible, otherwise false.</returns>
			bool Assert() const;

			/// <summary>
			/// Sets whether success log messages are displayed.
			/// </summary>
			/// <param name="a_Success">True to show success messages, otherwise false.</param>
			void SetSuccess(bool a_Success);

			/// <summary>
			/// Checks if success log messages are displayed.
			/// </summary>
			/// <returns>True if success messages are visible, otherwise false.</returns>
			bool Success() const;

			/// <summary>
			/// Sets whether combined info and success log messages are displayed.
			/// </summary>
			/// <param name="a_InfoSuccess">True to show combined info-success messages, otherwise false.</param>
			void SetInfoSuccess(bool a_InfoSuccess);

			/// <summary>
			/// Checks if combined info and success log messages are displayed.
			/// </summary>
			/// <returns>True if combined info-success messages are visible, otherwise false.</returns>
			bool InfoSuccess() const;

			/// <summary>
			/// Sets whether "awesome" log messages are displayed.
			/// </summary>
			/// <param name="a_Awesome">True to show "awesome" messages, otherwise false.</param>
			void SetAwesome(bool a_Awesome);

			/// <summary>
			/// Checks if "awesome" log messages are displayed.
			/// </summary>
			/// <returns>True if "awesome" messages are visible, otherwise false.</returns>
			bool Awesome() const;

			/// <summary>
			/// Sets the list of previously opened projects.
			/// </summary>
			/// <param name="a_PreviousProjects">The set of previous project paths.</param>
			void SetPreviousProjects(const std::unordered_set<std::string>& a_PreviousProjects);

			/// <summary>
			/// Retrieves the list of previously opened projects.
			/// </summary>
			/// <returns>A const reference to the set of previous project paths.</returns>
			const std::unordered_set<std::string>& GetPreviousProjects() const;

			/// <summary>
			/// Adds a project path to the list of previously opened projects.
			/// </summary>
			/// <param name="a_PreviousProject">The project path to add.</param>
			void AddToPreviousProjects(const std::string& a_PreviousProject);

			/// <summary>
			/// Removes a project path from the list of previously opened projects.
			/// </summary>
			/// <param name="a_PreviousProject">The project path to remove.</param>
			void ErasePreviousProject(const std::string& a_PreviousProject);
		private:
			bool m_ScrollToBottom = false; /// Auto-scroll setting for the console.
			bool m_Info = true; /// Visibility toggle for info log messages.
			bool m_Warning = true; /// Visibility toggle for warning log messages.
			bool m_Error = true; /// Visibility toggle for error log messages.
			bool m_Assert = true; /// Visibility toggle for assertion log messages.
			bool m_Success = true; /// Visibility toggle for success log messages.
			bool m_InfoSuccess = true; /// Visibility toggle for combined info-success messages.
			bool m_Awesome = true; /// Visibility toggle for "awesome" log messages.

			glm::vec2 m_Size = glm::vec2(DEFAULT_SIZE_X, DEFAULT_SIZE_Y); /// The editor window size.
			std::unordered_set<std::string> m_PreviousProjects; /// A set of previously opened project paths.
		};
	}
}

#endif // __EDITOR__
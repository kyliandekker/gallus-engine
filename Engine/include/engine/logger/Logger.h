#pragma once

#include "engine/System.h"

#include <assert.h>
#include <queue>
#include <thread>
#include <string>
#include <mutex>

namespace coopscoop
{
	namespace engine
	{
		typedef enum LogSeverity {
			LOGSEVERITY_ASSERT,
			LOGSEVERITY_ERROR,
			LOGSEVERITY_WARNING,
			LOGSEVERITY_INFO,
			LOGSEVERITY_SUCCESS,
			LOGSEVERITY_INFO_SUCCESS,
			LOGSEVERITY_AWESOME,
		} LogSeverity;

		/// <summary>
		/// Converts a log severity enumeration value to its corresponding string representation.
		/// </summary>
		/// <param name="a_LogSeverity">The log severity to convert.</param>
		/// <returns>A string representing the specified log severity.</returns>
		inline const std::string LogSeverityToString(LogSeverity a_LogSeverity)
		{
			switch (a_LogSeverity)
			{
				case LOGSEVERITY_ASSERT:
				{
					return "ASSERT";
				}
				case LOGSEVERITY_ERROR:
				{
					return "ERROR";
				}
				case LOGSEVERITY_WARNING:
				{
					return "WARNING";
				}
				case LOGSEVERITY_INFO:
				{
					return "INFO";
				}
				case LOGSEVERITY_SUCCESS:
				{
					return "SUCCESS";
				}
				case LOGSEVERITY_INFO_SUCCESS:
				{
					return "INFO SUCCESS";
				}
				case LOGSEVERITY_AWESOME:
				{
					return "AWESOME";
				}
			}
		}

		namespace logger
		{
#define ASSERT_LEVEL LogSeverity::LOGSEVERITY_ERROR

			/// <summary>
			/// Represents the logger message with variables for location, category and severity.
			/// </summary>
			class LoggerMessage
			{
			public:
				LoggerMessage(const std::string& a_RawMessage, const std::string& a_Category, const std::string& a_Location, LogSeverity a_Severity, const std::chrono::system_clock::time_point& a_Time);

				/// <summary>
				/// Retrieves the raw message.
				/// </summary>
				/// <returns>A string containing the message without any category, location and severity information.</returns>
				const std::string& GetRawMessage() const;

				/// <summary>
				/// Retrieves the logging category.
				/// </summary>
				/// <returns>A string containing the logging category.</returns>
				const std::string& GetCategory() const;

				/// <summary>
				/// Retrieves the location the logging took place in.
				/// </summary>
				/// <returns>A string containing the location the logging took place in.</returns>
				const std::string& GetLocation() const;

				/// <summary>
				/// Retrieves the severity.
				/// </summary>
				/// <returns>The log severity enumeration.</returns>
				LogSeverity GetSeverity() const;

				/// <summary>
				/// Retrieves the time the message was logged.
				/// </summary>
				/// <returns>The timepoint containing information about when the message was logged.</returns>
				const std::chrono::system_clock::time_point& GetTime() const;
			private:
				std::string m_RawMessage; /// The message without any category, location and severity information.
				std::string m_Category; /// The logging category.
				std::string m_Location; /// The file the logging took place in.
				LogSeverity m_Severity; /// The severity of the log message.
				std::chrono::system_clock::time_point m_Time; /// The time the logging was done.
			};

			/// <summary>
			/// Represents the logger system that outputs log messages to the console and a log file.
			/// </summary>
			class Logger : public ThreadedSystem
			{
			public:
				/// <summary>
				/// Loop method for the thread.
				/// </summary>
				void Loop() override;

				/// <summary>
				/// Signals the thread to stop.
				/// </summary>
				bool Destroy() override;

				/// <summary>
				/// Logs a message with category, file and line info to the console and a file.
				/// </summary>
				/// <param name="a_Severity">The severity of the log message.</param>
				/// <param name="a_Category">The category (class) that is wants to log the message.</param>
				/// <param name="a_Message">The message that will be logged.</param>
				/// <param name="a_File">The file in which the logging happened.</param>
				/// <param name="a_Line">The line at which the logging happened.</param>
				void Log(LogSeverity a_Severity, const char* a_Category, const char* a_Message, const char* a_File, int a_Line);

				/// <summary>
				/// Logs a formatted message with category, file and line info to the console and a file.
				/// </summary>
				/// <param name="a_Severity">The severity of the log message.</param>
				/// <param name="a_Category">The category (class) that is wants to log the message.</param>
				/// <param name="a_Message">The message that will be logged.</param>
				/// <param name="a_File">The file in which the logging happened.</param>
				/// <param name="a_Line">The line at which the logging happened.</param>
				void LogF(LogSeverity a_Severity, const char* a_Category, const char* a_Message, const char* a_File, int a_Line, ...);
				void PrintMessage(LogSeverity a_Severity, const char* a_Category, const char* a_Message, const char* a_File, int a_Line);

				//Event<const Message&> OnMessageLogged;
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

				std::queue<LoggerMessage> m_Messages; /// Queue of messages that will be logged.
				std::mutex m_MessagesMutex; /// The mutex used for synchronization between the threads for stopping or initializing.
			};
			inline extern Logger LOGGER = {};
		}
	}
}

// Messages should be like this: "STATUS ACTION", so "Created x" or "Failed creating x"
#define LOGF(a_Severity, a_Category, a_Message, ...)\
do{\
	engine::logger::LOGGER.LogF(a_Severity, a_Category, a_Message, __FILE__, __LINE__, __VA_ARGS__);\
	if (a_Severity <= ASSERT_LEVEL)\
		assert(false);\
} while (0)

// Messages should be like this: "STATUS ACTION", so "Created x" or "Failed creating x"
#define LOG(a_Severity, a_Category, a_Message)\
do{\
	engine::logger::LOGGER.Log(a_Severity, a_Category, a_Message, __FILE__, __LINE__);\
	if (a_Severity <= ASSERT_LEVEL)\
		assert(false);\
} while (0)

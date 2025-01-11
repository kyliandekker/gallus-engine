#include "core/logger/Logger.h"

#include <cstdarg>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <format>
#include <windows.h>
#include <iostream>

#define CATEGORY_LOGGER "LOGGER"

namespace coopscoop
{
	namespace core
	{
		namespace logger
		{
			/*
				* Logger Message
			*/

#pragma region LOGGER_MESSAGE

			LoggerMessage::LoggerMessage(const std::string& a_RawMessage, const std::string& a_Category, const std::string& a_Location, LogSeverity a_Severity, const std::chrono::system_clock::time_point& a_Time) :
				m_RawMessage(a_RawMessage),
				m_Category(a_Category),
				m_Location(a_Location),
				m_Severity(a_Severity),
				m_Time(a_Time)
			{ }

			const std::string& LoggerMessage::GetRawMessage() const
			{
				return m_RawMessage;
			}

			const std::string& LoggerMessage::GetLocation() const
			{
				return m_Location;
			}

			const std::string& LoggerMessage::GetCategory() const
			{
				return m_Category;
			}

			LogSeverity LoggerMessage::GetSeverity() const
			{
				return m_Severity;
			}

			const std::chrono::system_clock::time_point& LoggerMessage::GetTime() const
			{
				return m_Time;
			}

#pragma endregion LOGGER_MESSAGE

			/*
				* Logger
			*/

#pragma region LOGGER

			FILE* console = nullptr;
			FILE* logFile = nullptr;
			bool Logger::InitializeThread()
			{
				// Terminal/Console initialization for debug builds.
#ifdef _DEBUG
				AllocConsole();
				freopen_s(&console, "CONOUT$", "w", stdout);

				// Enable virtual terminal processing (for ANSI escape codes, if needed)
				HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
				DWORD dwMode = 0;
				GetConsoleMode(hOut, &dwMode);
				dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
				SetConsoleMode(hOut, dwMode);

				// Set console screen buffer size to avoid wrapping issues
				COORD coord = { 80, 300 };  // Width: 80, Height: 500
				SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coord);

				// Optionally adjust console window size
				HWND consoleWindow = GetConsoleWindow();
				MoveWindow(consoleWindow, 100, 100, 800, 600, TRUE);
#endif // _DEBUG

				time_t time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
				struct tm buf;

				localtime_s(&buf, &time_t);

				std::string logFilename(50, '\0');
				std::strftime(&logFilename[0], logFilename.size(), "./log-%Y-%m-%d %H-%M-%S.log", &buf);  // Changed colon to dash

				fopen_s(&logFile, logFilename.c_str(), "wb");
				if (!logFile)
				{
					LOG(LOGSEVERITY_SUCCESS, CATEGORY_LOGGER, "Failed initializing logger: Could not create log file..");
					return false;
				}

				LOG(LOGSEVERITY_SUCCESS, CATEGORY_LOGGER, "Initialized logger.");

				return ThreadedSystem::InitializeThread();
			}

			void Logger::Finalize()
			{
#ifdef _DEBUG
				if (console)
				{
					fclose(console);
					console = nullptr;
				}
#endif
				if (logFile)
				{
					fclose(logFile);
					logFile = nullptr;
				}

				ThreadedSystem::Finalize();
			}

			// Colors for console.
			constexpr auto COLOR_YELLOW = "\033[0;33m";
			constexpr auto COLOR_WHITE = "\033[0;37m";
			constexpr auto COLOR_GREEN = "\033[0;32m";
			constexpr auto COLOR_RED = "\033[31m";
			constexpr auto COLOR_CYAN = "\033[0;36m";
			constexpr auto COLOR_PURPLE = "\033[0;35m";
			constexpr auto COLOR_BLUE = "\033[0;34m";
			constexpr auto COLOR_PINK = "\033[1;35m";

			// Colors for the different severity levels.
			std::string LOGGER_SEVERITY_COLOR[8] =
			{
				COLOR_RED, // ASSERT
				COLOR_RED, // ERROR
				COLOR_YELLOW, // WARNING
				COLOR_CYAN, // INFO
				COLOR_CYAN, // TEST
				COLOR_GREEN, // SUCCESS
				COLOR_BLUE, // INFO SUCCESS
				COLOR_PINK, // AWESOME
			};

			Logger::~Logger()
			{
				if (logFile)
				{
					fclose(logFile);
					logFile = nullptr;
				}
			}

			void Logger::Loop()
			{
				// This is a loop because this makes it so that it will display all messages before destruction.
				while (!m_Messages.empty())
				{
					const LoggerMessage lm = m_Messages.front();
					m_Messages.pop();

					// Format the message.
					std::string message =
						"[" + LOGGER_SEVERITY_COLOR[lm.GetSeverity()] +
						LogSeverityToString(lm.GetSeverity()) +
						COLOR_WHITE + "] " + lm.GetRawMessage() + " " +
						lm.GetLocation() + "\n";

					// Print the message to the console.
					printf(message.c_str());
					fflush(stdout);

					// Format the message.
					message =
						"[" + LogSeverityToString(lm.GetSeverity()) +
						"] " + lm.GetRawMessage() + " " +
						lm.GetLocation() +"\n";

					fprintf(logFile, message.c_str());

					//OnMessageLogged(lm);
				}
			}

			bool Logger::Destroy()
			{
				LOG(LOGSEVERITY_INFO, CATEGORY_LOGGER, "Destroying logger.");
				return ThreadedSystem::Destroy();
			}

			void Logger::Log(LogSeverity a_Severity, const char* a_Category, const char* a_Message, const char* a_File, int a_Line)
			{
				PrintMessage(a_Severity, a_Category, a_Message, a_File, a_Line);
			}

			void Logger::LogF(LogSeverity a_Severity, const char* a_Category, const char* a_Message, const char* a_File, int a_Line, ...)
			{
				va_list va_format_list;
				va_start(va_format_list, a_Line);

				const size_t buffersize = vsnprintf(NULL, 0, a_Message, va_format_list) + 1;
				char* formatted_message = (char*)malloc(buffersize);
				vsnprintf(formatted_message, buffersize, a_Message, va_format_list);

				PrintMessage(a_Severity, a_Category, formatted_message, a_File, a_Line);

				free(formatted_message);
			}

			void Logger::PrintMessage(LogSeverity a_Severity, const char* a_Category, const char* a_Message, const char* a_File, int a_Line)
			{
				std::string message = std::format("{0} on line {1}",
					a_File,
					a_Line);

				std::scoped_lock lock(m_MessagesMutex);
				m_Messages.push(LoggerMessage(a_Message, a_Category, message, a_Severity, std::chrono::system_clock::now()));
			}

#pragma endregion LOGGER
		}
	}
}
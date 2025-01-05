#include "engine/logger/Logger.h"

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
	namespace engine
	{
		namespace logger
		{
			constexpr auto COLOR_YELLOW = "\033[0;33m";
			constexpr auto COLOR_WHITE = "\033[0;37m";
			constexpr auto COLOR_GREEN = "\033[0;32m";
			constexpr auto COLOR_RED = "\033[31m";
			constexpr auto COLOR_CYAN = "\033[0;36m";
			constexpr auto COLOR_PURPLE = "\033[0;35m";
			constexpr auto COLOR_BLUE = "\033[0;34m";
			constexpr auto COLOR_PINK = "\033[1;35m";

			std::string LOGGER_SEVERITY_COLOR[7] =
			{
				COLOR_RED,
				COLOR_RED,
				COLOR_YELLOW,
				COLOR_CYAN,
				COLOR_GREEN,
				COLOR_BLUE,
				COLOR_PINK,
			};

			std::string LOGGER_SEVERITY_TEXT[7] =
			{
				"ASSERT",
				"ERROR",
				"WARNING",
				"INFO",
				"SUCCESS",
				"INFO SUCCESS",
				"AWESOME",
			};

			Message::Message(const std::string& a_RawMessage, const std::string& a_Category, const std::string& a_Location, LogSeverity a_Severity, const std::chrono::system_clock::time_point& a_Time) :
				m_RawMessage(a_RawMessage),
				m_Category(a_Category),
				m_Location(a_Location),
				m_Severity(a_Severity),
				m_Time(a_Time)
			{ }

			const std::string& Message::GetRawMessage() const
			{
				return m_RawMessage;
			}

			const std::string& Message::GetLocation() const
			{
				return m_Location;
			}

			const std::string& Message::GetCategory() const
			{
				return m_Category;
			}

			LogSeverity Message::GetSeverity() const
			{
				return m_Severity;
			}

			const std::chrono::system_clock::time_point& Message::GetTime() const
			{
				return m_Time;
			}

			bool Logger::Initialize()
			{
				LOG(LOGSEVERITY_INFO, CATEGORY_LOGGER, "Initializing logger.");
				return ThreadedSystem::Initialize();
			}

			FILE* console = nullptr;
			bool Logger::InitializeThread()
			{
#ifdef _DEBUG
				AllocConsole();
				freopen_s(&console, "CONOUT$", "w", stdout);

				HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
				DWORD dwMode = 0;
				GetConsoleMode(hOut, &dwMode);
				dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
				SetConsoleMode(hOut, dwMode);
#endif // _DEBUG

				LOG(LOGSEVERITY_SUCCESS, CATEGORY_LOGGER, "Initialized logger.");

				// Start the thread to handle logging
				ThreadedSystem::InitializeThread();

				// Wait for messages until m_Ready is false and the queue is empty
				while (m_Ready.load() || !m_Messages.empty())
				{
					std::unique_lock lock(m_MessagesMutex);
					m_MessageCondVar.wait(lock, [this]() { return !m_Messages.empty() || !m_Ready.load(); });

					// Process available messages
					while (!m_Messages.empty())
					{
						const Message lm = m_Messages.front();
						m_Messages.pop();
						Print(lm);
					}
				}

				// Log that the thread is shutting down
				LOG(LOGSEVERITY_SUCCESS, CATEGORY_LOGGER, "Logger thread shutting down.");
				return true;
			}

			bool Logger::Destroy()
			{
				m_Ready.store(false); // Use atomic store

				// Notify the logger thread to wake up and process any remaining messages
				m_MessageCondVar.notify_all();

				// Wait until all messages are processed
				{
					std::unique_lock lock(m_MessagesMutex);
					m_MessageCondVar.wait(lock, [this]() { return m_Messages.empty(); });
				}
#ifdef _DEBUG
				if (console)
				{
					fclose(console);
				}
#endif
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
				struct tm lt;
				time_t t;
				t = time(NULL);
				localtime_s(&lt, &t);

				std::string message = std::format("{0} on line {1}",
					a_File,
					a_Line);

				std::scoped_lock lock(m_MessagesMutex);
				m_Messages.push(Message(a_Message, a_Category, message, a_Severity, std::chrono::system_clock::now()));
			}

			void Logger::Print(const Message& a_Message)
			{
				std::string message =
					"[" + LOGGER_SEVERITY_COLOR[a_Message.GetSeverity()] +
					LOGGER_SEVERITY_TEXT[a_Message.GetSeverity()] +
					COLOR_WHITE + "] " + a_Message.GetRawMessage() + " " +
					a_Message.GetLocation();

				std::cout << message.c_str() << std::endl;
				fflush(stdout);
			}
		}
	}
}
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

		namespace logger
		{
#define ASSERT_LEVEL LogSeverity::LOGSEVERITY_ERROR

			class Message
			{
			public:
				Message(const std::string& a_RawMessage, const std::string& a_Category, const std::string& a_Location, LogSeverity a_Severity, const std::chrono::system_clock::time_point& a_Time);

				const std::string& GetRawMessage() const;
				const std::string& GetLocation() const;
				const std::string& GetCategory() const;
				LogSeverity GetSeverity() const;
				const std::chrono::system_clock::time_point& GetTime() const;
			private:
				std::string m_RawMessage;
				std::string m_Category;
				std::string m_Location;
				LogSeverity m_Severity;
				std::chrono::system_clock::time_point m_Time;
			};

			class Logger : public ThreadedSystem
			{
			public:
				bool InitializeThread() override;
				void Loop() override;
				void Stop() override;

				void Log(LogSeverity a_Severity, const char* a_Category, const char* a_Message, const char* a_File, int a_Line);
				void LogF(LogSeverity a_Severity, const char* a_Category, const char* a_Message, const char* a_File, int a_Line, ...);
				void PrintMessage(LogSeverity a_Severity, const char* a_Category, const char* a_Message, const char* a_File, int a_Line);

				void Print(const Message& a_Message);

				//Event<const Message&> OnMessageLogged;
			private:
				void Destroy() override;

				std::queue<Message> m_Messages;
				std::mutex m_MessagesMutex;
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

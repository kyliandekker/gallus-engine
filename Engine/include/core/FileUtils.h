#pragma once

#include <string>

namespace coopscoop
{
	namespace core
	{
		class DataStream;
	}
	namespace file
	{
		class FileLoader
		{
		public:
			static const std::string GetAppDataPath();
			static bool LoadFile(const std::string& a_Path, core::DataStream& a_Data);
		};
	}
}
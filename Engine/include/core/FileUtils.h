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
			static bool LoadFile(const std::string& a_Path, core::DataStream& a_Data);
		};
	}
}
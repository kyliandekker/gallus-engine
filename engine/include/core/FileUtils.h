#pragma once

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

namespace gallus
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
			static const fs::path GetAppDataPath();
			static bool LoadFile(const fs::path& a_Path, core::DataStream& a_Data);
			static bool SaveFile(const fs::path& a_Path, const core::DataStream& a_Data);
			static bool CreateFolder(const fs::path& a_Path);
			static bool OpenInExplorer(const fs::path& a_Path);
			static fs::path GetPath(const std::string& a_Path);
		};
	}
}
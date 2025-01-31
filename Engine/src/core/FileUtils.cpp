#include "core/FileUtils.h"

#include <ShlObj_core.h>
#include <filesystem>

#include "core/DataStream.h"

namespace fs = std::filesystem;

namespace coopscoop
{
    namespace file
    {
		const std::string FileLoader::GetAppDataPath()
		{
			PWSTR path_tmp;
			fs::path path;
			SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &path_tmp);
			path = path_tmp;

			std::string appDataPath = std::string(path.generic_string());
			fs::create_directories(appDataPath);

			return appDataPath;
		}

		bool FileLoader::LoadFile(const std::string& a_Path, core::DataStream& a_Data)
        {
			FILE* file = nullptr;
			fopen_s(&file, a_Path.c_str(), "rb");
			if (!file)
			{
				return false;
			}

			fseek(file, 0, SEEK_END);
			size_t fileSize = ftell(file);
			rewind(file);

			a_Data = core::DataStream(fileSize);
			fread(a_Data.data(), fileSize, 1, file);

			fclose(file);

			return true;
        }
    }
}
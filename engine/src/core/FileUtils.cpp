#include "core/FileUtils.h"

#include <ShlObj_core.h>
#include <filesystem>

#include "core/DataStream.h"
#include "core/logger/Logger.h"

namespace fs = std::filesystem;

namespace gallus
{
    namespace file
    {
		const fs::path FileLoader::GetAppDataPath()
		{
			PWSTR path_tmp;
			fs::path path;
			SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &path_tmp);
			path = path_tmp;

			fs::create_directories(path);

			return path;
		}

		bool FileLoader::LoadFile(const fs::path& a_Path, core::DataStream& a_Data)
        {
			if (!fs::exists(a_Path))
			{
				return false;
			}

			FILE* file = nullptr;
			fopen_s(&file, a_Path.generic_string().c_str(), "rb");
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

		bool FileLoader::SaveFile(const fs::path& a_Path, const core::DataStream& a_Data)
		{
			FILE* file = nullptr;
			fopen_s(&file, a_Path.generic_string().c_str(), "wb");
			if (!file)
			{
				return false;
			}

			fwrite(a_Data.data(), a_Data.size(), 1, file);

			fclose(file);

			return true;
		}

		bool FileLoader::CreateFolder(const fs::path& a_Path)
		{
			return fs::create_directories(a_Path);
		}

		fs::path FileLoader::GetPath(const std::string& a_Path)
		{
			return fs::path(a_Path);
		}
    }
}
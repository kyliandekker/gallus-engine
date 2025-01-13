#include "core/FileUtils.h"

#include "core/DataStream.h"

namespace coopscoop
{
    namespace file
    {
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
#ifdef __EDITOR__

#include "editor/imgui/ImGuiDefines.h"

#include <cstdarg>
#include <format>

namespace renegade
{
	namespace editor
	{
		namespace imgui
		{
			std::string IMGUI_FORMAT_ID(const std::string& a_Text, const char* a_ID, const std::string& a_IDName)
			{
				std::string full_id = a_Text + a_ID + a_IDName;
				return full_id;
			}
		}
	}
}

#endif // __EDITOR__
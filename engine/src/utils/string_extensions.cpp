#include "utils/string_extensions.h"

#include <algorithm>

namespace gallus
{
	namespace string_extensions
	{
		std::string StringToUpper(std::string a_StrToConvert)
		{
			std::transform(a_StrToConvert.begin(), a_StrToConvert.end(), a_StrToConvert.begin(), ::toupper);

			return a_StrToConvert;
		}

		std::string StringToLower(std::string a_StrToConvert)
		{
			std::transform(a_StrToConvert.begin(), a_StrToConvert.end(), a_StrToConvert.begin(), ::tolower);

			return a_StrToConvert;
		}
	}
}
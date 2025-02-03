#pragma once

#include <rapidjson/document.h>
#include <string>

namespace rapidjson
{
	inline bool GetString(const rapidjson::Value& a_Document, const std::string& a_Key, std::string& a_Value)
	{
		if (!a_Document.HasMember(a_Key.c_str()))
		{
			return false;
		}
		if (!a_Document[a_Key.c_str()].IsString())
		{
			return false;
		}
		a_Value = a_Document[a_Key.c_str()].GetString();
		return true;
	}

	inline bool GetFloat(const rapidjson::Value& a_Document, const std::string& a_Key, float& a_Value)
	{
		if (!a_Document.HasMember(a_Key.c_str()))
		{
			return false;
		}
		if (!a_Document[a_Key.c_str()].IsFloat())
		{
			return false;
		}
		a_Value = a_Document[a_Key.c_str()].GetFloat();
		return true;
	}

	inline bool GetBool(const rapidjson::Value& a_Document, const std::string& a_Key, bool& a_Value)
	{
		if (!a_Document.HasMember(a_Key.c_str()))
		{
			return false;
		}
		if (!a_Document[a_Key.c_str()].IsBool())
		{
			return false;
		}
		a_Value = a_Document[a_Key.c_str()].GetBool();
		return true;
	}

	inline bool GetLong(const rapidjson::Value& a_Document, const std::string& a_Key, long long& a_Value)
	{
		if (!a_Document.HasMember(a_Key.c_str()))
		{
			return false;
		}
		if (!a_Document[a_Key.c_str()].IsInt64())
		{
			return false;
		}
		a_Value = a_Document[a_Key.c_str()].GetInt64();
		return true;
	}

	inline bool GetInt(const rapidjson::Value& a_Document, const std::string& a_Key, int& a_Value)
    {
		if (!a_Document.HasMember(a_Key.c_str()))
		{
			return false;
		}
		if (!a_Document[a_Key.c_str()].IsInt())
		{
			return false;
		}
		a_Value = a_Document[a_Key.c_str()].GetInt();
		return true;
    }
}
#pragma once

#include <rapidjson/document.h>
#include <string>

namespace gallus
{
	namespace gameplay
	{
		class Component
		{
		public:
			virtual ~Component() = default;

			virtual void Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const = 0;
			virtual void Deserialize(const rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) = 0;
		};
	}
}
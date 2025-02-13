#include "gameplay/systems/EntityInfoSystem.h"

#define JSON_ENTITY_DETAIL_COMPONENT_VAR "details"

namespace gallus
{
	namespace gameplay
	{
		std::string EntityInfoSystem::GetPropertyName() const
		{
			return JSON_ENTITY_DETAIL_COMPONENT_VAR;
		}
	}
}
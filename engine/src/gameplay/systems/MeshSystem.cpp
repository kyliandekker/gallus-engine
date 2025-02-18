#include "gameplay/systems/MeshSystem.h"

#define JSON_ENTITY_MESH_COMPONENT_VAR "meshInfo"

namespace gallus
{
	namespace gameplay
	{
		std::map<EntityID, MeshComponent>& MeshSystem::GetComponents()
		{
			return m_Components;
		}

		std::string MeshSystem::GetPropertyName() const
		{
			return JSON_ENTITY_MESH_COMPONENT_VAR;
		}
	}
}
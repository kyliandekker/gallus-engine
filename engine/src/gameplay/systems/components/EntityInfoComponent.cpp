#include "gameplay/systems/components/EntityInfoComponent.h"

#include <rapidjson/utils.h>

#define JSON_ENTITY_DETAIL_COMPONENT_ACTIVE_VAR "active"
#define JSON_ENTITY_DETAIL_COMPONENT_NAME_VAR "name"

namespace gallus
{
	namespace gameplay
	{
		void EntityInfoComponent::SetName(const std::string& a_Name)
		{
			m_Name = a_Name;
		}

		void EntityInfoComponent::SetIsActive(bool a_Active)
		{
			m_IsActive = a_Active;
		}

		void EntityInfoComponent::Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const
		{
			if (!a_Document.IsObject())
			{
				return;
			}

			a_Document.AddMember(JSON_ENTITY_DETAIL_COMPONENT_ACTIVE_VAR, m_IsActive, a_Allocator);
			a_Document.AddMember(JSON_ENTITY_DETAIL_COMPONENT_NAME_VAR, rapidjson::Value().SetString(m_Name.c_str(), m_Name.size()), a_Allocator);
		}

		void EntityInfoComponent::Deserialize(const rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator)
		{
			if (!a_Document.IsObject())
			{
				return;
			}

			if (a_Document.HasMember(JSON_ENTITY_DETAIL_COMPONENT_ACTIVE_VAR) && a_Document[JSON_ENTITY_DETAIL_COMPONENT_ACTIVE_VAR].IsBool())
			{
				rapidjson::GetBool(a_Document, JSON_ENTITY_DETAIL_COMPONENT_ACTIVE_VAR, m_IsActive);
			}
			if (a_Document.HasMember(JSON_ENTITY_DETAIL_COMPONENT_NAME_VAR) && a_Document[JSON_ENTITY_DETAIL_COMPONENT_NAME_VAR].IsString())
			{
				m_Name = a_Document[JSON_ENTITY_DETAIL_COMPONENT_NAME_VAR].GetString();
			}
		}
	}
}
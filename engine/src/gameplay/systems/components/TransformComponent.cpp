#include "gameplay/systems/components/TransformComponent.h"

#include <rapidjson/utils.h>

#define JSON_ENTITY_TRANSFORM_COMPONENT_POSITION_VAR "position"
#define JSON_ENTITY_TRANSFORM_COMPONENT_ROTATION_VAR "rotation"
#define JSON_ENTITY_TRANSFORM_COMPONENT_SCALE_VAR "scale"
#define JSON_ENTITY_TRANSFORM_COMPONENT_X_VAR "x"
#define JSON_ENTITY_TRANSFORM_COMPONENT_Y_VAR "y"
#define JSON_ENTITY_TRANSFORM_COMPONENT_Z_VAR "z"

namespace gallus
{
	namespace gameplay
	{
		graphics::dx12::Transform& TransformComponent::Transform()
		{
			return m_Transform;
		}

		void TransformComponent::Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const
		{
			if (!a_Document.IsObject())
			{
				return;
			}

			a_Document.AddMember(JSON_ENTITY_TRANSFORM_COMPONENT_POSITION_VAR, rapidjson::Value().SetObject(), a_Allocator);

			a_Document[JSON_ENTITY_TRANSFORM_COMPONENT_POSITION_VAR].AddMember(JSON_ENTITY_TRANSFORM_COMPONENT_X_VAR, m_Transform.GetPosition().x, a_Allocator);
			a_Document[JSON_ENTITY_TRANSFORM_COMPONENT_POSITION_VAR].AddMember(JSON_ENTITY_TRANSFORM_COMPONENT_Y_VAR, m_Transform.GetPosition().y, a_Allocator);
			a_Document[JSON_ENTITY_TRANSFORM_COMPONENT_POSITION_VAR].AddMember(JSON_ENTITY_TRANSFORM_COMPONENT_Z_VAR, m_Transform.GetPosition().z, a_Allocator);

			a_Document.AddMember(JSON_ENTITY_TRANSFORM_COMPONENT_ROTATION_VAR, rapidjson::Value().SetObject(), a_Allocator);

			a_Document[JSON_ENTITY_TRANSFORM_COMPONENT_ROTATION_VAR].AddMember(JSON_ENTITY_TRANSFORM_COMPONENT_X_VAR, m_Transform.GetRotation().x, a_Allocator);
			a_Document[JSON_ENTITY_TRANSFORM_COMPONENT_ROTATION_VAR].AddMember(JSON_ENTITY_TRANSFORM_COMPONENT_Y_VAR, m_Transform.GetRotation().y, a_Allocator);
			a_Document[JSON_ENTITY_TRANSFORM_COMPONENT_ROTATION_VAR].AddMember(JSON_ENTITY_TRANSFORM_COMPONENT_Z_VAR, m_Transform.GetRotation().z, a_Allocator);

			a_Document.AddMember(JSON_ENTITY_TRANSFORM_COMPONENT_SCALE_VAR, rapidjson::Value().SetObject(), a_Allocator);

			a_Document[JSON_ENTITY_TRANSFORM_COMPONENT_SCALE_VAR].AddMember(JSON_ENTITY_TRANSFORM_COMPONENT_X_VAR, m_Transform.GetScale().x, a_Allocator);
			a_Document[JSON_ENTITY_TRANSFORM_COMPONENT_SCALE_VAR].AddMember(JSON_ENTITY_TRANSFORM_COMPONENT_Y_VAR, m_Transform.GetScale().y, a_Allocator);
			a_Document[JSON_ENTITY_TRANSFORM_COMPONENT_SCALE_VAR].AddMember(JSON_ENTITY_TRANSFORM_COMPONENT_Z_VAR, m_Transform.GetScale().z, a_Allocator);
		}

		void TransformComponent::Deserialize(const rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator)
		{
			if (!a_Document.IsObject())
			{
				return;
			}

			if (a_Document.HasMember(JSON_ENTITY_TRANSFORM_COMPONENT_POSITION_VAR) && a_Document[JSON_ENTITY_TRANSFORM_COMPONENT_POSITION_VAR].IsObject())
			{
				rapidjson::GetFloat(a_Document[JSON_ENTITY_TRANSFORM_COMPONENT_POSITION_VAR], JSON_ENTITY_TRANSFORM_COMPONENT_X_VAR, m_Transform.GetPosition().x);
				rapidjson::GetFloat(a_Document[JSON_ENTITY_TRANSFORM_COMPONENT_POSITION_VAR], JSON_ENTITY_TRANSFORM_COMPONENT_Y_VAR, m_Transform.GetPosition().y);
				rapidjson::GetFloat(a_Document[JSON_ENTITY_TRANSFORM_COMPONENT_POSITION_VAR], JSON_ENTITY_TRANSFORM_COMPONENT_Z_VAR, m_Transform.GetPosition().z);
			}

			if (a_Document.HasMember(JSON_ENTITY_TRANSFORM_COMPONENT_ROTATION_VAR) && a_Document[JSON_ENTITY_TRANSFORM_COMPONENT_ROTATION_VAR].IsObject())
			{
				rapidjson::GetFloat(a_Document[JSON_ENTITY_TRANSFORM_COMPONENT_ROTATION_VAR], JSON_ENTITY_TRANSFORM_COMPONENT_X_VAR, m_Transform.GetRotation().x);
				rapidjson::GetFloat(a_Document[JSON_ENTITY_TRANSFORM_COMPONENT_ROTATION_VAR], JSON_ENTITY_TRANSFORM_COMPONENT_Y_VAR, m_Transform.GetRotation().y);
				rapidjson::GetFloat(a_Document[JSON_ENTITY_TRANSFORM_COMPONENT_ROTATION_VAR], JSON_ENTITY_TRANSFORM_COMPONENT_Z_VAR, m_Transform.GetRotation().z);
			}

			if (a_Document.HasMember(JSON_ENTITY_TRANSFORM_COMPONENT_SCALE_VAR) && a_Document[JSON_ENTITY_TRANSFORM_COMPONENT_SCALE_VAR].IsObject())
			{
				rapidjson::GetFloat(a_Document[JSON_ENTITY_TRANSFORM_COMPONENT_SCALE_VAR], JSON_ENTITY_TRANSFORM_COMPONENT_X_VAR, m_Transform.GetScale().x);
				rapidjson::GetFloat(a_Document[JSON_ENTITY_TRANSFORM_COMPONENT_SCALE_VAR], JSON_ENTITY_TRANSFORM_COMPONENT_Y_VAR, m_Transform.GetScale().y);
				rapidjson::GetFloat(a_Document[JSON_ENTITY_TRANSFORM_COMPONENT_SCALE_VAR], JSON_ENTITY_TRANSFORM_COMPONENT_Z_VAR, m_Transform.GetScale().z);
			}
		}
	}
}
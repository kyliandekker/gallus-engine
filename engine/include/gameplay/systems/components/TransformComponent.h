#pragma once

#include "gameplay/systems/components/Component.h"

#include "graphics/dx12/Transform.h"

namespace gallus
{
	namespace gameplay
	{
		class TransformComponent : public Component
		{
		public:
			graphics::dx12::Transform& Transform();

			void Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const override;
			void Deserialize(const rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) override;
		private:
			graphics::dx12::Transform m_Transform;
		};
	}
}
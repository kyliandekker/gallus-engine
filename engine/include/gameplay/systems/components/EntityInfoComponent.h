#pragma once

#include "gameplay/systems/components/Component.h"

namespace gallus
{
	namespace gameplay
	{
		struct EntityID;

		class EntityInfoComponent : public Component
		{
		public:
			const std::string& GetName() const
			{
				return m_Name;
			}
			void SetName(const std::string& a_Name);

			bool IsActive() const
			{
				return m_IsActive;
			}
			void SetIsActive(bool a_Active);

			void Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const override;
			void Deserialize(const rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) override;
		private:
			bool m_IsActive = true;
			std::string m_Name;
		};
	}
}
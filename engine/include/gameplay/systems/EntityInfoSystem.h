#pragma once

#include "gameplay/ECSBaseSystem.h"
#include "gameplay/systems/components/EntityInfoComponent.h"

namespace gallus
{
	namespace gameplay
	{
		class EntityInfoSystem : public ECSBaseSystem<EntityInfoComponent>
		{
		public:
			std::string GetPropertyName() const override;
		};
	}
}
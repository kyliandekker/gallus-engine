#pragma once

#include "gameplay/ECSBaseSystem.h"
#include "gameplay/systems/components/TransformComponent.h"

namespace gallus
{
	namespace gameplay
	{
		class TransformSystem : public ECSBaseSystem<TransformComponent>
		{
		public:
			std::string GetPropertyName() const override;
		};
	}
}
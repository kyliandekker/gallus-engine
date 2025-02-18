#pragma once

#include "gameplay/ECSBaseSystem.h"
#include "gameplay/systems/components/MeshComponent.h"

namespace gallus
{
	namespace gameplay
	{
		class MeshSystem : public ECSBaseSystem<MeshComponent>
		{
		public:
			std::map<EntityID, MeshComponent>& GetComponents();

			std::string GetPropertyName() const override;
		};
	}
}
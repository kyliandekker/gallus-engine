#include "gameplay/EntityComponentSystem.h"

#include "core/logger/Logger.h"

#include "gameplay/ECSBaseSystem.h"

#include "gameplay/systems/EntityInfoSystem.h"
#include "gameplay/systems/TransformSystem.h"
#include "gameplay/systems/MeshSystem.h"

namespace gallus
{
	namespace gameplay
	{
		bool EntityComponentSystem::Initialize()
		{
			CreateSystem<EntityInfoSystem>();
			CreateSystem<TransformSystem>();
			CreateSystem<MeshSystem>();

			LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_ECS, "ECS initialized.");
			return System::Initialize();
		}

		bool EntityComponentSystem::Destroy()
		{
			for (AbstractECSSystem* system : m_Systems)
			{
				delete system;
			}
			LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_ECS, "ECS destroyed.");
			return System::Destroy();
		}

		void EntityComponentSystem::Update(const float& a_DeltaTime)
		{
			std::lock_guard<std::mutex> lock(m_EntityMutex);

			bool changed = (!m_EntitiesToDelete.empty()) || m_Clear || (!m_EntitiesToAdd.empty());

			if (!m_EntitiesToDelete.empty())
			{
				for (EntityID entity : m_EntitiesToDelete)
				{
					DeleteEntity(entity);
				}
				m_EntitiesToDelete.clear();
			}

			if (m_Clear)
			{
				ClearEntities();
				m_Clear = false;
			}

			if (changed)
			{
				m_OnEntitiesUpdated();
			}

			for (auto& sys : m_Systems)
			{
				sys->UpdateComponents(a_DeltaTime);
			}

			if (!m_Started)
			{
				return;
			}

			if (m_Paused)
			{
				return;
			}

			for (auto& sys : m_Systems)
			{
				sys->Update(a_DeltaTime);
			}
		}

		bool EntityComponentSystem::IsPaused() const
		{
			return m_Paused;
		}

		void EntityComponentSystem::SetPaused(bool a_Paused)
		{
			m_Paused = a_Paused;
		}

		bool EntityComponentSystem::HasStarted() const
		{
			return m_Started;
		}

		void EntityComponentSystem::SetStarted(bool a_Started)
		{
			m_Started = a_Started;
		}

		EntityID EntityComponentSystem::CreateEntity(const std::string& a_Name)
		{
			EntityID id(++m_NextID);
			m_Entities.push_back(id);

			GetSystem<EntityInfoSystem>().CreateComponent(id).SetName(a_Name);

			return m_Entities[m_Entities.size() - 1];
		}

		void EntityComponentSystem::Delete(const EntityID& a_ID)
		{
			m_EntitiesToDelete.push_back(a_ID);
		}

		void EntityComponentSystem::DeleteEntity(const EntityID& a_ID)
		{
			for (auto& sys : m_Systems)
			{
				sys->DeleteComponent(a_ID);
			}
			m_Entities.erase(
				std::remove_if(m_Entities.begin(), m_Entities.end(),
				[&a_ID](const EntityID& entity)
				{
					return entity == a_ID;
				}),
				m_Entities.end());
		};

		bool EntityComponentSystem::IsEntityValid(const EntityID& a_ID) const
		{
			return a_ID.IsValid();
		}

		void EntityComponentSystem::Clear()
		{
			m_Clear = true;
		}

		void EntityComponentSystem::ClearEntities()
		{
			m_Entities.clear();
		}

		std::string EntityComponentSystem::GetUniqueName(const std::string& a_Name)
		{
			std::string name = a_Name;

			bool found = true;
			int i = 0;
			while (found)
			{
				found = false;
				for (EntityID& entity : m_Entities)
				{
					if (GetSystem<EntityInfoSystem>().GetComponent(entity).GetName() == name)
					{
						i++;
						if (i != 0)
						{
							name = a_Name + " (" + std::to_string(i) + ")";
							found = true;
						}
					}
				}
			}
			return name;
		}

		std::vector<EntityID>& EntityComponentSystem::GetEntities()
		{
			return m_Entities;
		}

		std::vector<AbstractECSSystem*> EntityComponentSystem::GetSystemsContainingEntity(const EntityID& a_ID)
		{
			std::vector<AbstractECSSystem*> systems;
			for (AbstractECSSystem* system : m_Systems)
			{
				if (system->ContainsID(a_ID))
				{
					systems.push_back(system);
				}
			}
			return systems;
		}

		std::vector<AbstractECSSystem*> EntityComponentSystem::GetSystems()
		{
			return m_Systems;
		}
	}
}
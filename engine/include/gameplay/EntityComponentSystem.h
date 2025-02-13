#pragma once

#include "core/System.h"

#include <vector>
#include <string>
#include <mutex>

#include "gameplay/EntityID.h"
#include "core/Event.h"

namespace gallus
{
	namespace gameplay
	{
		class AbstractECSSystem;

		class EntityComponentSystem : public core::System
		{
		public:
			SimpleEvent<> m_OnEntitiesUpdated;
			SimpleEvent<> m_OnEntityComponentsUpdated;

			bool Initialize() override;
			bool Destroy() override;

			void Update(const float& a_DeltaTime);

			bool IsPaused() const;
			void SetPaused(bool a_Paused);

			bool HasStarted() const;
			void SetStarted(bool a_Started);

			EntityID CreateEntity(const std::string& a_Name);
			void Delete(const EntityID& a_ID);
			bool IsEntityValid(const EntityID& a_ID) const;
			void Clear();

			std::string GetUniqueName(const std::string& a_Name);

			template <class T>
			T& CreateSystem()
			{
				T* system = new T();
				m_Systems.push_back(system);
				return *system;
			}

			template <class T>
			T& GetSystem()
			{
				for (AbstractECSSystem* sys : m_Systems)
				{
					T* result = dynamic_cast<T*>(sys);
					if (result)
					{
						return *result;
					}
				}
				return CreateSystem<T>();
			};

			std::vector<EntityID>& GetEntities();
			std::vector<AbstractECSSystem*> GetSystemsContainingEntity(const EntityID& a_ID);
			std::vector<AbstractECSSystem*> GetSystems();

			std::mutex m_EntityMutex;
		private:
			void DeleteEntity(const EntityID& a_ID);
			void ClearEntities();

			bool m_Clear = false;
			std::vector<AbstractECSSystem*> m_Systems;
			std::vector<EntityID> m_Entities;
			std::vector<EntityID> m_EntitiesToDelete;
			std::vector<EntityID> m_EntitiesToAdd;
			unsigned int m_NextID = 0;
			bool m_Paused = false;
#ifdef __EDITOR__
			bool m_Started = false;
#else
			bool m_Started = true;
#endif // __EDITOR__
		};
	}
}
#pragma once
#include "Archetypes/EntityArchetype.h"
#include "VkEngine/Systems/SceneSystem.h"

namespace game
{
	class GameScene : public vke::Scene
	{
	protected:
		void Allocate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void Free(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		virtual void DefineSystems(jlb::SystemsInitializer<EntityArchetypeInfo> initializer) = 0;
		void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		
	private:
		[[nodiscard]] EntityArchetypeInfo CreateInfo(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems);

		jlb::StackAllocator _sceneAllocator{};
		jlb::SystemManager<EntityArchetypeInfo> _archetypeManager{};
	};
}

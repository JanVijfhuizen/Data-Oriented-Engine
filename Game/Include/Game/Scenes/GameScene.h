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

		[[nodiscard]] jlb::Systems<EntityArchetypeInfo> GetEntityArchetypes();
		[[nodiscard]] jlb::StackAllocator& GetAllocator();

		void OnKeyInput(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, int key, int action) override;
		void OnMouseInput(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, int key, int action) override;
		
	private:
		[[nodiscard]] EntityArchetypeInfo CreateInfo(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems);

		jlb::StackAllocator _allocator{};
		jlb::SystemManager<EntityArchetypeInfo> _archetypeManager{};
	};
}

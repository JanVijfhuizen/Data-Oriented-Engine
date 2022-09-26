#include "pch.h"
#include "Game.h"

#include "VkEngine/Archetypes/EntityArchetype.h"
#include "VkEngine/Systems/SceneSystem.h"
#include "VkEngine/Systems/ThreadPoolSystem.h"

namespace game
{
	class GameSceneSystem final : public vke::SceneSystem
	{
	protected:
		void DefineScenes(const Initializer& initializer) override
		{
			
		}
	};

	void DefineSystems(const jlb::SystemsInitializer<vke::EngineData>& initializer)
	{
		jlb::StackAllocator alloc{};
		vke::EntityArchetype<int, float, bool> archetype{};

		archetype.Allocate(alloc, 12);

		jlb::ArchetypeView<float> fView{};
		jlb::ArchetypeView<bool> bView{};
		archetype.GetViews(fView, bView);

		for (int i = 0; i < archetype.GetCount(); ++i)
		{
			float& f = fView[i];
			bool& b = bView[i];
		}


		initializer.DefineSystem<vke::ThreadPoolSystem>();
		initializer.DefineSystem<GameSceneSystem>();
	}
}

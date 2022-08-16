#include "pch.h"
#include "Archetypes/DummyArchetype.h"
#include "Systems/ResourceManager.h"
#include "VkEngine/Graphics/TextureUtils.h"

namespace game::demo
{
	void DummyArchetype::PreUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		jlb::Vector<DummyEntity>& entities)
	{
		const auto resourceSys = systems.GetSystem<ResourceManager>();

		for (auto& entity : entities)
		{
			auto& characterInput = entity.input;
			characterInput.movementDir.x = rand() % 3 - 1;
			characterInput.movementDir.y = rand() % 3 - 1;
		}

		CharacterArchetype<DummyEntity>::PreUpdate(info, systems, entities);

		const auto subTexture = resourceSys->GetSubTexture(ResourceManager::EntitySubTextures::humanoid);
		jlb::StackArray<vke::SubTexture, 2> subTexturesDivided{};
		vke::texture::Subdivide(subTexture, 2, subTexturesDivided);
		// todo something.
	}

	vke::SubTexture DummyArchetype::DefineSubTextureSet(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems)
	{
		const auto resourceSys = systems.GetSystem<ResourceManager>();
		const auto subTexture = resourceSys->GetSubTexture(ResourceManager::EntitySubTextures::humanoid);
		return subTexture;
	}
}
